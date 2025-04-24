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

from Configurables import (
    LcioEvent,
    MarlinProcessorWrapper,
    EDM4hep2LcioTool,
    Lcio2EDM4hepTool,
)


from k4FWCore.utils import get_logger

logger = get_logger()


class IOHandlerHelper:
    def __init__(self, alg_list, io_svc):
        self._alg_list = alg_list
        self._io_svc = io_svc
        self._lcio_input = False
        self._edm4hep_output = False
        self._lcio_writers = []

    def add_reader(self, input_files):
        """Add a reader that is equipped to read the passed files and make sure
        it appears on the list of algorithms if necessary
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
        """Add a writer for LCIO output. Note, this doesn't configure anything
        yet, that is still left to do outside"""
        writer = MarlinProcessorWrapper(alg_name, ProcessorType="LCIOOutputProcessor")

        self._alg_list.append(writer)
        self._lcio_writers.append(writer)

        return writer

    def add_edm4hep_writer(self, output_file, output_cmds):
        """Add an EDM4hep writer"""
        self._io_svc.Output = output_file
        self._io_svc.outputCommands = output_cmds
        self._edm4hep_output = True

    def finalize_converters(self):
        """Attach the necessary converters to make sure data are available in
        the desired format always"""
        # Check if we have only MarlinWrapper algorithms
        only_wrappers = all(
            isinstance(a, (LcioEvent, MarlinProcessorWrapper)) for a in self._alg_list
        )
        if only_wrappers and self._lcio_input and not self._edm4hep_output:
            # We have LCIO input and LCIO output and no need to ever convert. We
            # are done here
            logger.debug(
                "Not adding any converters, because everything is done in LCIO"
            )
            return

        if not self._lcio_input:
            # We need to convert the input to LCIO from EDM4hep. We attach this
            # to the first algorithm that does NOT have another converter
            # configured
            for alg in self._alg_list:
                if (
                    isinstance(alg, (LcioEvent, MarlinProcessorWrapper))
                    and not alg.EDM4hep2LcioTool.getName()
                ):
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
                if (
                    isinstance(alg, MarlinProcessorWrapper)
                    and not alg.Lcio2EDM4hepTool.getName()
                ):
                    output_conv = Lcio2EDM4hepTool("OutputConverter")
                    output_conv.convertAll = True
                    output_conv.collNameMapping = {"MCParticle": "MCParticles"}
                    alg.Lcio2EDM4hepTool = output_conv
                    logger.info(
                        f"Added an output converter (LCIO to EDM4hep) to the {alg.getName()} algorithm"
                    )
                    break

        if not only_wrappers:
            if self._lcio_input:
                # Attach a converter to the last wrapped processor BEFORE the
                # first non-wrapper algorithm
                for alg, next_alg in zip(self._alg_list, self._alg_list[1:]):
                    if (
                        isinstance(alg, MarlinProcessorWrapper)
                        and not isinstance(next_alg, (MarlinProcessorWrapper))
                        and not alg.Lcio2EDM4hepTool.getName()
                    ):
                        input_conv = Lcio2EDM4hepTool(
                            f"{alg.getName()}_OutputConverter"
                        )
                        input_conv.convertAll = True
                        input_conv.collNameMapping = {"MCParticle": "MCParticles"}
                        alg.Lcio2EDM4hepTool = input_conv
                        logger.info(
                            f"Added an input converter (LCIO to EDM4hep) to the {alg.getName()} algorithm"
                        )
                        break

            for writer in self._lcio_writers:
                # Attach a converter to every LCIOOutputProcessor just to be
                # sure they always see the correct event content
                writer_conv = EDM4hep2LcioTool(f"{writer.getName()}_InputConverter")
                writer_conv.collNameMapping = {"MCParticles": "MCParticle"}
                writer_conv.convertAll = True
                writer.EDM4hep2LcioTool = writer_conv
                logger.info(
                    f"Added an output converter (EDM4hep to LCIO) to the {writer.getName()} algorithm"
                )
