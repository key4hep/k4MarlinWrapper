# v00-04-01

* 2022-03-08 Andre Sailer ([PR#67](https://github.com/key4hep/k4MarlinWrapper/pull/67))
  - GaudiOpts: add example for FCCee_o1_v04 reconstruction based on https://github.com/iLCSoft/CLICPerformance/blob/master/fcceeConfig/fccReconstruction.xml including conversions from EDM4hep to LCIO at the start and to EDM4hep at the end
  - GaudiOpts: CLIC Reco: add some missing calo hit Contributions collections, BeamCal Collection

* 2022-03-03 Valentin Volkl ([PR#70](https://github.com/key4hep/k4MarlinWrapper/pull/70))
  - [doc] add zenodo badge to README

* 2022-03-03 Placido Fernandez Declara ([PR#69](https://github.com/key4hep/k4MarlinWrapper/pull/69))
  - add a simple logo for the project

* 2022-03-02 Valentin Volkl ([PR#62](https://github.com/key4hep/k4MarlinWrapper/pull/62))
  - convert RecoParticleVertexAssociation and MCRecoTrackParticleAssociation

* 2022-03-01 Placido Fernandez Declara ([PR#63](https://github.com/key4hep/k4MarlinWrapper/pull/63))
  - Add error message if hits and hit contributions in clusters are not the same size
  - Copy `parseConstants.py` file to Gaudi-generated genConfDir to make it available Gaudi-wide

* 2022-02-28 Placido Fernandez Declara ([PR#68](https://github.com/key4hep/k4MarlinWrapper/pull/68))
  - Update documentation
  - Add clang-format instructions
  - Add badges for tests status

* 2022-02-09 Andre Sailer ([PR#66](https://github.com/key4hep/k4MarlinWrapper/pull/66))
  - Docs: titles for reco subsections with different input formats
  - Docs: fix filename of clicRec_e4h_input.py

* 2022-02-09 Andre Sailer ([PR#65](https://github.com/key4hep/k4MarlinWrapper/pull/65))
  - CI: adapt to new container images and remove unnecessary container setup

* 2022-01-26 Andre Sailer ([PR#64](https://github.com/key4hep/k4MarlinWrapper/pull/64))
  - CI: use github hosted docker image

* 2022-01-25 Thomas Madlener ([PR#59](https://github.com/key4hep/k4MarlinWrapper/pull/59))
  - Adapting to the new immutable defaults introduced in AIDASoft/podio#205 and key4hep/EDM4hep#132.

* 2021-12-15 Placido Fernandez Declara ([PR#61](https://github.com/key4hep/k4MarlinWrapper/pull/61))
  - LCIO -> EDM4hep, and EDM4hep -> LCIO converters now have the option to convert all available collections
  - Updated docs 
  
  Needs:
  - https://github.com/key4hep/k4LCIOReader/pull/15
  - https://github.com/key4hep/EDM4hep/pull/133

* 2021-12-06 Placido Fernandez Declara ([PR#58](https://github.com/key4hep/k4MarlinWrapper/pull/58))
  - Capture Exceptions coming from Marlin and handle them.

* 2021-11-24 Placido Fernandez Declara ([PR#55](https://github.com/key4hep/k4MarlinWrapper/pull/55))
  - Test clicReconstruction sequence with EDM4hep input
  - Use converters for both EDM4hep <--> LCIO conversion in all relevant algorithms
  - Output reconstruction in EDM4hep format

* 2021-11-22 Placido Fernandez Declara ([PR#54](https://github.com/key4hep/k4MarlinWrapper/pull/54))
  - remove calls to `getReadCollections()` to comply with updated k4fwcore https://github.com/key4hep/k4FWCore/pull/70

* 2021-11-18 Valentin Volkl ([PR#51](https://github.com/key4hep/k4MarlinWrapper/pull/51))
  - [cmake] fix wrong include for targets

* 2021-11-12 Placido Fernandez Declara ([PR#50](https://github.com/key4hep/k4MarlinWrapper/pull/50))
  - Convert LCIO cellID string to metadata when using LCIO2EDM4hep conversion

# v00-04

* 2021-10-19 Valentin Volkl ([PR#49](https://github.com/key4hep/k4MarlinWrapper/pull/49))
  - [ci] use jupytext from stack

* 2021-10-07 Placido Fernandez Declara ([PR#48](https://github.com/key4hep/k4MarlinWrapper/pull/48))
  - Update CI to properly check clang-format
  - Apply clang-format to all cpp and h files

* 2021-10-07 Placido Fernandez Declara ([PR#39](https://github.com/key4hep/k4MarlinWrapper/pull/39))
  - Changes needed to have EDM4hep input when running clicReconstruction using the EDM converters
  - Needs EDM4hep: https://github.com/key4hep/EDM4hep/pull/122
  - Needs k4LCIOReader: https://github.com/key4hep/k4LCIOReader/pull/12
  - Needs MarlinTrkProcessor: https://github.com/iLCSoft/MarlinTrkProcessors/pull/49
  - Needs DDMarlinPandora: https://github.com/iLCSoft/DDMarlinPandora/pull/21
  
  - Tests now run in regular and nightly builds

* 2021-10-06 Valentin Volkl ([PR#44](https://github.com/key4hep/k4MarlinWrapper/pull/44))
  - switch to edm4hep::Const* in interfaces, for compatibility with podio > 0.13.1

* 2021-10-01 Placido Fernandez Declara ([PR#32](https://github.com/key4hep/k4MarlinWrapper/pull/32))
  - Get metadata for SimCaloHits and convert it to LCIO

* 2021-09-27 Placido Fernandez Declara ([PR#43](https://github.com/key4hep/k4MarlinWrapper/pull/43))
  - convertMarlinSteeringToGaudi: When an escape XML character was found (&), reading the XML Tree would fail. The character is nowescaped, so the marlin XML file is processed correctly

* 2021-09-27 Placido Fernandez Declara ([PR#42](https://github.com/key4hep/k4MarlinWrapper/pull/42))
  - When indicating to process more events than available, algorithm will create empty event and fail by trying to continue.
  - If no more events to read are found, the processing will stop

* 2021-09-02 Placido Fernandez Declara ([PR#41](https://github.com/key4hep/k4MarlinWrapper/pull/41))
  - Add remaining implicit conversions in LCIO->EDM4hep conversion

* 2021-08-30 Placido Fernandez Declara ([PR#40](https://github.com/key4hep/k4MarlinWrapper/pull/40))
  - Update paths for LCGEO in tests

* 2021-08-12 Placido Fernandez Declara ([PR#37](https://github.com/key4hep/k4MarlinWrapper/pull/37))
  - Remove need to indicate the type alongside collection names in the python steering file
  - For LCIO->EDM4hep directly write the collections instead of using a DataHandle
  - Update documentation for converters

* 2021-08-12 Placido Fernandez Declara ([PR#33](https://github.com/key4hep/k4MarlinWrapper/pull/33))
  - convert calohitcontributions for SimCaloHits, re-use table of collect…

* 2021-08-05 Valentin Volkl ([PR#34](https://github.com/key4hep/k4MarlinWrapper/pull/34))
  - [cmake] use BUILD_TESTING to make tests optional

* 2021-07-21 Placido Fernandez Declara ([PR#28](https://github.com/key4hep/k4MarlinWrapper/pull/28))
  - Fix checks on second parameter when sorting them.

* 2021-06-18 Valentin Volkl ([PR#31](https://github.com/key4hep/k4MarlinWrapper/pull/31))
  - [ci] use github-cvmfs action in ci

* 2021-06-18 Valentin Volkl ([PR#30](https://github.com/key4hep/k4MarlinWrapper/pull/30))
  - Add starterkit page from key4hep-doc, with test workflow

* 2021-06-17 Placido Fernandez Declara ([PR#29](https://github.com/key4hep/k4MarlinWrapper/pull/29))
  - !! Breaking change!!
  - Remove the need for an END_TAG in the options file: replace it with a map/dict of key-values
  - Adapt XML to Python converter to fit for the change
  - Adapt tests with new format

# v00-03-01

* 2021-04-26 Placido Fernandez Declara ([PR#26](https://github.com/key4hep/k4MarlinWrapper/pull/26))
  - Implement EDM4hep to LCIO converter on the fly
  - Add LCIO to EDM4hep converter on the fly through k4LCIOReader
  - Gaudi Tools to be added to Algorithms on demand on options file
  - Added testing for EDM4hep -> LCIO conversion and EDM4hep -> LCIO -> EDM4hep conversion, creating the collections artificially, in place.
  - Changed testing to get input files from HTTP key4hep
  - Added documentation on usage of converters. Updated documentation.

# v00-03

* 2021-01-19 Placido Fernandez ([PR#25](https://github.com/key4hep/k4MarlinWrapper/pull/25))
  - Adaptation and compilation for Gaudi v35

* 2020-11-09 Placido Fernandez Declara ([PR#23](https://github.com/key4hep/k4MarlinWrapper/pull/23))
  - Change every GMP mention to k4MarlinWrapper
  - Adapt CI to new name

* 2020-11-06 Placido Fernandez Declara ([PR#22](https://github.com/key4hep/k4MarlinWrapper/pull/22))
  - Change compilation to use CMVFS HSF repository
  - Remove Gaudi compilation for CI
  - Adapt tests to use new environment variables

* 2020-11-06 Placido Fernandez Declara ([PR#21](https://github.com/key4hep/k4MarlinWrapper/pull/21))
  - steeringFileConverter
      - Add constants parsing of XML files
      - Add test case for multiple constants, lists and nested constants replacing
  
  - Removed BOOST dependency

* 2020-10-07 Placido Fernandez Declara ([PR#19](https://github.com/key4hep/k4MarlinWrapper/pull/19))
  - Added test which invokes the XML to Python converter
  - The same test runs and checks the complete clicReconstruction

* 2020-10-05 Placido Fernandez Declara ([PR#17](https://github.com/key4hep/k4MarlinWrapper/pull/17))
  - convertMarlinSteeringToGaudi.py
    - The script will now generate the output file, instead of printing it. 
    - This will break whatever scripts are using the converter by piping its output to generate a file.
    - It now warns on output about having to uncomment optional processors.

# v00-02-01

* 2020-09-21 Andre Sailer ([PR#15](https://github.com/key4hep/GMP/pull/15))
  - convertMarlinSteeringToGaudi: translate all possible Marlin LogLevels to Gaudi levels

# v00-02

* 2020-09-21 Andre Sailer ([PR#13](https://github.com/key4hep/GMP/pull/13))
  - MarlinWrapper: make sure the random seeds are refreshed, fixes #12

* 2020-09-21 Placido Fernandez Declara ([PR#11](https://github.com/key4hep/GMP/pull/11))
  - Use standard Gaudi locations for executables

* 2020-07-31 Placido Fernandez Declara ([PR#10](https://github.com/key4hep/GMP/pull/10))
  - CMake improvements and modularization
  - CPP modernization

* 2020-07-14 Placido Fernandez Declara ([PR#9](https://github.com/key4hep/GMP/pull/9))
  - convertMarlinSteeringToGaudi: Fix parameter parse exceptions caused by empty strings in parameter list.
  - convertMarlinSteeringToGaudi: Add algorithms from if statements in algList as comments

# v00-01

# Version 0.1.0

* 2020-06-15 Placido Fernandez Declara
    Add versioning
