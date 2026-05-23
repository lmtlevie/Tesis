# **Readme for Users**
  This package provides a tool that can tranfer data from an HDF5 file to an InfluxDB instance. This tool was designed and developed to visualize data produced by PowerDEVS simulations in Grafana.

  HDF5 files, produced by PoweDEVS contain time series data, are characterized by a specific hierachical structure and their size depends on the simulation time and the number of parameters logged.
Tools for exporting HDF5 simulation results to influxDB (which can later be accessed by Grafana) can be found in /bin/hdf52influxdb 

  Both Influxdb and Grafana were chosen for their ability to handle time series data. Influxdb and Grafana services can be running locally with docker containers or accessed over the over the network in remote installations.


# **Table of Contents**

- [**Readme for Users**](#readme-for-users)
- [**Table of Contents**](#table-of-contents)
- [**1. Specifications**](#1-specifications)
- [**2. Requirements**](#2-requirements)
    - [**2.1. Remote Instances**](#21-remote-instances)
    - [**2.2. Local Instances**](#22-local-instances)
- [**3. Remote Instances**](#3-remote-instances)
    - [**3.1. Grafana**](#31-grafana)
    - [**3.2. Influxdb**](#32-influxdb)
- [**4. Local Instances (Docker Containers)**](#4-local-instances-docker-containers)
    - [**4.1. How to Start Containers**](#41-how-to-start-containers)
    - [**4.2. Grafana**](#42-grafana)
      - [**4.2.1. How to Save Dashboards**](#421-how-to-save-dashboards)
    - [**4.3. Influxdb**](#43-influxdb)
    - [**4.4. Docker**](#44-docker)
- [**5. How to Use Python Scripts**](#5-how-to-use-python-scripts)
    - [**5.1. config.ini**](#51-configini)
    - [**5.2. hdf52influxdb.py**](#52-hdf52influxdbpy)
      - [**5.2.1. More Time Precision**](#521-more-time-precision)
      - [**5.2.2. Less Time Precision**](#522-less-time-precision)
      - [**5.2.3. Save with Different Name**](#523-save-with-different-name)
      - [**5.2.4. Use Another Config**](#524-use-another-config)
      - [**5.2.5. More Help**](#525-more-help)
    - [**5.3. influxdbRemoveData.py**](#53-influxdbremovedatapy)
      - [**5.3.1. Use Another Config**](#531-use-another-config)
      - [**5.3.2. More Help**](#532-more-help)
- [**6. Change Time Range in Grafana**](#6-change-time-range-in-grafana)
- [**7. Notes for HDF5 File and InfluxDB Structure**](#7-notes-for-hdf5-file-and-influxdb-structure)


# **1. Specifications**

  The supported versions of InfluxDB and Grafana are shown in the table below:

  |                  | InfluxDB | Grafana |
  |------------------|----------|---------|
  | Remote (CERN IT) | 1.8.3    | 8.4.3   |
  | Local  (docker)  | 2.4.0    | 8.4.3   |

  Regarding the remote case, instances' versions are the ones provided by CERN IT and DBOD service. Regarding the local case, only the InfluxDB version is different and is the last one that exists at the moment.

  Some similarities and differences of the two InfluxDB versions are:
  
  - Both 1.8.3 and 2.4.0 provide support for **Flux** language.
  - Version 2.4.0 is better supported from the InfluxDB client libraries.
  - Version 2.4.0 provides an integrated GUI, to explore database scheme and data.
  - Version 1.8.3 can be easily connected to grafana using **InfluxQL** language.


# **2. Requirements**

  To use the tool, **python 3.6** is recommended. To install all the necessary modules, from the current directory use:

  ```
  pip install -r requirements.txt
  ```

### **2.1. Remote Instances**
  
  In order to use the remote instance of InfluxDB, **influx CLI v1.10.8** is required. Find instructions on how to install it for your os in the link below, under *Are you interested in InfluxDB 1.x Open Source?*:
  
  - <https://portal.influxdata.com/downloads/>

  Also, if a database explorer is needed, for influx version 1.x, Infludata provides **Chronograf**. More details and instruction on how to install it can be found in the same link above, under the *Are you interested in InfluxDB 1.x Open Source? / Chronograf time series data visualization*.

### **2.2. Local Instances**
  
  In order to have InfluxDB and Grafana locally, **Docker** with the plugin **Docker Compose** is required. Find instructions on how to install docker engine for your os in the links below:

  - <https://docs.docker.com/engine/install/ubuntu/>
  - <https://docs.docker.com/engine/install/>
  - <https://docs.docker.com/engine/install/linux-postinstall/>
  
  Docker compose plugin, should be installed automatically.


# **3. Remote Instances**

### **3.1. Grafana** 
  
  If you are connected to CERN network, you can access grafana instance using the link below and your CERN credentials:

   - <https://grafana-00.web.cern.ch/?orgId=1>

  Grafana has two InfluxDB datasources, one using Flux quering language and the other using InfluxQL. Using Flux is more flexible as it allows access to different buckets. Using InfluxQL, you have to configure a spesific datasource for every database.

### **3.2. InfluxDB**
  
  You can access InfluxDB instance with influx **CLI v1.10.8**, by using the credentials and the command below:

  - Host: https://dbod-influxdb-00.cern.ch
  - Port: 8092
  - Username: ASK FOR THAT
  - Password: ASK FOR THAT
  
  ```
  influx --username username --password password --ssl -host dbod-influxdb-00.cern.ch -port 8092
  ```

  Documentation for InfluxDB v1.8 can be found in the link below:

  - <https://docs.influxdata.com/influxdb/v1.8/>


# **4. Local Instances (Docker Containers)**

  The appropriate docker compose file is located in the directory:

  > powerdevs/docker/grafana

### **4.1. How to Start Containers**
  
  Regarding the local instances, after installing docker, you can start the containers using the following commands:

  ```
  cd powerdevs/docker/grafana
  docker compose up -d
  ```

### **4.2. Grafana**
  
  Connect using:
  
  - Host: <http://localhost:3000>
  - Username: admin
  - Password: password
  - Org: -
  - Token: -
  
  Grafana has already InfluxDB configured as a datasource, using Flux quering language and also there is a dashboard provided as template.

  #### **4.2.1. How to Save Dashboards**
  
  If Grafana is running within a container, when the container is deleted all changes to the dashboards will be lost. 
  You can save the Dashboard as a `.json` file: https://grafana.com/docs/grafana/v9.0/dashboards/export-import/

  Grafana loads saved dashboards every time the container is initialized from the following directory:
  > docker/grafana/grafana/provisioning/dashboards
  

### **4.3. InfluxDB**
  
  Connect using:
  
  - Host: <http://localhost:8086>
  - Username: admin
  - Password: password
  - Org: cern
  - Token: admintoken

  This local insstance uses InfluxDB version 2.4.0, so it provides a UI to manage and explore the database.

  Documentation for InfluxDB v2.4 can be found in the link below:
  
  - <https://docs.influxdata.com/influxdb/v2.4/>

### **4.4. Docker**

If you want more control over docker, you can find infromation about images, conatainers and volumes in the links below:

- <https://docs.docker.com/engine/reference/commandline/image/>
- <https://docs.docker.com/engine/reference/commandline/container/>
- <https://docs.docker.com/storage/volumes/>


# **5. How to Use Python Scripts**

### **5.1. config.ini**
  
  Use *config.ini* to specify InfluxDB version, host, port and authentication credentials.

  configRemote.ini and config.ini can be used as templates.

### **5.2. Exporting HDF5 results to InfluxDB: hdf52influxdb.py**

  Use **hdf52influxdb.py** to write an HDF5 file to the database. To do so, you have to provide the appropriate hdf5 file. For example:

  ```
  python hdf52influxdb.py --h5_file <hdf5_file>
  ```

  *hdf52influxdb.py* creates the database/bucket with the same name as the hdf5 file. If the database/bucket already exists, you can choose to overwrite it or not. See 'Removing data from influxDB'

  #### **5.2.1. Time Precision**

  The simulation uses C++ double to represent time in seconds, which allows for very high precision. 
  The start of the simulation (time 0.0) is mapped to timestamp `1970-01-01 01:00:00` in InfluxDB.  
  By default timestamps will be saved with nanoseconds precision in InfluxDB.  However, Grafana plots are able to visualize data only with milliseconds precision. Points with nanoseconds precision will be plotted to the closest millisecond. As a result, you can see different points with the same timestamp.
  
  If a higher time precision is required, a workaround is possible by scaling up simulation time but careful interpretation of plots is required.
  To scale up simulation time, use the argument *--time_scalar*. For example:

  ```
  python hdf52influxdb.py --h5_file <hdf5_file> --time_scalar 1000000
  ```

  This command scales up times from the simulation by multiplying by the given factor, effectively increasing the time precision.
  For example, by default 2 events ocurring at t1=1.000000000001 (1 second and 1 picosecond) and t2=1.000000000002 will be recorded with the same timestamp matching the closest nanosecond t3=1.000000001 mapped to `1970-01-01 01:00:01.000000001` in influxDB . Using the command above, simulation times will be scaled up to t'1=1000000.000001 (1 million seconds and 1 microsecond) and t2=1000000.000002 which will be recorded as different timestamps mapped to `1970-01-11 13:46:40.000000001` in influxDB.
  Care must be taken to interpret the plots, as Grafana will show a date time that must be downscaled appropriately.

  #### **5.2.2. Less Time Precision**

  Use the argument *--time_precision*. For example:

  ```
  python hdf52influxdb.py --h5_file <hdf5_file> --time_precision 'ms'
  ```

  In this way, timestamps are rounded to the closest millisecond before saved in InfluxDB. As a result, different points can have the same timestamp. InfluxDB ovewrites points with same timestamp, and so, as a result less points are saved in InfluxDB and in Grafana you will not have plots with point with same timestamp.

  #### **5.2.3. Save with Different Name**

  To save the file in a database/bucket with a different name:

  For more details use:
  ```
  python hdf52influxdb.py --h5_file <hdf5_file> --name <name>
  ```

  #### **5.2.4. Use Another Config**

  To use another configuration file:

  For more details use:
  ```
  python hdf52influxdb.py --h5_file <hdf5_file> --config_file <config_file>
  ```

  #### **5.2.5. More Help**

  For more details use:

  ```
  python hdf52influxdb.py -h
  ```

### **5.3. Removing data from influxDB: influxdbRemoveData.py**

  Use **influxdbRemoveData.py** to delete a database/bucket. To do so, you have to specify the name of the database/bucket. For example:

  ```
  python influxdbRemoveData.py --database_or_bucket_name <database/bucket>
  ```

  #### **5.3.1. Use Another Config**
  To use another configuration file:

  For more details use:
  ```
  python influxdbRemoveData.py --database_or_bucket_name <database/bucket> --config_file <config_file>
  ```
  
  #### **5.3.2. More Help**

  For more details use:

  ```
  python influxdbRemoveData.py -h
  ```

# **6. Change Time Range in Grafana**

  If you want to change the time range in a dashboard, in order to match a simulation, the easier way is to add *&from=0&to=end_time*, where *end_time* is the total time of simulation in milliseconds.

  For example:
  
  ```
  https://grafana-00.web.cern.ch/d/gYJW8ZiVk/tdaq_phase2?orgId=1&from=0&to=10000
  ```

# **7. Notes for HDF5 File and InfluxDB Structure**

  HDF5 format offer two types of objects:

  - datasets, which are multidimentional arrarys containing data of the same type
  - groups, which contain datasets and other groups

For example, exploring an HDF5 file produced by the simulation we can see the following:

```
DH0/TcpSessions/TCP_SND_0/cwnd
```

The HDF5 format it's like a file system format. *cwnd* is the dataset and the rests are groups. In our case, the datasets are 2 dimentional arrays with pairs (points) of value and timestamp.

To sent data to influx every point must be converted in the followinf format:

```
measurment,tag_name=tag_value field_name=field_value timestamp
```

Lets assume that inside the *cwnd* dataset, there is the point (value, timestamp) = (value0, timestamp0). In order to send this to influx we produce the following.

```
cwnd,'level0'='DH0','level1'='TcpSessions','level2'='TCP_SND_0' 'value'=value0 timestamp0
```
