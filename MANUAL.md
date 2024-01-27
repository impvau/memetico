
# Environment Setup
View the [Dockerfile](.devcontainer\Dockerfile) and replicate the installation of environment and required packages, currently:
```
sudo apt install g++-10
sudo apt install gdb
sudo apt install libeigen3-dev
sudo apt install nlohmann-json3-dev
```

## NVIDIA compiler (nvcc)

Ensure the environment has the NVIDIA compiler (nvcc) for GPU use. Follow instructions from [Ubuntu Instructions](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html#ubuntu) and [CUDA Downloads](https://developer.nvidia.com/cuda-downloads)

Installing cuda-2.13 in this laptop (deb local installation method):
```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
sudo mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/12.3.0/local_installers/cuda-repo-ubuntu2004-12-3-local_12.3.0-545.23.06-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu2004-12-3-local_12.3.0-545.23.06-1_amd64.deb
sudo cp /var/cuda-repo-ubuntu2004-12-3-local/cuda-351BBB3D-keyring.gpg /usr/share/keyrings/
sudo apt-get update
sudo apt-get -y install cuda-toolkit-12-3
```

## Environment Variables
Set environment variable in .bashrc
```
export PATH=/usr/local/cuda-12.3/bin${PATH:+:${PATH}}
```

# Compilation
Compile with
```
make clean
make
```