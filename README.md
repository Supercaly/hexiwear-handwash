# Hexiwear hand wash

***NOTE: This branch is and old version of the project using `mbed os 5` further development can be found in the `master` branch using `mbed os 6`***

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

The source code of the application relys on some generated .c/.h files, to generate them run the script `generate_asset.py` like this
```console
$ python3 generate_asset.py --input path/to/assets --output src/generated --name menu_resources
```

### Program the KW40Z

Tho have access to the Bluetooth Low Energy and the buttons of the Hexiwear you need to flash the KW40Z binary file downloadable [here](https://github.com/MikroElektronika/HEXIWEAR/blob/master/SW/KW40%20IAR/HEXIWEAR_bluetooth/binary/HEXIWEAR_KW40_v100.bin).
After you have downloaded the `.bin` file set the switch in the docking station to `00111111` and copy the file to the mounted device to flash it.

## Read Hexiwear output

The program can print some logs to his standard output, to read those you need to establish a serial connection to the device; this can be done through a variety of programs, the suggested one is [picocom](https://linux.die.net/man/8/picocom)

```console
$ picocom imap lfcrlf /dev/serial/device
```