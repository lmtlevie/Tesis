This is a template model to use common infrastructure:
    - Loading of Scilab parameters
    - Swapping of parameters
    - saving the results of simulations
    - debug levels (how much it is written to logs) in metrics.sce
    - logging level and loggers (variables written to scilab)  in metrics.sce


To use this model:
   1. Copy the complete directory 'example/TemplateWithInfrastructure' to where you want your model. For example to 'example/YourName/MyNewModel'
   3. Rename the model file 'templateModel.pdm' to the name of your new model. For example 'myNewModel.pdm"
   2. You need to update the path in the 'model.scilabParams' file. 
                  - Go to 'example/YourName/MyNewModel/Scilab/model.scilabParams' 
                  - change the 'exec('../examples/TemplateWithInfrastructure/metrics.sce', 0);' line for: exec('../example/YourName/MyNewModel/metrics.sce', 0);
   3. You need to change the path in the coupled model
            - open the 'example/YourName/MyNewModel/myNewModel.pdm' in PowerDEVS
            - change the paths for the LoadScilabParams, UpdateScilabParams and FinalizationCommands model to point to the path were you saved your model.
                Make sure to change 'example/TemplateWithInfrastructure' for 'example/YourName/MyNewModel' in all the models
                
   4. You should now be ready to compile (F5 in PowerDEVS). You can also run a simulation to check everything is fine (but it will do nothing as there are no models ;-))
   
   
                