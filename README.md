
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4719244.svg)](https://doi.org/10.5281/zenodo.4719244)

# k4MarlinWrapper

<p align="center">
  <img src="doc/k4marlinwrapper_logo.svg"/>
</p>

k4MarlinWrapper runs [Marlin](https://github.com/iLCSoft/Marlin) procesors as [Gaudi](https://gitlab.cern.ch/gaudi/Gaudi) algorithms.
It provides the necessary converters and interfaces between the file types and formats used between both frameworks:
- Python interface to configure Marlin processors
- In-memory Event Data Model (EDM) converters between [LCIO](https://github.com/iLCSoft/LCIO) and [EDM4hep](https://github.com/key4hep/EDM4hep)
- Steering file input conversion from Marlin (XML) to Gaudi (Python)
- Input and output file support for LCIO and EDM4hep

## Build status


[![linux](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/test.yml/badge.svg)](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/test.yml)

[![doctest-linux](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/doctest.yaml/badge.svg)](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/doctest.yaml)

[![clang-format-check](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/key4hep/k4MarlinWrapper/actions/workflows/clang-format-check.yml)


## Using Event Data Model (EDM) converters

Please refer to the [EDM Converters](./doc/edmConverters.md) documentation


## Configuring, compiling and installing

Use the CVMFS HSF Key4hep view to compile and install k4MarlinWrapper.

```bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh
# Alternatively use the nightly build to get the latest development branches of key4hep packages
# source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh

git clone https://github.com/key4hep/k4MarlinWrapper.git
cd k4MarlinWrapper

mkdir build install; cd build

cmake ..

cmake -DCMAKE_INSTALL_PREFIX=../install ..
make -j 4
make install
```


## Running

k4MarlinWrapper processors run with LCIO and EDM4hep input.
- As an example for LCIO, copy the `muons.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to `test/inputFiles/` directory in k4MarlinWrapper

To copy the file and run:
```bash
mkdir ../test/inputFiles
cp /path/to/muons.slcio ../test/inputFiles/muons.slcio
k4run ../k4MarlinWrapper/examples/runit.py
```


## Format

To keep source code style consistent a `.clang-format` configuration is included.
A CI test will check for compliance with this format on Pull Requests.
To apply Clang format to all source and header files, please use your editor's or IDE's built-in integration, or directly from the command line. For example:

```bash
find . -regex '.*\.\(cpp\|h\)' -exec clang-format -style=file -i {} \;
```


## Testing and examples

Several tests are provided. These serve as examples on how to use different features of k4MarlinWrapper.

Refer to the `test/gaudi_opts` directory to find steering files covering different use cases.

```bash
# Display available tests
ctest -N
# Run all tests
ctest
# Run specific test with verbose output
ctest --verbose -R test_clicReconstruction
```

## Using Event Data Model (EDM) converters

Please refer to [this doc](./doc/edmConverters.md) on how to convert between EDMs in k4MarlinWrapper

## Running in parallel: multi-thread support

Please refer to [this doc](./doc/howtoMultithread.md) on how to run with multi-thread support, and which options are supported.
