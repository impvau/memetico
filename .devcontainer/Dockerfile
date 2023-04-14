
FROM nvidia/cuda:12.0.1-devel-ubuntu22.04

ARG ENV

RUN apt-get update

# Compilation & Debugging
#RUN if ["$ENV" = "dev"] ; then apt-get -y install g++ ; fi
#RUN if ["$ENV" = "dev"] ; then apt-get -y install gdb ; fi
RUN apt-get -y install g++
RUN apt-get -y install gdb

# Program Libraries
RUN apt-get -y install libeigen3-dev
RUN apt-get -y install nlohmann-json3-dev

# Documentation Tools
#RUN apt-get install doxygen
#RUN apt-get install graphviz

# Revision Control
#RUN apt-get install git

WORKDIR /root
