=========================

Tested on epdtdifogkv03.cern.ch

# Project directory location (AFS/EOS not supported)   

docker image can not be used if the project lies in a shared file system (AFS/EOS/CVMFS). Problem: the docker image can not share the directory (and associated permissions)


# Created a directory for the project
```
mkdir simulation 
cd simulation
```

# Git clone the project
```
git clone https://gitlab.cern.ch/tdaq-simulation/powerdevs.git
git checkout tdaq_phase2_alternatives
cd powerdevs
git pull
```


# Delete old compilation 
```
find . -name "*.dep" -type f -delete
rm build/lib/*.so; rm build/lib/py2pdevs/core/py2pdevs_core.so
```


# Hardcoded part: copy the two Makefiles

```
cp Makefile_main /home/aabedabu/simulation/powerdevs
cp Makefile_py2pdevs /home/aabedabu/simulation/powerdevs/src/py2pdevs
```

These modified Makefiles allows to perform separately py2pdevs automatic file generations which requires access to a graphical interface. Problem: when compiling in a machine accessed with `ssh -x` the docker image can not access the X11 graphical interfaces. Solution: compile within the docker image, but generate py2pdevs library files from outside docker (`gen_py_lib` whcih calls `bin/pdppt`)


# Start docker
```
cd docker
docker ps
docker rm -f CONTAINER_ID

./startPowerdevs.sh
exit 
```

# Compile the project

These are the instruction to compile within docker, and separately generate py2pdevs library files from outside docker
  
```
sudo docker exec --user=$USER "PowerDEVS-devel-$USER" scl enable devtoolset-6 "make clean"
sudo docker exec --user=$USER "PowerDEVS-devel-$USER" scl enable devtoolset-6 "make -j8"
cd ../
make gen_py_lib -C src/py2pdevs
sudo docker exec --user=$USER "PowerDEVS-devel-$USER" scl enable devtoolset-6 "make -j8"
```

# Example of how to run a py2pdevs model
These runs the `examples/tdaq_phase2_alternatives/run2/option1_pull.py` model, generating a hdf5 results file

```
sudo docker exec --user=$USER "PowerDEVS-devel-$USER" scl enable devtoolset-6 "cd examples/tdaq_phase2_alternatives/run2; ./option1_pull.py -tf 10 -variable_logging_backend hdf5 -c ../tdaq_phase2.params"
```

# Check status of the run2 simulation
```
tail -f ./examples/tdaq_phase2_alternatives/run2/pdevs_run0.log
```


# Extra: some hdf5 commands

```
h5ls option1_pull_0.h5
h5ls -d option1_pull_0.h5/core_router.egressPort10.queue.elemSize.value
```
