FROM centos:7

#COPY .ssh/* /root/.ssh/
#COPY etc /etc
#RUN chown root:root /root/.ssh/* && chmod 600 /root/.ssh/*

RUN yum -y upgrade

# enable repositories
RUN yum install -y centos-release-scl epel-release
RUN yum-config-manager --enable epel

# C++ stuff 
RUN  yum install -y make git devtoolset-6 boost-devel valgrind kcachegrind graphviz

# scilab
RUN yum install -y wget java-openjdk mesa-dri-drivers
RUN cd /opt/rh ; wget http://www.scilab.org/download/5.5.2/scilab-5.5.2.bin.linux-x86_64.tar.gz -nv ; tar xzf scilab-5.5.2.bin.linux-x86_64.tar.gz ; rm -f scilab-5.5.2.bin.linux-x86_64.tar.gz
RUN ln -s /opt/rh/scilab-5.5.2/bin/scilab /usr/bin/scilab

# linux useful tools (killall)
RUN yum install -y psmisc 

# python stuff 
RUN yum install -y python-matplotlib python-devel h5py

# Qt
RUN  yum install -y qt-devel
RUN ln -s /usr/bin/qmake-qt4 /usr/bin/qmake

# Powerdevs required stuff
RUN  yum install -y gsl-devel gnuplot hdf5-devel

## Enable the SCL for all bash scripts. (can run "docker run ... bash -c "<command>" " or "docker run ... scl enable devtoolset-v "<command" ")
ENV BASH_ENV=/opt/rh/devtoolset-6/enable \
    ENV=/opt/rh/devtoolset-6/enable \
    PROMPT_COMMAND=". /opt/rh/devtoolset-6/enable"	

# add py2pdevs to the PythonPath    
ENV PYTHONPATH "${PYTHONPATH}:/powerdevs/build/lib"
ENV LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:/powerdevs/build/lib"
