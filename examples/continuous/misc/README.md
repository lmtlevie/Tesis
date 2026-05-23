# Van Der Pol Oscillator Model


## Run the model through the GUI

1. Open the model. Go to File -> Open, the choose folder `continuous/` and the file `vanderpol_osc.pdm`.
2. Press the play button or the F5 key.
3. A window pops up, then press "Run Simulation".

## Run the model through the command line

1. Go to the root folder and run the preprocessor to generate `model.h`:
```bash
cd powerdevs/
bin/pdppt -m examples/continuous/misc/vanderpol_osc.pdm
```

2. Then compile the model (make sure that there is no `model` executable in the `output` folder:
```bash
cd build
rm ../output/model
make
```
the folloging line will be run:
```bash
g++ -Wall  -DO_BINARY=0  -std=c++11 -fPIC  -I../src -I../src/engine/common -I../atomics -I../src/utils -Iinclude -I. -I/usr/include/python2.7/  ../src/model/model.cpp -Llib -lengine -lmodels -lutils -lhdf5 -lboost_program_options -lboost_system -lboost_python -lpython2.7 -lgsl -lgslcblas -Wl,-rpath=/powerdevs/build/lib  -o ../output/model
```

3. Finally, run the model specifying the final simulation time (in this case 300s):
```bash
cd ../output/
./model -tf 50
```
