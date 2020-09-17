# GMP Wrapper

Welcome to Gaudi-Marlin-Processor Wrapper

## Configuring, compiling and installing

Add the iLCSoft LCG 97 view to your system, then indicate the paths to cmake for:
- Gaudi: `DGaudi_DIR` and `DGaudiProject_DIR`

```bash
git clone https://github.com/andresailer/GMP.git
cd GMP

source /cvmfs/clicdp.cern.ch/iLCSoft/lcg/97/nightly/x86_64-centos7-gcc8-opt/init_ilcsoft.sh

mkdir build; cd build

cmake -DGaudi_DIR="path/to/GaudiInstallation" -DGaudiProject_DIR="path/to/GaudiInstallation" ..

mkdir ../../gmp_install
cmake -DCMAKE_INSTALL_PREFIX=../../gmp_install ..
make -j 4
make install
```

## Running

To be able to run GMP Wrapper:
- Copy the `muon.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to `test/inputFiles/` directory in GMPWrapper

Copy file and run:

```bash
mkdir ../test/inputFiles
cp /path/to/muon.slcio ../test/inputFiles/my.slcio
./run gaudirun.py ../runit/runit.py
```

## Testing

```bash
# Check that input file exist
mkdir ../test/inputFiles
cp /path/to/muon.slcio ../test/inputFiles/my.slcio
# Display available tests
ctest -N
# Run tests
ctest
```
