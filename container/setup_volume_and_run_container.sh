#!/usr/bin/env bash

# Example of usage 1:
# ./setup_volume_and_run_container.sh --use-display --user-name $USER --image-name powerdevs-cc7:stable
# Example of usage 2 specifying an additional folder with a PD example:
# ./setup_volume_and_run_container.sh --use-display --user-name $USER --image-name powerdevs-cc7:stable --pdevs-example-dir <absolute-path-to-powerdevs-example-folder>

# Default values
CONTAINER_NAME="pd-container"
VOLUME_NAME="powerdevs-volume"
IMAGE_NAME="powerdevs-cc7:slim"
PDEVS_ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
USERNAME="default"

USE_DISPLAY=false

USE_VOLUME=false

USERNAME=$(whoami)
OS=$(uname)

# Parse input arguments (may change default values)
while [[ $# -gt 0 ]]; do
    case $1 in
        --use-display) # OPTIONAL: enables the use of the host system's display
        USE_DISPLAY=true
        shift # Remove current argument from processing
        ;;
        --image-name)
        IMAGE_NAME="$2" # OPTIONAL: chooses specific docker image
        shift # Remove current argument
        shift # Remove current value
        ;;
        --container-name)
        CONTAINER_NAME="$2" # OPTIONAL: customizes the name of the docker container
        shift # Remove current argument
        shift # Remove current value
        ;;
        --use-volume)
        USE_VOLUME=true # OPTIONAL: create volume instead of bind
        shift # Remove current argument
        shift # Remove current value
        ;;
        --volume-name)
        VOLUME_NAME="$2" # OPTIONAL: customizes the name of the docker volume
        shift # Remove current argument
        shift # Remove current value
        ;;
        --user-name)
        USERNAME="$2" # OPTIONAL: customizes the name of the user inside the docker container
        shift # Remove current argument
        shift # Remove current value
        ;;
	--pdevs-example-dir)
        PDEVS_EXAMPLE_DIR="$2" # OPTIONAL: specifies an external folder with a PowerDEVS model
        shift # Remove current argument
        shift # Remove current value
        ;;
        *)
        # If the argument is unknown, shift and continue
        shift
        ;;
    esac
done

if [ "$OS" == "Linux" ]; then
    echo "Running on Linux."
    if $USE_DISPLAY; then
        # DISPLAY=":0" 
        # The command bellow allows the Docker container to communicate with the X server (Xorg) on your Linux system
        # This is done by adding the current user to the X server's allow list.
        xhost local:$(whoami)
        echo "--use-display is set. Using display $DISPLAY"
    fi
elif [ "$OS" == "Darwin" ]; then
    echo "Running on macOS."
    if $USE_DISPLAY; then
        DISPLAY="$(ipconfig getifaddr en0):0" 
        # The command bellow allows the Docker container to communicate with the X server (XQuartz) on your macOS
        # This is done by adding the IP address of your macOS system (often via `en0` for Wi-Fi) to the X server's allow list. 
        # If you're using ethernet, the interface might be `en1` or similar.
        xhost + $(ipconfig getifaddr en0)
        echo "--use-display is set. Using display $DISPLAY"
    fi
else
    echo "Running on an unknown system."
fi

VOLUME_LOOKUP=$(docker volume ls --format "{{.Name}}" | awk -v vol="$VOLUME_NAME" '$1 == vol')

if $USE_VOLUME; then
	if [ ! -z "$VOLUME_LOOKUP" ]; then
	    echo "Volume exists. Reusing it."
	else
	    echo "Volume does not exist. Creating Docker volume named $VOLUME_NAME..."
	    docker volume create $VOLUME_NAME
	    if [ $? -ne 0 ]; then
		echo "Error creating docker volume."
		exit 1
	    fi
	fi
fi

# Determine docker command based on USE_DISPLAY and USE_VOLUME
if $USE_VOLUME; then
	DOCKER_CMD="docker run -d -h $(hostname) -v $VOLUME_NAME:/powerdevs --ipc=host --net=host -it "
