#!/usr/bin/env python
from __future__ import absolute_import, unicode_literals, print_function

from copy import deepcopy
import re
import sys

from xml.etree.ElementTree import fromstring, ElementTree


def getProcessors(tree):
  """ return a dict of all processors and their parameters """
  processors = dict()

  procElements = tree.findall('processor')
  for proc in procElements:
    procName = proc.attrib.get("name")
    paramDict = dict()
    paramDict["type"] = proc.attrib.get("type")

    parameters = proc.findall('parameter')
    for param in parameters:
      paramName = param.attrib.get('name')
      paramDict[paramName] = getValue(param, "")
    processors[procName] = paramDict

  groupElements = tree.findall('group')
  for group in groupElements:
    groupParam = dict()
    parameters = group.findall('parameter')
    for param in parameters:
      paramName = param.attrib.get('name')
      groupParam[paramName] = getValue(param, "")

    procElements = group.findall("processor")
    for proc in procElements:
      procName = proc.attrib.get("name")
      paramDict = deepcopy(groupParam)
      paramDict["type"] = proc.attrib.get("type")

      parameters = proc.findall('parameter')
      for param in parameters:
        paramName = param.attrib.get('name')
        paramDict[paramName] = getValue(param, "")

      processors[procName] = paramDict

  return processors


def replaceConstants(value, constants):
  """Replace with constants if pattern found in value(s) """
  formatted_array = []
  split_values = value.split()
  for val in split_values:
    captured_patterns = re.findall('\$\{\w*\}', val)
    if not captured_patterns:
      formatted_array.append('\"{}\"'.format(val))
    elif captured_patterns:
      val_format = re.sub(r'\$\{(\w*)\}', r'%(\1)s', val)
      val_format = '\"{}\" % CONSTANTS'.format(val_format)
      formatted_array.append(val_format)

  return ", ".join(formatted_array)


def convertConstants(lines, tree):
  """ Find constant tags, write them to python and replace constants within themselves """
  constants = dict()

  constElements = tree.findall('constants/include')
  if constElements:
    print('WARNING: include statements found inside constants tag')

  constElements = tree.findall('constants/constant')
  for const in constElements:
    constants[const.attrib.get('name')] = getValue(const)

  for key, value in constants.items():
    if not value:
      continue
    formatted_array = []
    split_values = value.split()
    if len(split_values) == 1:
      """ capture all ${constant} """
      captured_patterns = re.findall('\$\{\w*\}', value)
      for pattern in captured_patterns:
        """ replace every ${constant} for %(constant)s """
        constants[key] = re.sub(r'\$\{(\w*)\}', r'%(\1)s', constants[key])
      constants[key] = "\"{}\"".format(constants[key])
    elif len(split_values) > 1:
      for val in split_values:
        """ capture all ${constant} """
        captured_patterns = re.findall('\$\{\w*\}', val)
        if len(captured_patterns) == 0:
          formatted_array.append("\"{}\"".format(val))
        elif len(captured_patterns) >= 1:
          """ replace every ${constant} for %(constant)s """
          val_format = re.sub(r'\$\{(\w*)\}', r'%(\1)s', val)
          val_format = "\"{}\"".format(val_format)
          formatted_array.append(val_format)
      constants[key] = '[{}]'.format(", ".join(formatted_array))

  lines.append("\nCONSTANTS = {")
  for key in constants:
    lines.append(' ' * len('CONSTANTS = {') + "'{}': {},".format(key, constants[key]))
  lines.append("}\n")

  lines.append("parseConstants(CONSTANTS)\n")

  return constants


def getValue(element, default=None):
  if element.get('value'):
    return element.get('value').strip()
  if element.text:
    return element.text.strip()
  return default


def getGlobalDict(globalElements):
  """return dict of global parameters and values """
  pars = dict()
  for param in globalElements:
    name = param.get('name')
    value = getValue(param)
    pars[name] = value

  return pars


def getGlobalParameters(tree):
  """ compare the global parameters in the marlin steering files """
  return getGlobalDict(tree.findall('global/parameter'))


def resolveGroup(lines, proc, execGroup):
  for member in execGroup:
    if member.get('name') == proc:
      for child in member:
        if child.tag == "processor":
          lines.append("algList.append(%s)" % child.get('name').replace(".", "_"))


def getExecutingProcessors(lines, tree):
  """ compare the list of processors to execute, order matters """
  execProc = tree.findall('execute/*')
  execGroup = tree.findall('group')
  optProcessors = False

  for proc in execProc:
    if proc.tag == "if":
      for child in proc:
        if child.tag == "processor":
          optProcessors = True
          lines.append("# algList.append(%s)  # %s" % (child.get('name').replace(".", "_"), proc.get('condition')))
    if proc.tag == "processor":
      lines.append("algList.append(%s)" % proc.get('name'))
    if proc.tag == "group":
      resolveGroup(lines, proc.get('name'), execGroup)
  return optProcessors


