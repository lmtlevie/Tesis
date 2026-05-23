#!/bin/bash

IMAGE_NAME=powerdevs_devel_img_$USER

docker build --rm -t $IMAGE_NAME .
