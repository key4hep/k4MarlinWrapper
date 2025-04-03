#!/usr/bin/env python
#
# Copyright (c) 2019-2024 Key4hep-Project.
#
# This file is part of Key4hep.
# See https://key4hep.github.io/key4hep-doc/ for further info.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from copy import deepcopy
import re
import sys

from xml.etree.ElementTree import fromstring, ElementTree


def getProcessors(tree):
    """return a dict of all processors and their parameters"""
    processors = dict()

    procElements = tree.findall("processor")
    for proc in procElements:
        procName = proc.attrib.get("name")
        paramDict = dict()
        paramDict["type"] = proc.attrib.get("type")

        parameters = proc.findall("parameter")
        for param in parameters:
            paramName = param.attrib.get("name")
            paramDict[paramName] = getValue(param, "")
        processors[procName] = paramDict

    groupElements = tree.findall("group")
    for group in groupElements:
        groupParam = dict()
        parameters = group.findall("parameter")
        for param in parameters:
            paramName = param.attrib.get("name")
            groupParam[paramName] = getValue(param, "")

        procElements = group.findall("processor")
        for proc in procElements:
            procName = proc.attrib.get("name")
            paramDict = deepcopy(groupParam)
            paramDict["type"] = proc.attrib.get("type")

            parameters = proc.findall("parameter")
            for param in parameters:
                paramName = param.attrib.get("name")
                paramDict[paramName] = getValue(param, "")

            processors[procName] = paramDict

    return processors


def replaceConstants(value, constants):
    """Replace with constants if pattern found in value(s)"""
    formatted_array = []
    split_values = value.split()
    for val in split_values:
        captured_patterns = re.findall("\$\{\w*\}", val)
        if not captured_patterns:
            formatted_array.append(f'"{val}"')
        elif captured_patterns:
            val_format = re.sub(r"\$\{(\w*)\}", r"%(\1)s", val)
            val_format = f'"{val_format}" % CONSTANTS'
            formatted_array.append(val_format)

    return ", ".join(formatted_array)


def convertConstants(tree):
    """Find constant tags, write them to python and replace constants within themselves"""
    constants = dict()
    lines = []

    constElements = tree.findall("constants/constant")
    for const in constElements:
        constants[const.attrib.get("name")] = getValue(const)

    for key, value in constants.items():
        if not value:
            continue
        formatted_array = []
        split_values = value.split()
        if len(split_values) == 1:
            # capture all ${constant}
            captured_patterns = re.findall("\$\{\w*\}", value)
            for pattern in captured_patterns:
                # replace every ${constant} for %(constant)s
                constants[key] = re.sub(r"\$\{(\w*)\}", r"%(\1)s", constants[key])
            constants[key] = f'"{constants[key]}"'
        elif len(split_values) > 1:
            for val in split_values:
                # capture all ${constant}
                captured_patterns = re.findall("\$\{\w*\}", val)
                if len(captured_patterns) == 0:
                    formatted_array.append('"{val}"')
                elif len(captured_patterns) >= 1:
                    # replace every ${constant} for %(constant)s
                    val_format = re.sub(r"\$\{(\w*)\}", r"%(\1)s", val)
                    val_format = '"{val_format}"'
                    formatted_array.append(val_format)
            constants[key] = f"[{', '.join(formatted_array)}]"

    lines.append("\nCONSTANTS = {")
    for key in constants:
        lines.append(f'    "{key}": {constants[key]},')
    lines.append("}\n")

    lines.append("parseConstants(CONSTANTS)\n")

    return constants, lines


def getValue(element, default=None):
    if element.get("value"):
        return element.get("value").strip()
    if element.text:
        return element.text.strip()
    return default


def getGlobalDict(globalElements):
    """return dict of global parameters and values"""
    pars = dict()
    for param in globalElements:
        name = param.get("name")
        value = getValue(param)
        pars[name] = value

    return pars


