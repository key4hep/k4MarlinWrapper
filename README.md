# k4MarlinWrapper

Welcome to k4 Marlin Wrapper

## Configuring, compiling and installing

Add the HSF key4hep view to your system to compile k4MarlinWrapper.

```bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh

git clone https://github.com/key4hep/k4MarlinWrapper.git
cd k4MarlinWrapper

mkdir build install; cd build

cmake ..

cmake -DCMAKE_INSTALL_PREFIX=../install ..
make -j 4
make install
```

## Running

To be able to run k4MarlinWrapper:
- Copy the `muons.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to `test/inputFiles/` directory in k4MarlinWrapper

Copy file and run:

```bash
mkdir ../test/inputFiles
cp /path/to/muons.slcio ../test/inputFiles/muons.slcio
./run gaudirun.py ../k4MarlinWrapper/examples/runit.py
```

## Testing

```bash
# Check that input file exist
mkdir ../test/inputFiles
cp /path/to/muons.slcio ../test/inputFiles/muons.slcio
# Display available tests
ctest -N
# Run tests
ctest
```
