# PowerDEVS Container

The **stable dockerfile** provides everything you need, along with some additional utilities that might be useful. This image has a larger footprint but takes a zero-risk approach to dependencies.

The **slim dockerfile** provides just the essentials needed to run the Powerdevs GUI and a simulation. This image is roughly half the size, builds faster but doesn't include the extra features.

> **Tip**: Start with the slim version to save time. If you encounter missing dependencies, switch to the stable version.


## Docker Instructions

There are two options for using Docker:
1. Download a pre-built Docker image with everything ready to use - go to step **Download docker image**.
2. Build the Docker image from a Dockerfile - go to step **Get the repo**.

### Download docker image

Download a Docker image with CentOS and PowerDEVS from this [link](https://cernbox.cern.ch/s/pnTFwT06ftJcUHQ). This will download a file named `vscode_powerdevs_scilab552_image.tar`.

The name of the image is: `vsc-powerdevs-fix` (Make sure the image file is located in the Linux directory where you're currently positioned in the terminal).

To load the image into Docker:
```bash
docker load < /path/to/image.tar
```

To run a Docker container and then open a Bash terminal inside it, use the script `setup_volume_and_run_container.sh`:

```bash
cd powerdevs/container
./setup_volume_and_run_container.sh --use-display --user-name $USER --image-name vsc-powerdevs-fix
```
Then, go to step **Build PowerDEVS**.

### Get the repo
Clone the repo:
```bash
git clone https://git-modsimu.exp.dc.uba.ar/matiasb/powerdevs-CERN.git powerdevs
```

In case the following error comes up: 
> fatal: unable to access 'https://git-modsimu.exp.dc.uba.ar/matiasb/powerdevs-CERN.git/': 
> server certificate verification failed. CAfile: /etc/ssl/certs/ca-certificates.crt CRLfile: none

run: 
```bash
export GIT_SSL_NO_VERIFY=1
```
and try cloning again.

Then move inside the repository:
```bash
cd powerdevs
```

### Build image
Build the docker image:
```bash
docker build -t powerdevs-cc7:slim --build-arg USERNAME=$(whoami) -f container/Dockerfile-cc7-slim.txt .
```

### Run container
Run the docker container:
```bash
container/setup_volume_and_run_container.sh --use-volume
```
After running this, you should be attached to a fully functional container where you can run PowerDEVS, modify the source code and even git push your changes.
- **Security**: the script will copy your ssh keys to the container. If running this on a server you share with other people, be mindfull about who has access to your docker volume and container. To increase safety, it is advised to use `podman` insted of docker. The interface is the same and, unlike docker, it will only grant access to the creator by default.
- **Display GUI**: If you want to use powerdevs GUI, you need to add the `--use-display` flag to the command. Otherwise, the script will run headless by default. Doing this will link the host system's `X11` to the containers. 
- If you are a `macOS` user, you will need to follow a few extra steps to install an X server in your system. This will be explained in detail below.
- After running the script with the `--use-display` flag, test X11 works by running the `xeyes` command. A pair of eyes that follow your mouse should open. Use CTRL C to kill. 
- You can customize container, volume, image and user names to be used by passing alternative names as arguments to the script.

#### Particular cases
To run the container binding the PowerDEVS folder from the host use the following (without `--use-volume`):
```bash
./setup_volume_and_run_container.sh --use-display --user-name $USER --image-name powerdevs-cc7:stable
```
To run the container binding the PowerDEVS folder from the host and an additional folder with a separated example:
```bash
./setup_volume_and_run_container.sh --use-display --user-name $USER --image-name powerdevs-cc7:stable --pdevs-example-dir <absolute-path-to-example-folder>
```

### Build PowerDEVS
Build PowerDEVS inside the container:
```bash
make clean && make -j X
```
- **Note**: Replace X with as many physical cores your build system has.
  - If using a modern CPU, only count the P cores

### Test PowerDEVS
Test GUI
`./bin/pdme` or `./runPD`

Test simulation of a model built with the GUI (pdm file)
```bash
bin/pdppt -m examples/network/basic/network_basic.pdm
cd build
make
cd ../output/
./model -tf 30 -c ../examples/network/basic/network_basic.params -finalization_script ../examples/network/basic/finalization.py
```

Test simulation of a model built with the Python binding Py2PowerDEVS:
```bash
cd examples/network/basic/py2pdevs/
python network_basic.py -tf 30
```
Options:
  - `-tf`: specifies the simulation's final time.
  - `--c`: specifies a file containing the model parameters.
  - `-variable_logging_backend`: specifies the logging backend. Options: Scilab, hdf5.

## Connect VSCode to your Docker Container (Optional)

This is useful for development purposes. You can have a VSCode window where you can edit files, open the terminal, install dependencies, interact with git, and benefit from all the functionalities that an IDE provides.

**Prerequisites:**

- **VSCode:** Ensure you have [Visual Studio Code](https://code.visualstudio.com/) installed.
- **Remote - Containers Extension:** Install the [Remote - Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension from the VSCode marketplace.


## Running GUI on containers for macOS users

macOS doesn't natively use the X Window System but relies on its proprietary windowing system called Quartz. However, with XQuartz, macOS users can run X11 applications. This section guides macOS users on running GUI applications from Docker containers and displaying them on the macOS desktop using XQuartz.

**Prerequisites:**

- [XQuartz](https://www.xquartz.org/) - An X server for macOS.

##### Allow Connections From Network Clients

1. Open **XQuartz**.
2. Navigate to **Preferences**.
3. In the **Security** tab, enable **Allow connections from network clients**.
4. Restart XQuartz if necessary.