def getGlobalParameters(tree):
    """compare the global parameters in the marlin steering files"""
    return getGlobalDict(tree.findall("global/parameter"))


def resolveGroup(proc, execGroup):
    procNames = []
    for member in execGroup:
        if member.get("name") == proc:
            for child in member:
                if child.tag == "processor":
                    procNames.append(child.get("name").replace(".", "_"))
    return procNames


def getExecutingProcessors(tree):
    """compare the list of processors to execute, order matters"""
    execProc = tree.findall("execute/*")
    execGroup = tree.findall("group")
    procs = []

    for proc in execProc:
        if proc.tag == "if":
            for child in proc:
                if child.tag == "processor":
                    procs.append((child.get("name").replace(".", "_"), proc.get("condition")))
        if proc.tag == "processor":
            procs.append((proc.get("name").replace(".", "_"), ""))
        if proc.tag == "group":
            for p in resolveGroup(proc.get("name"), execGroup):
                procs.append((p, ""))

    return procs


def dumpAlgList(execProcs, lines):
    """Dump the algorithms into the algList"""
    optProcessors = False
    for name, condition in execProcs:
        if condition:
            optProcessors = True
            lines.append(f"# algList.append({name})  # {condition}")
        else:
            lines.append(f"algList.append({name})")
    return optProcessors


def createHeader(lines, geo_svc=False, cellid_svc=False):
    lines.append("from Gaudi.Configuration import *\n")
    imports = ["LcioEvent", "EventDataSvc", "MarlinProcessorWrapper"]
    if geo_svc:
        imports.append("GeoSvc")
    if cellid_svc:
        imports.append("TrackingCellIDEncodingSvc")
    lines.append(f"from Configurables import {', '.join(imports)}")
    lines.append("from k4MarlinWrapper.parseConstants import *")
    lines.append("algList = []")
    lines.append("svcList = []")
    lines.append("evtsvc = EventDataSvc()\n")
    lines.append("svcList.append(evtsvc)")


def createLcioReader(lines, glob):
    lines.append("read = LcioEvent()")
    lines.append(f"read.OutputLevel = {verbosityTranslator(glob.get('Verbosity', 'DEBUG'))}")
    lines.append(f'read.Files = ["{glob.get("LCIOInputFiles")}"]')
    lines.append("algList.append(read)\n")


def createFooter(lines, glob):
    lines.append("\nfrom Configurables import ApplicationMgr")
    lines.append("ApplicationMgr( TopAlg = algList,")
    lines.append("                EvtSel = 'NONE',")
    lines.append("                EvtMax   = 10,")
    lines.append("                ExtSvc = svcList,")
    lines.append(
        f"                OutputLevel={verbosityTranslator(glob.get('Verbosity', 'DEBUG'))}"
    )
    lines.append("              )\n")


def verbosityTranslator(marlinLogLevel):
    """Translate the verbosity level from Marlin to a Gaudi level.

    Marlin log level can end with numbers, e.g., MESSAGE4
    """
    logLevelDict = {
        "DEBUG": "DEBUG",
        "MESSAGE": "INFO",
        "WARNING": "WARNING",
        "ERROR": "ERROR",
        "SILENT": "FATAL",
    }
    for level, trans in logLevelDict.items():
        if marlinLogLevel.startswith(level):
            return trans


def convertParameters(params, proc, globParams, constants):
    """convert json of parameters to Gaudi"""
    lines = []
    if "Verbosity" in params:
        lines.append(f"{proc}.OutputLevel = {verbosityTranslator(params['Verbosity'])}")

    lines.append(f'{proc}.ProcessorType = "{params.get("type")}"')
    lines.append(f"{proc}.Parameters = {{")
    for para in sorted(params):
        if para not in ["type", "Verbosity"]:
            value = params[para].replace("\n", " ")
            value = " ".join(value.split())
            lines.append(f'    "{para}": [{replaceConstants(value, constants)}],')

    lines.append("}\n")
    return lines


