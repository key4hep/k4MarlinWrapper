#!/usr/bin/env python3
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

import sys
import logging

from Configurables import (
    LcioEvent,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
    Lcio2EDM4hepTool,
)

logger = logging.getLogger()


def _is_wrapped_proc_without_conv(alg, from_edm, to_edm):
    """Check if this algorithm has a configured (i.e. named) converter attached
    for the direction described by from_edm and to_edm"""
    if isinstance(alg, MarlinProcessorWrapper):
        if not getattr(alg, f"{from_edm}2{to_edm}Tool").getName():
            return True

    return False


class IOHandlerHelper:
    """Helper class to facilitate the transparent handling of LCIO or EDM4hep
    inputs and outputs.

    This class allows to
    - add the correct reader as determined from the input file name(s)
    - add (multiple) LCIO writers
    - add an EDM4hep writer
    - make sure that the necessary converters are introduced at the right places
    """

    def __init__(self, alg_list, io_svc):
        """Create a IOHandlerHelper

        Args:
            alg_list (list): The algorithm list which is being populated
            io_svc (IOSvc): The IOSvc that is being used for this run
        """
        self._alg_list = alg_list
        self._io_svc = io_svc
        self._lcio_input = False
        self._edm4hep_output = False

    def add_reader(self, input_files):
        """Add a reader that is equipped to read the passed files

        If the input is LCIO the necessary algorithm will be configured and
        added to the list of algorithms at the current spot. If the input is
        EDM4hep the file names will be passed to the IOSvc.Input

        Args:
            input_files (list): The input files that should be read

        """
        if input_files[0].endswith(".slcio"):
            if any(not f.endswith(".slcio") for f in input_files):
                logger.error("All input files need to have the same format (LCIO)")
                sys.exit(1)

            read = LcioEvent()
            read.Files = input_files
            self._alg_list.append(read)
            self._lcio_input = True
        else:
            if any(not f.endswith(".root") for f in input_files):
                logger.error("All input files need to have the same format (EDM4hep)")
                sys.exit(1)

            self._io_svc.Input = input_files

    def add_lcio_writer(self, alg_name):
        """Add a writer for LCIO output at the current spot in the algorithm list

        Note:
            This doesn't configure anything yet, that is still left to do outside

        Args:
            alg_name (str): The name this writer should have

        Returns:
            MarlinProcessorWrapper: The wrapped processor that has just been
                inserted into the algorithm list and that now needs to be
                further configured
        """
        writer = MarlinProcessorWrapper(alg_name, ProcessorType="LCIOOutputProcessor")
        self._alg_list.append(writer)
        return writer

    def add_edm4hep_writer(self, output_file, output_cmds=["keep *"]):
        """Add an EDM4hep writer at the very end of the algorithm execution

        This will pass the output file name as well as the output commands to
        the IOSvc.Ouptut and IOSvc.outputCommands respectively

        Args:
            output_file (str): The name of the output file

            output_cmds (list, optional): The list of output commands that
                should be applied. Defaults to ["keep *"]
        """
        self._io_svc.Output = output_file
        self._io_svc.outputCommands = output_cmds
        self._edm4hep_output = True

    def finalize_converters(self):
        """Attach the appropriate converters in all places they are necessary

        Go through the algorihtm list and determine where appropriate converters
        need be inserted such that the algorithms or wrapped processors always
        see a consistent picture of the event in both formats. Basically what
        this does is to go through the complete list of algorithms and introduce
        an LCIO to EDM4hep converter at the start of every run of
        MarlinProcessorWrappers and in the other direction at the end of every
        such run

        Note:
            Call this just before you pass the algorithm list to the ApplicationMgr

        Note:
            This will not change existing converters on wrapped processors
        """
        for alg, next_alg in zip(self._alg_list, self._alg_list[1:]):
            if not isinstance(next_alg, MarlinProcessorWrapper) and _is_wrapped_proc_without_conv(
                alg, "Lcio", "EDM4hep"
            ):
                # We change from a run of wrapped processors to algorithms and
                # we don't have a converter yet
                output_conv = Lcio2EDM4hepTool(f"{alg.getName()}_OutputConverter")
                output_conv.convertAll = True
                output_conv.collNameMapping = {"MCParticle": "MCParticles"}
                alg.Lcio2EDM4hepTool = output_conv
                logger.info(
                    f"Added an output converter (LCIO to EDM4hep) to the {alg.getName()} algorithm"
                )

            if not isinstance(
                alg, (MarlinProcessorWrapper, LcioEvent)
            ) and _is_wrapped_proc_without_conv(next_alg, "EDM4hep", "Lcio"):
                # We change from a run of algorithms to wrapped processors and
                # we do not have a converter yet
                input_conv = EDM4hep2LcioTool(f"{next_alg.getName()}_InputConverter")
                input_conv.convertAll = True
                input_conv.collNameMapping = {"MCParticles": "MCParticle"}
                next_alg.EDM4hep2LcioTool = input_conv
                logger.info(
                    f"Added an input converter (EDM4hep to LCIO) to the {next_alg.getName()} algorithm"
                )

        if not self._lcio_input:
            # We need to convert the input to LCIO from EDM4hep. We attach this
            # to the first wrapped processor that does NOT have another converter
            # configured
            for alg in self._alg_list:
                if _is_wrapped_proc_without_conv(alg, "EDM4hep", "Lcio"):
                    input_conv = EDM4hep2LcioTool("InputConversion")
                    input_conv.convertAll = True
                    input_conv.collNameMapping = {"MCParticles": "MCParticle"}
                    alg.EDM4hep2LcioTool = input_conv
                    logger.info(
                        f"Added an input converter (EDM4hep to LCIO) to the {alg.getName()} algorithm"
                    )
                    break

        if self._edm4hep_output:
            # We need to convert to EDM4hep. We attach the converter to the last
            # wrapped processor that thas not have another converter attached
            for alg in reversed(self._alg_list):
                if _is_wrapped_proc_without_conv(alg, "Lcio", "EDM4hep"):
                    output_conv = Lcio2EDM4hepTool("OutputConverter")
                    output_conv.convertAll = True
                    output_conv.collNameMapping = {"MCParticle": "MCParticles"}
                    alg.Lcio2EDM4hepTool = output_conv
                    logger.info(
                        f"Added an output converter (LCIO to EDM4hep) to the {alg.getName()} algorithm"
                    )
                    break
