#!/bin/bash

IMAGE_NAME=powerdevs_devel_eclipse_img_$USER


docker build -t $IMAGE_NAME -f Dockerfile.eclipse .
