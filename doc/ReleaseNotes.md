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