else
	if [ -n "$PDEVS_EXAMPLE_DIR" ]; then
		DOCKER_CMD="docker run -d -h $(hostname) -v $PDEVS_ROOT_DIR:/powerdevs -v $PDEVS_EXAMPLE_DIR:/pd-example-model --ipc=host --net=host -it "
		if [ $? -ne 0 ]; then
			echo "Error binding model folder to container."
			exit 1
		fi
	else
		DOCKER_CMD="docker run -d -h $(hostname) -v $PDEVS_ROOT_DIR:/powerdevs --ipc=host --net=host -it "
	fi
fi

if $USE_DISPLAY; then
    CONTAINER_NAME="$CONTAINER_NAME-display" 
    # the options below bind the container's display to the host system's display
    DOCKER_CMD="$DOCKER_CMD -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v $HOME/.Xauthority"
else
    CONTAINER_NAME="$CONTAINER_NAME-headless"
fi
DOCKER_CMD="$DOCKER_CMD --name $CONTAINER_NAME $IMAGE_NAME" 

echo "Running Docker container named $CONTAINER_NAME..."
echo $DOCKER_CMD
eval $DOCKER_CMD
if [ $? -ne 0 ]; then
    echo "Error running docker container."
    exit 1
fi

# move the powerdevs directory to the container
if $USE_VOLUME; then
	if [ -z "$VOLUME_LOOKUP" ]; then
		echo "Copying files from $PDEVS_ROOT_DIR to /powerdevs inside the Docker container..."
        docker cp $PDEVS_ROOT_DIR/. $CONTAINER_NAME:/powerdevs
		if [ $? -ne 0 ]; then
		    echo "Error copying powerdevs to container."
		    exit 1
		fi
		if [ -n "$PDEVS_EXAMPLE_DIR" ]; then
		    docker cp $PDEVS_EXAMPLE_DIR $CONTAINER_NAME:/
		    if [ $? -ne 0 ]; then
		        echo "Error copying model to container."
		        exit 1
		    fi
		fi
	fi
fi

# setup ssh keys
docker cp ~/.ssh $CONTAINER_NAME:/home/$USERNAME/.ssh # copy the ssh keys to the container
docker exec -u root $CONTAINER_NAME rm /home/$USERNAME/.ssh/config # host ssh configs may not be compatible with the container1
if [ $? -ne 0 ]; then
    echo "Error copying ssh dir to container."
fi

echo "Changing ownership of files inside the Docker container for CentOS default user access..."
docker exec -u root $CONTAINER_NAME chown -R 1000:1000 /powerdevs /home/$USERNAME
if [ $? -ne 0 ]; then
    echo "Error changing ownership of files inside container to 1000:1000."
    exit 1
fi

echo "Attaching to the running Docker container named $CONTAINER_NAME..."
docker attach $CONTAINER_NAME

# Capture the exit status
EXIT_STATUS=$?
echo "Exited Docker container named $CONTAINER_NAME with exit status $EXIT_STATUS."

# Check for common exit statuses you expect and want to handle quietly
if [ $EXIT_STATUS -eq 0 ]; then
    echo "Detached from Docker container named $CONTAINER_NAME."
elif [ $EXIT_STATUS -eq 127 ]; then
    echo "Detached from Docker container named $CONTAINER_NAME with CTRL+D."
else
    echo "Error attaching to docker container."
fi


echo "Attempting to stop Docker container..."
docker stop $CONTAINER_NAME
if [ $? -ne 0 ]; then
    echo "Warning: Error stopping the docker container. Continuing..."
else
    echo "Stopped Docker container."
fi

echo "Attempting to remove Docker container..."
docker rm $CONTAINER_NAME
if [ $? -ne 0 ]; then
    echo "Warning: Error removing the docker container. Continuing..."
else
    echo "Removed Docker container."
fi
