# Memetico

An implementation of a C++ memetic algorithm under the direction of its inceptor Prof. Moscato as proposed in his 1988 <a href="https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.27.9474&rep=rep1&type=pdf" target="_blank">technical report</a>. 

Prof. Moscato collaborated with Haoyuan Sun in 2018 to introduce <a href="https://ieeexplore.ieee.org/abstract/document/8789889" target="_blank">a memetico algorithm for symbolic regression that utilised a continued fractions representation </a> known as Continued Fractions Regression (CFR). This was later adapted by Mohammad Haque and later Andrew Ciezak.

This repository is a major re-developement to achieve a generalised memetic framework utilisatble by CFR but can be extended to problem classes outside of regression and any solution representation (e.g. graph, distance matrix, etc.)

# Regression Usage

The Regression template class extends the base Model class for a traditional linear form, taking a numeric type for float or integer models. 

## Continued Fration Representation

The ContinuedFractions class extends the Regression class and facilitates depth functionality.

Papers related to this development include the following

* <a href="https://ieeexplore.ieee.org/abstract/document/8789889/" target="_blank"> CFR-V1 - H. Sun and P. Moscato, "A Memetic Algorithm for Symbolic Regression," 2019 IEEE Congress on Evolutionary Computation (CEC), Wellington, New Zealand, 2019, pp. 2167-2174, DOI: 10.1109/CEC.2019.8789889.</a>
* <a href="https://arxiv.org/abs/2001.00624" target="_blank"> CFR-V2 - Moscato, P., Sun, H. and Haque, M.N., 2019. Analytic Continued Fractions for Regression: A Memetic Algorithm Approach. arXiv preprint arXiv:2001.00624. </a>

# Documentation

Project documentation is viewable at `.../memetico-cpp/docs/index.html` and rebuilt with `doxygen doxy.config` from the base directory

Adherance to the <a href="https://people.canonical.com/~msawicz/guides/c++/cppguide.html" target="_blank"> Canonical C++ Style Guide </a> is observed and <a href="https://www.doxygen.nl/index.html" target="_blank"> Doxygen </a> rebuilds the documentation output to the `docs/` directory with `doxygen doxy.config`. 

# Code

See [main.cpp](./main_8cpp.html) for entry into the program and key global variables [globals.h](./globals_8h.html)

// Define how the model classes are used

// Define how they are extendable

// Define how the main.cpp is prepped up for an experiement

`.tpp` usage: Due to heavy reliance on templates, class definitions are within `.tpp` files referenced at the end of `.h` files so that explicit definition of template classes (e.g. Population\<ContinuedFraction\<int\>\>) are not required for compilation.

## Setup

This project assumes development in <a href="https://code.visualstudio.com/" target="_blank"> Visual Studio Code (VCS) </a> with <a href="https://code.visualstudio.com/docs/remote/containers" target="_blank"> Development Containers </a>

This requires the installation of
* <a href="https://code.visualstudio.com/" target="_blank"> Visual Studio Code </a>
* <a href="https://www.docker.com/" target="_blank"> Docker </a>
* <a href="https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers" target="_blank"> Remote Containers Extenion in VS code </a>
* <a href="https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools" target="_blank"> C++ Tools </a>

After loading the repository in VSC select `Remote-Containers: Open Folder in Container` from the F1 Command Pallet.

The container will be build as per `.devcontainer/Dockerfile` which defines the require repositories for code execution

Pressing F5 will run the `Makefile` and code debugger, stopping at any configured breakpoints.

This process is all that is required to compile and run the code and is agnostic of standard iOS, Windows and Linux operation environments.

