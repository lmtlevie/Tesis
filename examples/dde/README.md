# Delayed Differential Equation Models

## Hairer et al. Model

### Run the model through the GUI

### Run the model through the command line

1. Go to the root folder and run the preprocessor to generate `model.h`:
```bash
cd powerdevs/
bin/pdppt -m examples/dde/hairer_et_al.pdm
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

3. Finally, run the model specifying the final simulation time (in this case 100s):
```bash
cd ../output/
./model -tf 100
```

## Oberle and Pesch Model

### Run the model through the GUI

### Run the model through the command line

1. Go to the root folder and run the preprocessor to generate `model.h`:
```bash
cd powerdevs/
bin/pdppt -m examples/dde/oberle_and_pesch.pdm
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

3. Finally, run the model specifying the final simulation time (in this case 10s):
```bash
cd ../output/
./model -tf 10
```

## Cellular Network Spikes Model

### Run the model through the GUI

### Run the model through the command line

1. Go to the root folder and run the preprocessor to generate `model.h`:
```bash
cd powerdevs/
bin/pdppt -m examples/dde/cellular_network_spikes.pdm
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

3. Finally, run the model specifying the final simulation time (in this case 50s):
```bash
cd ../output/
./model -tf 50
```
