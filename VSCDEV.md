
# Introduction

This section defines the working configuration using VS Code Dev Containers

# Environment Setup
- Open folder in VS Code e.g. `code .`
- Open Command Pallet (CP) with Ctrl+Shift+P
- Select 'Rebuild and Reopen Container' within CP

# Compilation/Execution/Debugging
Execution launch, including the debugger and program arguments is defined in [launch.json](.vscode\launch.json) which has references the [task.json](.vscode\tasks.json) that defines the compilation build tasks.
- Ensure the 'Program Lauch' run configuration is selected
- Place debug points as required
- Press F5 to recompile and run/debug 

# Testing
Test code launch is defined in [launch.json](.vscode\launch.json)
- Ensure the 'Program Test' run configuration is selected
- Press F5 to run the testing code

# Documentation
Run the following in the root directory
```
doxygen doxy.config
```
Open the built documentation by opening [index](docs/html/index.html)