def createHeader(lines):
  lines.append("from Gaudi.Configuration import *\n")
  lines.append("from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper")
  lines.append("from k4MarlinWrapper.parseConstants import *")
  lines.append("algList = []")
  lines.append("evtsvc = EventDataSvc()\n")
  # lines.append("END_TAG = \"END_TAG\"\n")


def createLcioReader(lines, glob):
  lines.append("read = LcioEvent()")
  lines.append("read.OutputLevel = %s" % verbosityTranslator(glob.get("Verbosity", "DEBUG")))
  lines.append("read.Files = [\"%s\"]" % glob.get("LCIOInputFiles"))
  lines.append("algList.append(read)\n")


def createFooter(lines, glob):
  lines.append("\nfrom Configurables import ApplicationMgr")
  lines.append("ApplicationMgr( TopAlg = algList,")
  lines.append("                EvtSel = 'NONE',")
  lines.append("                EvtMax   = 10,")
  lines.append("                ExtSvc = [evtsvc],")
  lines.append("                OutputLevel=%s" % verbosityTranslator(glob.get("Verbosity", "DEBUG")))
  lines.append("              )\n")


def verbosityTranslator(marlinLogLevel):
  """Translate the verbosity level from Marlin to a Gaudi level.

  Marlin log level can end with numbers, e.g., MESSAGE4
  """
  logLevelDict = {'DEBUG': 'DEBUG',
                  'MESSAGE': 'INFO',
                  'WARNING': 'WARNING',
                  'ERROR': 'ERROR',
                  'SILENT': 'FATAL',
                  }
  for level, trans in logLevelDict.items():
    if marlinLogLevel.startswith(level):
      return trans


def convertParamters(params, proc, globParams, constants):
  """ convert json of parameters to gaudi """
  lines = []
  lines.append("%s.OutputLevel = %s " % (proc.replace(".", "_"), verbosityTranslator(globParams.get("Verbosity"))))
  lines.append("%s.ProcessorType = \"%s\" " % (proc.replace(".", "_"), params.get("type")))
  lines.append("%s.Parameters = {" % proc.replace(".", "_"))
  for para in sorted(params):
    if para not in ["type", "Verbosity"]:
      value = params[para].replace('\n', ' ')
      value = " ".join(value.split())
      lines.append("%s\"%s\": [%s]," % \
        (' ' * (len(proc) + 15), para, replaceConstants(value, constants)))

  lines[-1] = lines[-1][:-1]
  lines.append("%s}\n" % (' ' * (len(proc) + 15)))
  return lines


def convertProcessors(lines, tree, globParams, constants):
  """ convert XML tree to list of strings """
  processors = getProcessors(tree)
  for proc in processors:
    lines.append("%s = MarlinProcessorWrapper(\"%s\")" % (proc.replace(".", "_"), proc))
    lines += convertParamters(processors[proc], proc, globParams, constants)
  return lines


def generateGaudiSteering(tree):
  globParams = getGlobalParameters(tree)
  lines = []
  createHeader(lines)
  constants = convertConstants(lines, tree)
  createLcioReader(lines, globParams)
  convertProcessors(lines, tree, globParams, constants)
  optProcessors = getExecutingProcessors(lines, tree)
  if optProcessors:
    print('Optional Processors were found!')
    print('Please uncomment the desired ones at the bottom of the resulting file\n')
  createFooter(lines, globParams)
  return lines


def escapeIllegalChars(file_str):
  return file_str.replace("&&", "&amp;&amp;")


def run(inputfile, outputfile):
  with open(inputfile, "r+") as infile:
    escaped_str = escapeIllegalChars(infile.read())

  try:
    tree = ElementTree(fromstring(escaped_str))
  except Exception as ex:
    print("Exception when getting trees: %r " % ex)
    sys.exit(1)

  with open(outputfile, 'w') as wf_file:
    wf_file.write("\n".join(generateGaudiSteering(tree)))


if __name__ == "__main__":
  import argparse
  parser = argparse.ArgumentParser(description='Script to convert Marlin XML steering files into Gaudi python option files')
  parser.add_argument('inputfile', help='Input Marlin XML steering file to convert')
  parser.add_argument('outputfile', help='Output Gaudi options python file')

  args = parser.parse_args()
  run(args.inputfile, args.outputfile)
