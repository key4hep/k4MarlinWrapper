# v00-12

* 2025-05-28 Victor Schwan ([PR#240](https://github.com/key4hep/k4MarlinWrapper/pull/240))
  - Introduced a `NamedTuple` (`ColDrawConfig`) to capture the true substructure of each configuration entry.
  - Refactored the configuration list to be a list of these structured entries.
  - Added a one-liner flattening back to the old list format for backward compatibility with legacy code.
  - General code formatting
  
  Previously, the configuration was stored as one extremely long, flat list, ignoring the logical grouping of each entry’s components (name, marker type, size, CED draw layer). This mismatch caused multiple problems:
  
  - Autoformatters spread the list over many lines.
  - Workarounds like disabling autoformatting on this section were a hotfix, not a real solution.

* 2025-05-27 Thomas Madlener ([PR#241](https://github.com/key4hep/k4MarlinWrapper/pull/241))
  - Switch to an Ubuntu 24 runner to run pre-commit

* 2025-05-22 Thomas Madlener ([PR#237](https://github.com/key4hep/k4MarlinWrapper/pull/237))
  - Make sure that Link collections are properly skipped from conversion if they already exist in the LCIO event

* 2025-04-29 Thomas Madlener ([PR#234](https://github.com/key4hep/k4MarlinWrapper/pull/234))
  - Introduce the `io_helpers.IOHandlerHelper` class that makes it possible to more easily transparently handle LCIO and EDM4hep inputs and outputs as well as the necessary conversions between them.
    - Fully based on the `IOSvc` so that the `PodioInput`, `PodioOutput` and `k4DataSvc` are no longer necessary for mixing and matching
  - Deprecate the `inputReader` module and suggest people to switch to the `io_helpers` instead.

* 2025-04-03 Thomas Madlener ([PR#230](https://github.com/key4hep/k4MarlinWrapper/pull/230))
  - Replace instances of `InitializeDD4hep` processors with `GeoSvc` and `TrackingCellIDEncodingSvc` (if necessary) in the conversion of Marlin steering files to Gaudi options files.

* 2025-03-28 Thomas Madlener ([PR#233](https://github.com/key4hep/k4MarlinWrapper/pull/233))
  - Unconditionally check the TES when looking for EDM4hep collections to convert in case `convertAll` is set to `True`.
  - Restructure conversion slightly and only do this lookup once and cache the results from the first conversion. This is possible since event contents are assumed to be constant.

* 2025-03-11 Thomas Madlener ([PR#232](https://github.com/key4hep/k4MarlinWrapper/pull/232))
  - Improve the debug log message for where EventHeader collections are converted to.

* 2025-03-11 Thomas Madlener ([PR#229](https://github.com/key4hep/k4MarlinWrapper/pull/229))
  - Make sure to read the `"EventHeader"` collection in the tests as it will be converted unconditionally and lead to termination if it's not found.

* 2025-03-11 Thomas Madlener ([PR#228](https://github.com/key4hep/k4MarlinWrapper/pull/228))
  - Switch to f-strings for formatting strings in the steering file converter script
  - Remove python2 compatibility
  - Fix erroneous removal of opening `{` when the parameters of a Marlin Processor are empty (see #222)

* 2025-03-11 Mateusz Jakub Fila ([PR#227](https://github.com/key4hep/k4MarlinWrapper/pull/227))
  - Update docs to use `IOSvc` instead of `k4DataSvc`. Use`IOSvc` by the default in the examples

* 2025-03-11 jmcarcell ([PR#225](https://github.com/key4hep/k4MarlinWrapper/pull/225))
  - Fix missing collections with IOSvc when using `convertAll=True`
  - Add a test that would fail without this fix
  - Rename `PseudoRecoAlgorithm` to `PseudoRecoFunctional` and create a new  `PseudoRecoAlgorithm` that implements a `Gaudi::Algorithm`

* 2025-02-21 Thomas Madlener ([PR#223](https://github.com/key4hep/k4MarlinWrapper/pull/223))
  - EDM4hep2Lcio: Make an exception message more useful by including the collection name for which retrieval fails

* 2025-02-17 Thomas Madlener ([PR#221](https://github.com/key4hep/k4MarlinWrapper/pull/221))
  - Fix the `trackerHitPlanes` name of the map going from EDM4hep to LCIO to make it work properly.
  - Remove some no longer necessary special handling of ParticleID and TrackerHitPlane maps.

* 2025-02-17 Thomas Madlener ([PR#220](https://github.com/key4hep/k4MarlinWrapper/pull/220))
  - Add a `skipNEvents` property to the `LcioEvent` algorithm to make it possible to skip the first N input LCIO events.
    - Assuming default algorithm name, `--LcioEvent.skipNEvents=<N>` can be used to change this parameter from the command line.

* 2025-02-10 jmcarcell ([PR#218](https://github.com/key4hep/k4MarlinWrapper/pull/218))
  - Try to retrieve the MetadataSvc only when not using PodioDataSvc to avoid a lookup error when using the Marlin wrapper.

# v00-11

* 2025-02-06 Thomas Madlener ([PR#217](https://github.com/key4hep/k4MarlinWrapper/pull/217))
  - Require the minimal version of k4EDM4hep2LcioConv to be 0.11 as that is a working tag

* 2025-02-05 jmcarcell ([PR#216](https://github.com/key4hep/k4MarlinWrapper/pull/216))
  - Add support for running the wrapper with `IOSvc` with a few tests:
    - Run the CLIC reconstruction with `IOSvc` (proves processors run under IOSvc, producing the same output).
    - Run `global_converter_maps` with `IOSvc`, where a functional algorithm produces output that is used by a Marlin processor and the output of the two is used either by a `Gaudi::Algorithm` or a functional algorithm (proves processors can take input from functional algorithms and functional algorithms can take input from processors).
    - Run `test_link_conversion_edm4hep` with `IOSvc`, where links produced by functional algorithms are checked by Marlin processors.

* 2024-12-16 Thomas Madlener ([PR#210](https://github.com/key4hep/k4MarlinWrapper/pull/210))
  - Stop the reading of the LCIO events early enough, such that no empty event is processed after the last event (which breaks "regular" Algorithms in the same chain)
  - Make it a hard error (exception) when reading an event fails
  - Add documentation about the small conceptual differences that might lead to unexpected results

* 2024-12-15 Mateusz Jakub Fila ([PR#213](https://github.com/key4hep/k4MarlinWrapper/pull/213))
  - Fixed implicit conversions int to bool, creating temporary to be emplaced in collection

* 2024-12-13 Thomas Madlener ([PR#211](https://github.com/key4hep/k4MarlinWrapper/pull/211))
  - Make sure to not fail the example in case of some spuriously missing ParticleID object that is not consistently created by LCFIPlus

* 2024-12-12 Mateusz Jakub Fila ([PR#212](https://github.com/key4hep/k4MarlinWrapper/pull/212))
  - Updated link to TBB in documentation and fixed typos

* 2024-12-10 jmcarcell ([PR#209](https://github.com/key4hep/k4MarlinWrapper/pull/209))
  - Remove the check for TrackerHit3D from edm4hep

* 2024-12-10 jmcarcell ([PR#207](https://github.com/key4hep/k4MarlinWrapper/pull/207))
  - Don't use the deprecated service retrieval
  - Add `final` when possible and use the default destructors instead of implementing them
  - Use `!` instead of `not` in `if(...)`
  - Use `!` instead of comparing to `nullptr`
  - Simplify setting the environment for tests, avoiding having to duplicate the names
  - Clean up some includes in header and source files

* 2024-12-10 Thomas Madlener ([PR#206](https://github.com/key4hep/k4MarlinWrapper/pull/206))
  - Add `ruff` configuration and add it to `pre-commit`
  - Format all python source files to make them conformant

* 2024-12-09 Thomas Madlener ([PR#205](https://github.com/key4hep/k4MarlinWrapper/pull/205))
  - Only set the `OutputLevel` for converted MarlinProcessors if `Verbosity` was set in the original XML steering file

# v00-10

* 2024-10-28 jmcarcell ([PR#204](https://github.com/key4hep/k4MarlinWrapper/pull/204))
  - Retrieve unique_ptrs for collections produced by functional algorithms. Needed after https://github.com/key4hep/k4FWCore/pull/250. It should also be able to handle the case for the previous approach with `std::shared_ptr`.

# v00-09

* 2024-09-11 jmcarcell ([PR#203](https://github.com/key4hep/k4MarlinWrapper/pull/203))
  - Use the Key4hepConfig flag to set the standard, compiler flags and rpath magic.
  - Fiw a few warnings about shadowing variables

* 2024-09-09 tmadlener ([PR#200](https://github.com/key4hep/k4MarlinWrapper/pull/200))
  - Switch from `Association` to `Link` in the conversion from EDM4hep to LCIO to accomodate for the new naming from ([EDM4hep#341](https://github.com/key4hep/EDM4hep/pull/341))
  - Add test to make sure that conversion of Links from EDM4hep to LCIO works as expected

* 2024-08-06 jmcarcell ([PR#198](https://github.com/key4hep/k4MarlinWrapper/pull/198))
  - Use Gaudi__Sequencer instead of the Sequencer coming from GaudiAlg

* 2024-08-06 jmcarcell ([PR#197](https://github.com/key4hep/k4MarlinWrapper/pull/197))
  - Move Associations to Links

* 2024-08-02 tmadlener ([PR#195](https://github.com/key4hep/k4MarlinWrapper/pull/195))
  - Bump the minimum required version of EDM4hep to 0.99

* 2024-08-01 tmadlener ([PR#196](https://github.com/key4hep/k4MarlinWrapper/pull/196))
  - Default to building with c++20 and remove support for c++14 and c++17, add possibility to build with c++23

* 2024-07-30 tmadlener ([PR#194](https://github.com/key4hep/k4MarlinWrapper/pull/194))
  - Make sure to attach dQ/dx information to LCIO tracks when converting from EDM4hep

* 2024-07-29 jmcarcell ([PR#165](https://github.com/key4hep/k4MarlinWrapper/pull/165))
  - Change headers and add EventContext in algorithms not to use GaudiAlg
  - Replace `GaudiTool` with `AlgTool`

* 2024-07-19 tmadlener ([PR#189](https://github.com/key4hep/k4MarlinWrapper/pull/189))
  - Add the conversion of `Association` collections to `LCRelation` collections after the functionality has been added to the converter library ([key4hep/k4EDM4hep2LcioConv#81](https://github.com/key4hep/k4EDM4hep2LcioConv/pull/81)

* 2024-07-09 jmcarcell ([PR#188](https://github.com/key4hep/k4MarlinWrapper/pull/188))
  - Add support for running together with functional algorithms
  - Add a test that creates MCParticles in EDM4hep with a functional algorithm that are later used by Marlin processors

* 2024-06-20 jmcarcell ([PR#187](https://github.com/key4hep/k4MarlinWrapper/pull/187))
  - Fix warnings after https://github.com/key4hep/EDM4hep/pull/315
  - Fix a warning by adding a default case in a switch statement

* 2024-06-09 tmadlener ([PR#185](https://github.com/key4hep/k4MarlinWrapper/pull/185))
  - Make sure to convert `TrackerHitPlane` and `TrackerHit3D` from EDM4hep to LCIO

* 2024-05-24 Leonhard Reichenbach ([PR#184](https://github.com/key4hep/k4MarlinWrapper/pull/184))
  - Add lcio to edm4hep event parameter conversion

* 2024-05-01 tmadlener ([PR#181](https://github.com/key4hep/k4MarlinWrapper/pull/181))
  - Make the on-the-fly EDM conversion deal with the reversed ParticleID relations and the related metadata

* 2024-04-24 Leonhard Reichenbach ([PR#183](https://github.com/key4hep/k4MarlinWrapper/pull/183))
  - Reintroduce missing minus sign in test over all events

* 2024-04-24 Leonhard Reichenbach ([PR#182](https://github.com/key4hep/k4MarlinWrapper/pull/182))
  - By default use the `MarlinProcessorWrapper.OutputLevel` to set the verbosity of the wrapped Marlin processor.

* 2024-04-11 Leonhard Reichenbach ([PR#178](https://github.com/key4hep/k4MarlinWrapper/pull/178))
  - Modernize event display example

* 2024-04-09 tmadlener ([PR#176](https://github.com/key4hep/k4MarlinWrapper/pull/176))
  - Switch to the non-deprecated conversion functions that have been introduced in [k4EDM4hep2LcioConv#54](https://github.com/key4hep/k4EDM4hep2LcioConv/pull/54)

* 2024-03-21 Leonhard Reichenbach ([PR#179](https://github.com/key4hep/k4MarlinWrapper/pull/179))
  - Add inputReader for easy reading of edm4hep or lcio files

* 2024-03-14 tmadlener ([PR#171](https://github.com/key4hep/k4MarlinWrapper/pull/171))
  - Cleanup the clic reconstruction from EDM4hep inputs example a bit.
    - Remove commented LCIO input
    - Remove explicit listing of input collections, which is no longer necessary after [k4FWCore#162](https://github.com/key4hep/k4FWCore/pull/162)

* 2024-02-26 tmadlener ([PR#177](https://github.com/key4hep/k4MarlinWrapper/pull/177))
  - Update the test input file for the clic reconstruction starting from EDM4hep inputs.

* 2024-02-22 tmadlener ([PR#168](https://github.com/key4hep/k4MarlinWrapper/pull/168))
  - Switch to use `edm4hep::TrackerHit3D` after it has been renamed upstream (https://github.com/key4hep/EDM4hep/pull/252)

# v00-08

* 2024-02-13 jmcarcell ([PR#167](https://github.com/key4hep/k4marlinwrapper/pull/167))
  - Use ExternalData and remove some scripts for testing to fix running tests in parallel

* 2024-02-08 tmadlener ([PR#173](https://github.com/key4hep/k4marlinwrapper/pull/173))
  - Remove the dedicated `clang-format` workflow since it is part of the `pre-commit` workflow

* 2024-02-08 tmadlener ([PR#172](https://github.com/key4hep/k4marlinwrapper/pull/172))
  - Switch the `doctest` CI workflows to an alma9 based container with CVMFS inside

* 2024-02-08 jmcarcell ([PR#169](https://github.com/key4hep/k4marlinwrapper/pull/169))
  - Delete build workflow since we have another one for key4hep that covers builds for nightlies, releases and all the operating systems we support

* 2024-01-27 jmcarcell ([PR#166](https://github.com/key4hep/k4marlinwrapper/pull/166))
  - Add the edm4hep includes to allow compilation with clang, which currently doesn't compile due to incomplete classes

* 2023-12-05 Leonhard Reichenbach ([PR#162](https://github.com/key4hep/k4marlinwrapper/pull/162))
  - Modified the edm4hep to lcio converter to always convert the`EventHeader` to have event number etc. available in lcio.

* 2023-12-03 Mateusz Jakub Fila ([PR#163](https://github.com/key4hep/k4marlinwrapper/pull/163))
  - fixed typos in documentation

* 2023-11-09 BrieucF ([PR#161](https://github.com/key4hep/k4marlinwrapper/pull/161))
  - Tiny fix to `test/gaudi_opts/fccRec_e4h_input.py`

# v00-07

* 2023-11-03 jmcarcell ([PR#156](https://github.com/key4hep/k4MarlinWrapper/pull/156))
  - Use the new `edm4hep::CellIDEncoding`. Needs https://github.com/key4hep/EDM4hep/pull/234

* 2023-11-02 tmadlener ([PR#157](https://github.com/key4hep/k4MarlinWrapper/pull/157))
  - Make sure to always create a `CaloHitContribution` collection if any `SimCalorimeterHit` is seen in the conversion even if all of them are empty.
  - Use the global object map to resolve `MCParticle` relations for these contributions.

* 2023-11-02 tmadlener ([PR#148](https://github.com/key4hep/k4MarlinWrapper/pull/148))
  - Add a warning message to the steering file converter script to warn users about any `<include ref="..." />` statements in the original Marlin steering file.

* 2023-10-09 jmcarcell ([PR#152](https://github.com/key4hep/k4MarlinWrapper/pull/152))
  - Fix links since edmConverter.md was removed

* 2023-10-09 tmadlener ([PR#149](https://github.com/key4hep/k4MarlinWrapper/pull/149))
  - Create a one page summary, including brief how-tos, for the wrapper and related topics.
    - Compiled from the existing documentation, focusing entirely on things that are part of the wrapper repository, removing things that should be introduced elsewhere (e.g. basics of the Gaudi framework).

* 2023-10-05 tmadlener ([PR#147](https://github.com/key4hep/k4MarlinWrapper/pull/147))
  - Introduce a *global object map* that is shared by all converter instances (in either direction). This is necessary to properly set all relations even if the related objects are not converted in the same converter instance.
    - Fixes [key4hep/k4LCIOReader#31](https://github.com/key4hep/k4LCIOReader/issues/31)
    - Fixes #113 
  - Add a `global_converter_maps` test case (and a description) that checks that this works as expected.

* 2023-10-04 tmadlener ([PR#146](https://github.com/key4hep/k4MarlinWrapper/pull/146))
  - Move the EDM4hep to LCIO (and back) converter tests to k4EDM4hep2LcioConv (https://github.com/key4hep/k4EDM4hep2LcioConv/pull/29) as they effectively only test functionality of the converter.

* 2023-10-03 tmadlener ([PR#144](https://github.com/key4hep/k4MarlinWrapper/pull/144))
  - Move the EDM4hep to LCIO (and back) converter tests to k4EDM4hep2LcioConv (https://github.com/key4hep/k4EDM4hep2LcioConv/pull/29) as they effectively only test functionality of the converter.

* 2023-10-03 tmadlener ([PR#142](https://github.com/key4hep/k4MarlinWrapper/pull/142))
  - Introduce a *global object map* that is shared by all converter instances (in either direction). This is necessary to properly set all relations even if the related objects are not converted in the same converter instance.
    - Fixes [key4hep/k4LCIOReader#31](https://github.com/key4hep/k4LCIOReader/issues/31)
    - Fixes #113 
  - Add a `global_converter_maps` test case (and a description) that checks that this works as expected.

* 2023-09-13 tmadlener ([PR#141](https://github.com/key4hep/k4MarlinWrapper/pull/141))
  - Move includes to where they are really necessary and remove unnecessary includes from other headers.

* 2023-09-13 tmadlener ([PR#140](https://github.com/key4hep/k4MarlinWrapper/pull/140))
  - Remove the workarounds for non-zero exit codes again. They were introduced in #120 to work around key4hep/k4FWCore#125 which has been fixed with key4hep/k4FWCore#132

* 2023-09-13 Andre Sailer ([PR#134](https://github.com/key4hep/k4MarlinWrapper/pull/134))
  - TrackingCellIDEncodingSvc: add service to set the Cell ID encoding string for iLCSoft track reconstruction

* 2023-09-12 tmadlener ([PR#139](https://github.com/key4hep/k4MarlinWrapper/pull/139))
  - Make sure to have an `import os` in all gaudi options files that use `os` in some form. This becomes necessary after https://github.com/key4hep/k4FWCore/pull/134

* 2023-09-12 tmadlener ([PR#138](https://github.com/key4hep/k4MarlinWrapper/pull/138))
  - Use the `EDM4hep2LCIOConv` namespace (introduced in https://github.com/key4hep/k4EDM4hep2LcioConv/pull/27) in the EDM4hep to LCIO conversion tool.

* 2023-09-05 jmcarcell ([PR#137](https://github.com/key4hep/k4MarlinWrapper/pull/137))
  - Docs: Correct the comment referring to the input file that has to be replaced
  - Test: no longer ignore the return value of a test. It used to return 4, but that was fixed in  key4hep/k4fwcore#132
  - Tests: Replace environment variable LCGEO with K4GEO following the usage in other key4hep repositories

* 2023-09-04 jmcarcell ([PR#136](https://github.com/key4hep/k4MarlinWrapper/pull/136))
  - Fix clang warning "inconsistent-missing-override" by adding final to some virtual functions

* 2023-09-04 tmadlener ([PR#128](https://github.com/key4hep/k4MarlinWrapper/pull/128))
  - Install the `clicRec_e4h_input.py` script that is used in the documentation in order to make the paths that are used there less confusing. Fixes [#90](https://github.com/key4hep/k4MarlinWrapper/issues/90)

* 2023-08-30 Andre Sailer ([PR#135](https://github.com/key4hep/k4MarlinWrapper/pull/135))
  - CI: clicrec_edm4hep test: created new edm4hep SIM file compatible with current podio master

* 2023-08-30 tmadlener ([PR#129](https://github.com/key4hep/k4MarlinWrapper/pull/129))
  - Make sure to convert the `CellIDEncoding` for collections when going from LCIO to EDM4hep.

* 2023-07-26 Andre Sailer ([PR#132](https://github.com/key4hep/k4MarlinWrapper/pull/132))
  - CI: add pre-commit

* 2023-07-26 Leonhard Reichenbach ([PR#131](https://github.com/key4hep/k4MarlinWrapper/pull/131))
  - Fixed a typo and creation of trailing white space in convertMarlinSteeringToGaudi.py

* 2023-07-26 Andre Sailer ([PR#130](https://github.com/key4hep/k4MarlinWrapper/pull/130))
  - Add license to all files
  - Add pre-commit
  - Fix some white space

# v00-06

* 2023-03-02 jmcarcell ([PR#108](https://github.com/key4hep/k4MarlinWrapper/pull/108))
  - Rename TPCHit -> RawTimeSeries

* 2023-02-28 Thomas Madlener ([PR#107](https://github.com/key4hep/k4MarlinWrapper/pull/107))
  - Fix link to edm converters documentation

* 2023-01-16 Thomas Madlener ([PR#106](https://github.com/key4hep/k4MarlinWrapper/pull/106))
  - Make it possible to run clic reco tests concurrently, by ensuring that cloning the CLICPerformance repository is not done concurrently by several test processes.
  - Only get the last commit of CLICPerformance to speed up the cloning.

* 2023-01-16 Frank Gaede ([PR#103](https://github.com/key4hep/k4MarlinWrapper/pull/103))
  - LcioEventAlgo: put LCIO input files into global::parameters
     - needed e.g. by PatchCollection processor
     - done in LcioEventAlgo::init()
     - this fix is needed to be able to convert LCIO files to EDM4hep when not all collections are present in all events (default in ILD reconstruction)
  - ensure marlin::Global:parameters is created exactly once

* 2023-01-15 Erica Brondolin ([PR#105](https://github.com/key4hep/k4MarlinWrapper/pull/105))
  - The input file test/gaudi_opts/fccRec_e4h_input.py is now updated with changes introduced with https://github.com/key4hep/k4MarlinWrapper/pull/91

# v00-05

* 2022-12-19 Thomas Madlener ([PR#101](https://github.com/key4hep/k4MarlinWrapper/pull/101))
  - Add `diff` based tests that compare the output files of the clic reconstruction test, with expected outputs via `anajob` and `podio-dump`. This should catch very obvious regressions like #98, already in CI.
  - Remove the restriction of running the clic reconstruction test only in nightly stacks.

* 2022-12-19 Thomas Madlener ([PR#100](https://github.com/key4hep/k4MarlinWrapper/pull/100))
  - Examples and other shared resources should go to `<prefix>/share/k4MarlinWrapper` in order for `key4hep-stack` environment variables to be properly populated. This fixes a problem where `$K4MARLINWRAPPER` points to an empty directory.
  - Make sure to run the `doctest` workflows using the newly built version of `k4MarlinWrapper`
  - Add `CEDViaWrapper` to the things that are run in the `doctest` workflow.
  - Update github actions versions in CI.

* 2022-12-19 Thomas Madlener ([PR#92](https://github.com/key4hep/k4MarlinWrapper/pull/92))
  - Fix an issue where `isFirstEvent` always returns `false` in wrapped processors by calling `setFirstEvent` after processing the first event.

* 2022-11-30 Andre Sailer ([PR#99](https://github.com/key4hep/k4MarlinWrapper/pull/99))
  - EDM4hep2LCIO: Fix so that FillMissingCollections is always called after other collections were converted. Needed also to correctly assign SimCalorimeterHitContributions. Depends on key4hep/k4EDM4hep2LcioConv#7, fixes #98

* 2022-11-24 Erica Brondolin ([PR#97](https://github.com/key4hep/k4MarlinWrapper/pull/97))
  - Just a small fix to the `event_display.py` file

* 2022-11-24 Thomas Madlener ([PR#95](https://github.com/key4hep/k4MarlinWrapper/pull/95))
  - Fix memory leak in `LcioEvent`, where the event read from file was leaked every event.

* 2022-11-15 Thomas Madlener ([PR#91](https://github.com/key4hep/k4MarlinWrapper/pull/91))
  -  EDM4hep2LcioTool, Lcio2EDM4hepTool: Breaking Change! Replaced configuration via a list of string (pairs) with a `convertAll` flag and a `collNameMapping` (map of string to string). This makes it possible to more easily convert all collections, but still override the naming of the target collection without having to type out all possible collections.

* 2022-07-08 Violetavican ([PR#89](https://github.com/key4hep/k4MarlinWrapper/pull/89))
  - Reordered information just to clarify the steps that must be followed for LCIO input reconstruction

* 2022-06-30 Thomas Madlener ([PR#87](https://github.com/key4hep/k4MarlinWrapper/pull/87))
  - Add documentation and a bit of explanation of how to run a wrapped CEDViewer for an event display using EDM4hep inputs.

* 2022-06-30 Thomas Madlener ([PR#86](https://github.com/key4hep/k4MarlinWrapper/pull/86))
  - Install the `examples/` directory into `<install-prefix>/share/` to make them more easily available after installation. In Key4hep releases they will be accessible via `$K4MARLINWRAPPER/examples/`.

# v00-04-02

* 2022-06-27 Thomas Madlener ([PR#85](https://github.com/key4hep/k4MarlinWrapper/pull/85))
  - Add a gaudi options file that can be used to run the event display using the CLIC detector via the Marlin wrapper.

* 2022-06-22 Placido Fernandez Declara ([PR#84](https://github.com/key4hep/k4MarlinWrapper/pull/84))
  - Move doc md files to starterkit

* 2022-06-21 Valentin Volkl ([PR#81](https://github.com/key4hep/k4MarlinWrapper/pull/81))
  - Update covMatrix with changes from edm4hep v0.5

* 2022-06-15 Placido Fernandez Declara ([PR#77](https://github.com/key4hep/k4MarlinWrapper/pull/77))
  - Use k4EDM4hep2LcioConv as separate lib
  - Remove conversion from MarlinWrapper

* 2022-05-30 Valentin Volkl ([PR#79](https://github.com/key4hep/k4MarlinWrapper/pull/79))
  * examples: use geometry file based on LCGEO from the current stack

* 2022-05-20 Placido Fernandez Declara ([PR#75](https://github.com/key4hep/k4MarlinWrapper/pull/75))
  - Use unique_ptr instead of new for converter
  - Improve comments and error messages
  - Handle errors coming from k4LCIOReader conversion

* 2022-05-17 Placido Fernandez Declara ([PR#73](https://github.com/key4hep/k4MarlinWrapper/pull/73))
  - Fix include dirs dependencies in cmake

* 2022-04-05 Thomas Madlener ([PR#52](https://github.com/key4hep/k4MarlinWrapper/pull/52))
  - Make it possible to set the random seed that is used for the `ProcessorEventSeeder` of the Marlin processors from the options file by retrieving it from Gaudis Random Service. Fixes #38 
  - Use `argparse` for handling arguments in converter script
  - Cleanup testing of `--num-events` handling and EOF.

* 2022-03-14 Placido Fernandez Declara ([PR#71](https://github.com/key4hep/k4MarlinWrapper/pull/71))
  - Add support for multi-threading with LCIO input only
  - Add example Gaudi steering file on how to run in parallel
  - Add documentation for multi-threading

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
