#!/usr/bin/env bash

# Default values

# Run makefile
make no-py2powerdevs

# create folder to save the model.h files
mkdir -p regTests/

################################ CONTINUOUS TIME MODELS ######################################
# EXAMPLE 1: Lotka-Volterra Model example
#
echo
echo "************************** Lotka-Volterra Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/lotka_volterra/lotka_volterra.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/lotka_volterra/lotka_volterra/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/lotka_volterra/lotka_volterra/

# EXAMPLE 2: Build Van Der Pol Oscillator Model example
echo
echo "************************** Van Der Pol Oscillator Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/misc/vanderpol_osc.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/misc/vanderpol_osc/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/misc/vanderpol_osc/

# EXAMPLE 3: Non-Linear Stiff Model example
echo
echo "************************** Non-Linear Stiff Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/stiff/nl_stiff.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/stiff/nl_stiff/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/stiff/nl_stiff/

# EXAMPLE 4: Transmission Line Model example
echo
echo "************************** Transmission Line Model **************************"
echo
# Build the model
bin/pdppt -m examples/continuous/transmission_line/lcline.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/transmission_line/lcline/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/continuous/transmission_line/lcline/

################################ DELAYED DIFFERENTIAL EQUATION MODELS ######################################

# EXAMPLE 5: Hairer et al. model example
echo
echo "************************** Hairer et al. Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/hairer_et_al.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/dde/hairer_et_al/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/dde/hairer_et_al/

# EXAMPLE 6: Oberle and Pesch model example
echo
echo "************************** Oberle and Pesch Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/oberle_and_pesch.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/dde/oberle_and_pesch/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/dde/oberle_and_pesch/

# EXAMPLE 7: Cellular Network Spikes Model example
echo
echo "************************** Cellular Network Spikes Model **************************"
echo
# Build the model
bin/pdppt -m examples/dde/cellular_network_spikes.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/dde/cellular_network_spikes/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/dde/cellular_network_spikes/

################################ DISCRETE TIME MODELS ######################################

# EXAMPLE 8: Nicholson-Bailey Model example
echo
echo "************************** Nicholson-Bailey Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/nicholson_bailey.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/nicholson_bailey/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/nicholson_bailey/

# EXAMPLE 9: Q-Operator Model example
echo
echo "************************** Q-Operator Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/qoperator.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/qoperator/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/qoperator/

# EXAMPLE 10: Discrete-Time State-Space Model example
echo
echo "************************** Discrete-Time State-Space Model **************************"
echo
# Build the model
bin/pdppt -m examples/discrete/statespace.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/statespace/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/discrete/statespace/

################################ HYBRID MODELS ######################################

# EXAMPLE 11: Bouncing Ball Model example
echo
echo "************************** Bouncing Ball Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/bouncing_ball/bball_downstairs.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/bouncing_ball/bball_downstairs/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/bouncing_ball/bball_downstairs/

# EXAMPLE 12: Discrete Buck Converter Model example
echo
echo "************************** Discrete Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_disc.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_disc/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_disc/

# EXAMPLE 13: Controlled Buck Converter Model example
echo
echo "************************** Controlled Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_controlled.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_controlled/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_controlled/

# EXAMPLE 14: Coupled Controlled Buck Converter Model example
echo
echo "************************** Coupled Controlled Buck Converter Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/buck/buck_controlled_coupled.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_controlled_coupled/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/buck/buck_controlled_coupled/

# EXAMPLE 15: DC Drive Model example
echo
echo "************************** DC Drive Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/dc_drive/dc_drive.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/dc_drive/dc_drive/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/dc_drive/dc_drive/

# EXAMPLE 16: DC Drive Buck Model example
echo
echo "************************** DC Drive Buck Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/dc_drive/dc_drive_buck.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/dc_drive/dc_drive_buck/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/dc_drive/dc_drive_buck/

# EXAMPLE 17: Spiking Neuron Model example
echo
echo "************************** Spiking Neuron Model **************************"
echo
# Build the model
bin/pdppt -m examples/hybrid/spiking_neuron/spikes.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/spiking_neuron/spikes/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/hybrid/spiking_neuron/spikes/

################################ QSC CONTROLLED MODELS ######################################

# EXAMPLE 18: QSC Control for an Inverted Pendulum Model example
echo
echo "************************** QSC Inverted Pendulum **************************"
echo
# Build the model
bin/pdppt -m examples/qsc/inverted_pendulum.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/qsc/inverted_pendulum/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/qsc/inverted_pendulum/

################################ NETWORK MODELS ######################################

# EXAMPLE 19: Network Basic Model example
echo
echo "************************** Network Basic Model (GUI) **************************"
echo
# Build the model
bin/pdppt -m examples/network/basic/network_basic.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/network/network_basic/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/network/network_basic/

################################ QUEUEING MODEL ######################################

# EXAMPLE 20: Queueing Model example
echo
echo "************************** Queueing Model (GUI) **************************"
echo
# Build the model
bin/pdppt -m examples/queueing/basic.pdm
# Copy the file model.h
mkdir -p regTests/$(git rev-parse --abbrev-ref HEAD)/queueing/basic/
mv build/model.h regTests/$(git rev-parse --abbrev-ref HEAD)/queueing/basic/

