<h1 align="center">
  <br>
  morgoth
</h1>

<h4 align="center">A source dedicated server manager</h4>

<div align="center">
  <a href="https://travis-ci.org/melkortf/morgoth">
    <img src="https://travis-ci.org/melkortf/morgoth.svg?branch=master" alt="build status">
  </a>
  <img src="https://img.shields.io/badge/status-experimental-yellow.svg" alt="status: experimental">
  <a href="https://github.com/melkortf/morgoth/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/license-GPL-blue.svg">
  </a>
</div>


## Features

* Start, stop and show status of any game server with one command
* Extend functionality by writing plug-ins
* Access your servers via [anna](https://github.com/melkortf/anna) API and [ando](https://github.com/melkortf/ando) front-end

## Dependencies

* Qt (>= 5.10)
* CMake (>= 3.10)
* systemd
* DBus

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

## Configuration

The configuration file is installed in `/etc/morgoth.conf`. Available options are:
* `dbus` specifies which bus should morgoth bind to. Leave it set to `system` if you wish to use morgoth as a daemon;
* `database` defines the location of SQlite database file;
* `user` specifies the user that will own all the running game server instances. It is adviced against running source game as a root.

You also need to create the user for the last configuration option:
```bash
$ useradd -r -U morgoth
```

## Usage

```bash
# Start morgoth daemon
$ systemctl start morgoth

# Add your server installation
$ morgothctl add my-fancy-server /home/tf2/servers/fancy

# Make sure your server is added
$ morgothctl list

# Adjust your server launch arguments
$ morgothctl config my-fancy-server

# Start the server
$ morgothctl start my-fancy-server

# Verify your server started successfully
$ morgothctl status my-fancy-server
```

## TODO

There are many TODO as of now, as the project is in an early development phase.
Apart from lots of integration tests, there are:
* add `org.morgoth.Server.displayName` config option for ando
* plugins: add plugin dependency system
* plugins: implement `server-updater` plugin
* plugins: implement `iptables` plugin
* morgothctl: add `restart` command
* (POC needed) handle morgoth <-> server communication via srcds plugin
