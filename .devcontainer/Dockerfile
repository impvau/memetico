
FROM nvidia/cuda:12.0.1-devel-ubuntu22.04

ARG ENV

RUN apt-get update

# Compilation & Debugging
RUN apt-get -y install g++
RUN apt-get -y install gdb

# Program Libraries
RUN apt-get -y install libeigen3-dev
RUN apt-get -y install nlohmann-json3-dev

# Documentation Tools
RUN apt-get -y install doxygen
RUN apt-get -y install graphviz
RUN apt-get install -y nfs-common
RUN apt-get install -y git
RUN apt-get install -y vim

RUN apt-get install -y mpich

WORKDIR /workspaces/memetico

#COPY run.sh /workspaces/memetico/run.sh
