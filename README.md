# GMP Wrapper

Welcome to Gaudi-Marlin-Processor Wrapper

## Configuring, compiling and installing

Add the LCG 96b view to your system, then indicate the paths to cmake for:
- ILCSoft: `DILCSoft_DIR`
- LCIO: `DLCIO_DIR`
- Gaudi: `DGaudi_DIR` and `DGaudiProject_DIR`

```bash
git clone https://github.com/andresailer/GMP.git
cd GMP

source /cvmfs/sft.cern.ch/lcg/views/LCG_96b/x86_64-centos7-gcc8-opt/setup.sh

mkdir build; cd build

cmake -DILCSoft_DIR="path/to/ILCSoftConfig.cmake" -DLCIO_DIR="path/to/LCIOConfig.cmake" -DGaudi_DIR="path/to/GaudiInstallation" -DGaudiProject_DIR="path/to/GaudiInstallation" ..

mkdir ../../gmp_install
cmake -DCMAKE_INSTALL_PREFIX=../../gmp_install ..
make -j 4
make install
```

## Running

To be able to run GMP Wrapper, please set the following from the Marlin installation:
- `export MARLIN_DLL=/path/to/libMarlinDD4hep.so:/path/to/libMarlinTrkProcessors.so`
- Copy the `muon.slcio` from the [DD4hep](https://github.com/AIDASoft/DD4hep) file located in the `DDTest/inputFiles/` directory to the `runit` directory in GMPWrapper

Copy file and run:

```bash
export MARLIN_DLL=/path/to/libMarlinDD4hep.so:/path/to/libMarlinTrkProcessors.so
cp /path/to/muon.slcio my.slcio
./run ../runit/gmprun ../runit/runit.py
```
