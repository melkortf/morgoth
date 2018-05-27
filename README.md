<h1 align="center">
  <br>
  morgoth
</h1>

<h4 align="center">A source dedicated server manager</h4>

<div align="center">
  <a href="https://travis-ci.org/melkortf/morgoth">
    <img src="https://img.shields.io/travis/melkortf/morgoth.svg?style=flat-square" alt="build status">
  </a>
  <img src="https://img.shields.io/badge/status-experimental-orange.svg?style=flat-square" alt="status: experimental">
  <a href="https://github.com/melkortf/morgoth/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/license-GPL-blue.svg?style=flat-square">
  </a>
</div>


## Features

* Start, stop and show status of any game server with one command
* Extend functionality by writing plug-ins
* Access your servers via [anne](https://github.com/melkortf/anne) API and [ando](https://github.com/melkortf/ando) front-end

## Dependencies

* Qt (>= 5.10)
* CMake (>= 3.10)

## Installation

```bash
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
