<h1 align="center">
  <br>
  morgoth
</h1>

<h4 align="center">A source dedicated server manager</h4>

<div align="center">
  <img src="https://img.shields.io/badge/stability-experimental-orange.svg?style=flat-square" alt="stability: experimental">
</div>


## Features

* Start, stop and show status of any game server with one command
* Extend functionality by writing plug-ins
* Access your servers via [anne](https://github.com/melkortf/anne) API and [ando](https://github.com/melkortf/ando) front-end

## Dependencies

* Qt (>= 5.8)
* CMake (>= 3.7)

## Installation

```
# Clone this repository
$ git clone https://github.com/melkortf/morgoth

# Create build directory
$ mkdir morgoth/build && cd morgoth/build

# Configure project
$ cmake ..

# Build project
$ make && make test

# Install
$ make install
```
