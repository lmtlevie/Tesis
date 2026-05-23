#!/usr/bin/env bash

# Default values

# Run makefile
make

# Open Scilab session
cd output/
scilab -f ../bin/loadBackDoor.sce &
cd ../

################################ CONTINUOUS TIME MODELS ######################################
# EXAMPLE 1: Lotka-Volterra Model example
#
echo
echo "************************** Lotka-Volterra Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/lotka_volterra/lotka_volterra.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 300
cd ../

# EXAMPLE 2: Build Van Der Pol Oscillator Model example
echo
echo "************************** Van Der Pol Oscillator Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/misc/vanderpol_osc.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 50
cd ../

# EXAMPLE 3: Non-Linear Stiff Model example
echo
echo "************************** Non-Linear Stiff Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/stiff/nl_stiff.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 500
cd ../

# EXAMPLE 4: Transmission Line Model example
echo
echo "************************** Transmission Line Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/transmission_line/lcline.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 100 --parameter_reading_backend Scilab
cd ../

################################ DELAYED DIFFERENTIAL EQUATION MODELS ######################################

# EXAMPLE 5: Hairer et al. model example
echo
echo "************************** Hairer et al. Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/hairer_et_al.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 100
cd ../

# EXAMPLE 6: Oberle and Pesch model example
echo
echo "************************** Oberle and Pesch Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/oberle_and_pesch.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 10
cd ../

# EXAMPLE 7: Cellular Network Spikes Model example
echo
echo "************************** Cellular Network Spikes Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/cellular_network_spikes.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 50
cd ../

################################ DISCRETE TIME MODELS ######################################

# EXAMPLE 8: Nicholson-Bailey Model example
echo
echo "************************** Nicholson-Bailey Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/nicholson_bailey.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 50
cd ../

# EXAMPLE 9: Q-Operator Model example
echo
echo "************************** Q-Operator Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/qoperator.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 10
cd ../

# EXAMPLE 10: Discrete-Time State-Space Model example
echo
echo "************************** Discrete-Time State-Space Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/statespace.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 100 --parameter_reading_backend Scilab
cd ../

################################ HYBRID MODELS ######################################

# EXAMPLE 11: Bouncing Ball Model example
echo
echo "************************** Bouncing Ball Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/bouncing_ball/bball_downstairs.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 30
cd ../

# EXAMPLE 12: Discrete Buck Converter Model example
echo
echo "************************** Discrete Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_disc.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 0.01 --parameter_reading_backend Scilab
cd ../

# EXAMPLE 13: Controlled Buck Converter Model example
echo
echo "************************** Controlled Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_controlled.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 0.2 --parameter_reading_backend Scilab
cd ../

# EXAMPLE 14: Coupled Controlled Buck Converter Model example
echo
echo "************************** Coupled Controlled Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_controlled_coupled.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 0.2 --parameter_reading_backend Scilab
cd ../

# EXAMPLE 15: DC Drive Model example
echo
echo "************************** DC Drive Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/dc_drive/dc_drive.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 0.2 --parameter_reading_backend Scilab
cd ../

# EXAMPLE 16: DC Drive Buck Model example
echo
echo "************************** DC Drive Buck Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/dc_drive/dc_drive_buck.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 0.2 --parameter_reading_backend Scilab
cd ../

# EXAMPLE 17: Spiking Neuron Model example
echo
echo "************************** Spiking Neuron Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/spiking_neuron/spikes.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 300
cd ../

################################ QSC CONTROLLED MODELS ######################################

# EXAMPLE 18: QSC Control for an Inverted Pendulum Model example
echo
echo "************************** QSC Inverted Pendulum **************************"
echo
# Build the model
bin/pdppt -m examples/qsc/inverted_pendulum.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 10
cd ../

################################ NETWORK MODELS ######################################

# EXAMPLE 19: Network Basic Model example
echo
echo "************************** Network Basic Model (GUI) **************************"
echo
# Build the model
bin/pdppt -m examples/network/basic/network_basic.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 30 -c ../examples/network/basic/network_basic.params -finalization_script ../examples/network/basic/finalization.py

echo
echo "************************** Network Basic Model (Py2PowerDEVS) **************************"
echo
# Run the model with Py2PowerDEVS
cd ../examples/network/basic/py2pdevs/
time python network_basic.py -tf 30
cd ../../../../

################################ QUEUEING MODEL ######################################

# EXAMPLE 20: Queueing Model example
echo
echo "************************** Queueing Model (GUI) **************************"
echo
# Build the model
bin/pdppt -m examples/queueing/basic.pdm
cd build/
make clean && make
# Run the model
cd ../output/
time ./model -tf 10 -c ../examples/queueing/basic.params --parameter_reading_backend CmdLine --variable_logging_backend hdf5
# Plot results
python3 ../examples/queueing/plot.py &

echo
echo "************************** Queueing Model (Py2PowerDEVS) **************************"
echo
# Run the model with Py2PowerDEVS
cd ../examples/queueing/
time python basic.py -tf 10 --variable_logging_backend hdf5
python3 plot.py &
cd ../../


