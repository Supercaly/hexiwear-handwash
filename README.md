# Hexiwear hand wash
Repository for the Hexiwear hand wash project.

## Get Started

This project is based on [mbed OS 5](https://os.mbed.com/docs/mbed-os/v5.15/introduction/index.html), to use it you need the `mbed-cli` at the version `1.5.0`; this is a Python 2.7 script that build and deploy all the code automatically, use pip to install the mbed-cli with

```console
$ pip install mbed-cli==1.5.0
```

Mbed CLI supports both [Git](https://git-scm.com/) and [Mercurial](https://www.mercurial-scm.org/) repositories, so you’ll also need to install Mercurial and Git.

This project uses by default the GCC_ARM compiler, you need to install it before and set his path with

```console
$ mbed config -G GCC_ARM_PATH path/to/gcc
```

Before compiling the code remember to fix mbed-os build system: 

* enable `c++ 11` by editing `mbed-os/tools/toolchains/gcc.py` 
* enable `.cc` compilation editing `mbed-os/tools/toolchains/__init__.py` 

follow [this guide](https://github.com/ARMmbed/mbed-os/pull/7437/files) for more info on this process.

## Read Hexiwear output

The program can print some logs to his standard output, to read those you need to establish a serial connection to the device; this can be done through a variety of programs, the suggested one is [picocom](https://linux.die.net/man/8/picocom)

```console
$ picocom /dev/serial/device
```