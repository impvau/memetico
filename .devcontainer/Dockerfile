
FROM ubuntu:latest

RUN apt-get update

# Compilation & Debugging
RUN apt-get -y install make
RUN apt-get -y install g++
RUN apt-get -y install gdb

# Program Libraries
RUN apt-get -y install libeigen3-dev
RUN apt-get -y install nlohmann-json3-dev

# Documentation Tools
RUN apt-get install doxygen 
RUN apt-get install graphviz

# Revision Control
RUN apt-get -y install git

WORKDIR /root

# Enter your git details
#RUN git config --global user.name "Your Name"
#RUN git config --global user.email "your.email@address"
