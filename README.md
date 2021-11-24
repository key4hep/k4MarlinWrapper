# k4MarlinWrapper

Welcome to k4 Marlin Wrapper

## Configuring, compiling and installing

Use the CVMFS HSF key4hep view to compile k4MarlinWrapper.

```bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh
# Alternatively use nightlies
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

To run k4MarlinWrapper, some input is needed:
- As an example, copy the `muons.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to `test/inputFiles/` directory in k4MarlinWrapper

To copy file and run:
```bash
mkdir ../test/inputFiles
cp /path/to/muons.slcio ../test/inputFiles/muons.slcio
k4run ../k4MarlinWrapper/examples/runit.py
```

## Testing

Several tests are provided

```bash
# Display available tests
ctest -N
# Run all tests
ctest
# Run specific test with verbose output
ctest --verbose -R test_clicReconstruction
```

## Using Event Data Model (EDM) converters

Please refer to [this doc](./doc/edmConverters.md)