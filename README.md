# Hexiwear hand wash

Repository for the Hexiwear hand wash project.

## Get Started

This project is based on [mbed OS 6](https://os.mbed.com/docs/mbed-os/v6.15/introduction/index.html), to use it you need the `mbed cli 2` compiler toolchain. Follow [this instructions](https://os.mbed.com/docs/mbed-os/v6.15/build-tools/mbed-cli-2.html) to install it.

Mbed CLI needs [Git](https://git-scm.com/) to download the dependencies, so you’ll also need to install it. This project uses by default the `GCC_ARM` compiler, so you need to install it too.

To compile and flash the program run this command

```console
$ mbed-tools compile -t GCC_ARM -m HEXIWEAR -f
```

### Program the KW40Z

Tho have access to the Bluetooth Low Energy and the buttons of the Hexiwear you need to flash the KW40Z binary file downloadable [here](https://github.com/MikroElektronika/HEXIWEAR/blob/master/SW/KW40%20IAR/HEXIWEAR_bluetooth/binary/HEXIWEAR_KW40_v100.bin).
After you have downloaded the `.bin` file set the switch in the docking station to `00111111` and copy the file to the mounted device to flash it.

## Read Hexiwear output

The program can print some logs to his standard output, to read those you need to establish a serial connection to the device.

The default method is using the mbed cli 2 by appending `-s/--stream` to the end of the `mbed-tools compile` command.

Another method is using an external program, the suggested one is [picocom](https://linux.die.net/man/8/picocom)

```console
$ picocom --imap lfcrlf /dev/serial/device
```