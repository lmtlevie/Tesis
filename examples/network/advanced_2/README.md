# Description 
This example is a continuation of `examples/network/advanced_1`
In this final example will we see how to create new network models by adding new capabilities to pre-existing ones. In particular, we will define a new type of router featuring RED (random early detection). We will also focus on how to properly fine tune its parameters in order to find suitable parameterizations.


# Model creation in py2pdevs
As the high-level view of the model is essentially the same as before, go back to the previous example and use the source code to create a new file, network_advanced_2.py. We will need to perform the following changes to the script:

- Change the creation of Router1 so as to use a RED router. This in turn will require an extension of the network library provided by py2pdevs.
- Change the parameters swept for those needed in RED.
- Change the plotting script. 


For reference, you can find the complete model script in examples/network/advanced_1/py2pdevs/network_advanced_2.py. 

# First step: extension of py2pdevs

Please follow: https://twiki.cern.ch/twiki/bin/view/Main/PowerDEVSSecondAdvancedNetworkExample

# Executing the model
## Py2pDEVS python 
Now proceed to run the script. Execute the following command in a terminal:

    python network_advanced_2.py -tf 10 -variable_logging_backend hdf5 

You should get three plots, as in the previous network example.

## PowerDEVS IGUI

- Load the `network_advanced_2.pdm` file in the PowerDEVS IGUI
- Compile the model by clicking on the `Simulate` button in the toolbar. Wait for the simulation window to pop up 
- Open a terminal and go to the `output` directory inside the PowerDEVS root directory. 
- Execute the following command:

    `./model -tf 10 -c ../examples/network/advanced_2/network_advanced_2.params -variable_logging_backend hdf5 -finalization_script ../examples/network/advanced_2/plot.py `

After some seconds a plot like the third one obtained through py2pdevs should appear. 

# Plotting 

You can use the reference plotting script in examples/network/advanced_2/py2pdevs/plot.py. Take a look at it and try to understand its new behavior --in particular, identify how the new log variable is used. 