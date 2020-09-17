#!/usr/bin/env python
from __future__ import absolute_import, unicode_literals, print_function

import sys
from copy import deepcopy

from xml.etree.ElementTree import ElementTree


def getTree(xmlFile):
  """ parse file and return XML tree """
  et = ElementTree()
  et.parse(xmlFile)
  return et


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

  for proc in execProc:
    if proc.tag == "if":
      for child in proc:
        if child.tag == "processor":
          # lines.append("# algList.append(%s)" % child.get('name').replace(".", "_"))
          lines.append("# algList.append(%s)  # %s" % (child.get('name').replace(".", "_"), proc.get('condition')))
    if proc.tag == "processor":
      lines.append("algList.append(%s)" % proc.get('name'))
    if proc.tag == "group":
      resolveGroup(lines, proc.get('name'), execGroup)


def createHeader(lines):
  lines.append("from Gaudi.Configuration import *\n")
  lines.append("from Configurables import LcioEvent, EventDataSvc, MarlinProcessorWrapper")
  lines.append("algList = []")
  lines.append("evtsvc = EventDataSvc()\n")
  lines.append("END_TAG = \"END_TAG\"\n")


def createLcioReader(lines, glob):
  lines.append("read = LcioEvent()")
  lines.append("read.OutputLevel = %s" % glob.get("Verbosity", "DEBUG"))
  lines.append("read.Files = [\"%s\"]" % glob.get("LCIOInputFiles"))
  lines.append("algList.append(read)\n")


def createFooter(lines, glob):
  lines.append("\nfrom Configurables import ApplicationMgr")
  lines.append("ApplicationMgr( TopAlg = algList,")
  lines.append("                EvtSel = 'NONE',")
  lines.append("                EvtMax   = 10,")
  lines.append("                ExtSvc = [evtsvc],")
  lines.append("                OutputLevel=%s" % glob.get("Verbosity", "DEBUG"))
  lines.append("              )\n")


def convertParamters(params, proc, globParams):
  """ convert json of parameters to gaudi """
  lines = []
  lines.append("%s.OutputLevel = %s " % (proc.replace(".", "_"), globParams.get("Verbosity")))
  lines.append("%s.ProcessorType = \"%s\" " % (proc.replace(".", "_"), params.get("type")))
  lines.append("%s.Parameters = [" % proc.replace(".", "_"))
  for para in sorted(params):
    if para not in ["type", "Verbosity"]:
      value = params[para].replace('\n', ' ')
      value = " ".join(value.split())
      value = value.replace(" ", "\", \"")

      if not value:
        lines.append("%s\"%s\", END_TAG," % (' ' * (len(proc) + 15), para))
      else:
        lines.append("%s\"%s\", \"%s\", END_TAG," % (' ' * (len(proc) + 15), para, value))

  lines[-1] = lines[-1][:-1]
  lines.append("%s]\n" % (' ' * (len(proc) + 15)))
  return lines


def convertProcessors(lines, tree, globParams):
  """ convert XML tree to list of strings """
  processors = getProcessors(tree)
  for proc in processors:
    lines.append("%s = MarlinProcessorWrapper(\"%s\")" % (proc.replace(".", "_"), proc))
    lines += convertParamters(processors[proc], proc, globParams)
  return lines


def generateGaudiSteering(tree):
  globParams = getGlobalParameters(tree)
  lines = []
  createHeader(lines)
  createLcioReader(lines, globParams)
  convertProcessors(lines, tree, globParams)
  getExecutingProcessors(lines, tree)
  createFooter(lines, globParams)
  return lines


def run():
  args = sys.argv
  if len(args) != 2:
    print("incorrect number of input files, need one marlin steering files as argument")
    print("convertMarlinSteeringToGaudi.py file1.xml")
    exit(1)

  try:
    tree = getTree(args[1])
  except Exception as ex:
    print("Exception when getting trees: %r " % ex)
    exit(1)

  print("\n".join(generateGaudiSteering(tree)))


if __name__ == "__main__":
  run()
