# GMP Wrapper

Welcome to Gaudi-Marlin-Processor Wrapper

## Configuring, compiling and installing

Add the iLCSoft LCG 97 view to your system, then indicate the paths to cmake for:
- Gaudi: `DGaudi_DIR` and `DGaudiProject_DIR`

```bash
git clone https://github.com/andresailer/GMP.git
cd GMP

source /cvmfs/sw.hsf.org/key4hep/setup.sh

mkdir build; cd build

cmake ..

mkdir ../../gmp_install
cmake -DCMAKE_INSTALL_PREFIX=../../gmp_install ..
make -j 4
make install
```

## Running

To be able to run GMP Wrapper:
- Copy the `muons.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to `test/inputFiles/` directory in GMPWrapper

Copy file and run:

```bash
mkdir ../test/inputFiles
cp /path/to/muons.slcio ../test/inputFiles/muons.slcio
./run gaudirun.py ../GMPWrapper/examples/runit.py
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
