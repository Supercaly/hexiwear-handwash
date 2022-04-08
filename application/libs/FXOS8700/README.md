# Hexiwear library for FXOS8700 accelerometer and magnetometer sensor

This library provides full access to the FXOS8700 accelerometer and magnetometer sensor inside the Hexiwear under mbed-os 6.

## Getting started

To use this library with the new mbed-cli 2 build system you need to follow this steps:

1. manually create a `hexiwear_FXOS8700.lib` file containing the url of this git repository
2. run `mbed-tools deploy` to download the required code
3. add the library to your main `CMakeLists.txt` file

```cmake
add_subdirectory(hexiwear_FXOS8700)
 
target_link_libraries(${APP_TARGET} 
    mbed-os 
    hexiwear_FXOS8700
)
```

## Simple Example

```c++
#include "FXOS8700.h"
#include "mbed.h"

FXOS8700 acc_mag(PTC11, PTC10);

int main()
{
    float acc_data[3] = {0};
    float mag_data[3] = {0};
    acc_mag.accel_config();
    acc_mag.mag_config();
    
    while (true)
    {
        acc_mag.acquire_accel_data_g(&acc_data);
        acc_mag.acquire_mag_data_g(&mag_data);
        printf("acc x: %f, y: %f, z: %f\n", acc_data[0], acc_data[1], acc_data[2]);
        printf("mag x: %f, y: %f, z: %f\n", mag_data[0], mag_data[1], mag_data[2]);
        ThisThread::sleep_for(1s);
    }
    return 0;
}
```