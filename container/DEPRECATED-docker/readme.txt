Instructions to develop PowerDEVS using the docker image
---------------------------------------------------------

1) Install docker in your host (see instructions below for each OS)
2) Download image and build: ./makePowedevs.sh ; ./makePy2PDEVS.sh;   # Only execute once the first time. This will download the public image from docker.hubBuilds (takes a few minutes). Then build powerdevs GUI (takes a few minutes)
2) Log into container: ./startPowerdevs.sh    # The first time, this will download the public image from docker.hub (might take a few minutes)
3) Inside the container, execute: ./runPD     # This will open the PowerDEVS and Scilab GUI

##
NOTE (on GUIs): To compile you need to be able to open graphical interfaces (GUIs). This docker should allow you to open the powerdevs, scilab, matplotlib GUIs. 
      But running GUI applications inside a docker container is really a hack and might need differnet twicks in different host environments.
      Starting pointer: http://wiki.ros.org/docker/Tutorials/GUI|
      
      (ssh GUI sharing): docker can not access X11 interfaces when the machine is accesses using `ssh -X`
		This is a typical CERN workflow, and a workaround is described in `docker/CERN_instructions`
##

##
NOTE (on makefile):  there is a bug in docker for CentOS in which running make with an interactive exec (docker exec -it) generates a segmentation fault.
       To build simulatoin source, you can use the ./makeSimu.sh which runs in non-interactive mode.
       This is not needed when running docker from Ubuntu. You can run make within the container in interactive mode or also from outside (with docker run)
##

##
NOTE (to build the image locally):  instead of downloading the image "camisa/powerdevs_devel:latest" in step 2, you can build it locally from the Dockerfile (this will also download stuff and takes longer)
  To do so, you can use ./buildPowerdevsImage.sh. This will create an image with the name "powerdevs_devel_img_<USER>". Remember to tag to update the IMAGE_NAME in the ./startPowerdevs.sh script.
##

##
NOTE (different images):  There are a few flavours of images (ej: with or without eclipse inside the docker). 
  To see available images check: https://hub.docker.com/r/camisa/powerdevs_devel/tags/
  To use a different image (instead of the default one camisa/powerdevs_devel:latest), update the IMAGE_NAME variable in the startContainer.sh script (remember to remove old containers). Ej: IMAGE_NAME=camisa/powerdevs_devel:eclipse
##

Develpment workflow
---------------------

Code: in the host and in docker (the git root directory is mounted in the contaier at /powerdevs. So all files within are shared and in sync)
Compile: in the docker (run "make" inside /powerdevs as usual). See note for CENTOS
Run: in the docker (/powerdevs/output/model -tf .... as usual)
PowerDEVS GUI: in the docker (use /powerdevs/runPD script)
Debug: in the docker with gdb. Or use eclipse inside the container (see camisa/powerdevs_devel:eclipse). Or use gdbserver and debug from host (not so easy to configure cross-tarjet gdb)


Install docker in CENTOS
-------------------------
see: https://www.liquidweb.com/kb/how-to-install-docker-on-centos-6/

1) yum -y install docker-io
2) sudo groupadd docker; sudo usermod -aG docker $USER; newgrp docker;  # this to avoid using sudo in every docker command https://docs.docker.com/install/linux/linux-postinstall/#manage-docker-as-a-non-root-user
3) service docker start
4) (optional, recommended) chkconfig docker on

NOTE: might be needed to disable SELinux to share folders between host and docker: sudo su -c "setenforce 0" 


Install docker in Ubuntu
-------------------------
- follow: https://docs.docker.com/install/linux/docker-ce/ubuntu/
- follow post-install for linux: https://docs.docker.com/install/linux/linux-postinstall/