def filterInitDD4hep(processors, constants, tree, procList):
    """Check if an InitDD4hep Processor is in the list of processors"""
    init_proc = [k for k, v in processors.items() if v.get("type", "") == "InitializeDD4hep"]
    exec_init = list(filter(lambda x: x[0] in init_proc and not x[1], procList))

    if not exec_init:
        return []

    if len(exec_init) > 1:
        print("WARNING: Found two InitDD4hep processors in the execute section.")
        print("         Only the first one will be converted to use the GeoSvc.")

    # extract the processor from the processor conversion and from the list of
    # algs to execute
    proc = processors.pop(exec_init[0][0])
    procList[:] = [i for i in procList if i[0] != exec_init[0][0]]

    lines = [
        'geoSvc = GeoSvc("GeoSvc")',
        f"geoSvc.OutputLevel = {verbosityTranslator(proc.get('Verbosity', 'MESSAGE'))}",
        "geoSvc.EnableGeant4Geo = False",
    ]

    compactFile = proc.get("DD4hepXMLFile", None)
    if compactFile:
        value = compactFile.replace("\n", " ")
        value = " ".join(value.split())
        lines.append(f"geoSvc.detectors = [{replaceConstants(value, constants)}]")

    lines.append("svcList.append(geoSvc)\n")

    encString = proc.get("EncodingStringParameter", None)
    if encString:
        lines.extend(
            [
                'cellIDSvc = TrackingCellIDEncodingSvc("CellIDSvc")',
                "cellIDSvc.GeoSvcName = geoSvc.name()",
                f'cellIDSvc.EncodingStringParameterName = "{encString}"',
                f"cellIDSvc.OutputLevel = {verbosityTranslator(proc.get('Verbosity', 'MESSAGE'))}",
                "svcList.append(cellIDSvc)\n",
            ]
        )

    return lines


def convertProcessors(lines, processors, globParams, constants):
    """convert XML tree to list of strings"""
    for proc in processors:
        proc_name = proc.replace(".", "_")
        lines.append(f'{proc_name} = MarlinProcessorWrapper("{proc}")')
        lines += convertParameters(processors[proc], proc_name, globParams, constants)
    return lines


def findWarnIncludes(tree):
    """Check the parsed XML structure for include statements and issue a warning"""
    if any(True for _ in tree.iter("include")):
        print(
            'ERROR: Found at least one <include ref="..."/> statement in the Marlin steering file'
        )
        print("       These cannot be handled by the conversion script.")
        print(
            "       Use Marlin -n <input-file> to resolve these includes and convert the output of that"
        )

        sys.exit(1)


def generateGaudiSteering(tree):
    findWarnIncludes(tree)
    globParams = getGlobalParameters(tree)
    processors = getProcessors(tree)
    constants, constLines = convertConstants(tree)
    procExecList = getExecutingProcessors(tree)
    geoSvcLines = filterInitDD4hep(processors, constants, tree, procExecList)
    lines = []
    createHeader(lines, len(geoSvcLines) != 0, len(geoSvcLines) > 6)
    lines.extend(constLines)
    createLcioReader(lines, globParams)
    lines.extend(geoSvcLines)
    convertProcessors(lines, processors, globParams, constants)
    optProcessors = dumpAlgList(procExecList, lines)
    if optProcessors:
        print("Optional Processors were found!")
        print("Please uncomment the desired ones at the bottom of the resulting file\n")
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
        print(f"Exception when getting trees: {ex}")
        sys.exit(1)

    with open(outputfile, "w") as wf_file:
        wf_file.write("\n".join(generateGaudiSteering(tree)))


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Script to convert Marlin XML steering files into Gaudi python option files"
    )
    parser.add_argument("inputfile", help="Input Marlin XML steering file to convert")
    parser.add_argument("outputfile", help="Output Gaudi options python file")

    args = parser.parse_args()
    run(args.inputfile, args.outputfile)
