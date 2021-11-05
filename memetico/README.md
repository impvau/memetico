# Memetico

An implementation of a C++ memetic algorithm under the direction of its innovator Prof. Moscato. The original concept was proposed in his 1988 <a href="https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.27.9474&rep=rep1&type=pdf" target="_blank">technical report</a>

This respository is based on the work originally by Haoyuan Sun, continued by Mohammad Haque & Andrew Ciezak towards a continued fractions representations within a memetic implementation.

Major developement was performed toward a general form of memetic algorithm and a framework that facilitates regression representation as one of many possible representations, for instance a graph-based representation can be implemented and it will seemlessly integrate with the core memetic algorithm.

See [main.cpp](./main_8cpp.html) for entry into the program and key global variables [globals.h](./globals_8h.html)

# Code Design

- As a consequence of the heavy reliance on templates, many class definitions appear in `.tpp` files which are included by the `.h` files. If we do not do so, we must explicitly define the used templates within multiple classes. e.g. `template class Population<int><ContinuedFraction>, Population<double><ContinuedFraction>, Population<int><EngelExpansion>` etc. This would require modification of the code each time a new representation was introduced thus the adoption of `.tpp` 

# Symbolic Regression

Models are represented in the Regression template class that implements a simple linear model with a constant. The template faciliates double and integer only representations and can be extended to other forms such as done in the ContinuedFraction class.

## Continued Fration Representation

Of particular interest is the Continued Fractions represetnation in regression problems. Papers related to this development include the following

* <a href="https://ieeexplore.ieee.org/abstract/document/8789889/" target="_blank"> CFR-V1 - H. Sun and P. Moscato, "A Memetic Algorithm for Symbolic Regression," 2019 IEEE Congress on Evolutionary Computation (CEC), Wellington, New Zealand, 2019, pp. 2167-2174, DOI: 10.1109/CEC.2019.8789889.</a>
* <a href="https://arxiv.org/abs/2001.00624" target="_blank"> CFR-V2 - Moscato, P., Sun, H. and Haque, M.N., 2019. Analytic Continued Fractions for Regression: A Memetic Algorithm Approach. arXiv preprint arXiv:2001.00624. </a>

# Documentation

Project documentation is viewable at `.../memetico-cpp/docs/index.html`

Adherance to the <a href="https://people.canonical.com/~msawicz/guides/c++/cppguide.html" target="_blank"> Canonical C++ Style Guide </a> is observed and <a href="https://www.doxygen.nl/index.html" target="_blank"> Doxygen </a> rebuilds the documentation output to the `docs/` directory with `doxygen doxy.config`. 

# Development

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
