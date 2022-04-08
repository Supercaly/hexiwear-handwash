# Hexiwear library for FXAS21002 gyroscope sensor

This library provides full access to the FXAS21002 gyroscope sensor inside the Hexiwear under mbed-os 6.

## Getting started

To use this library with the new mbed-cli 2 build system you need to follow this steps:

1. manually create a `hexiwear_FXAS21002.lib` file containing the url of this git repository
2. run `mbed-tools deploy` to download the required code
3. add the library to your main `CMakeLists.txt` file

```cmake
add_subdirectory(hexiwear_FXAS21002)
 
target_link_libraries(${APP_TARGET} 
    mbed-os 
    hexiwear_FXAS21002
)
```

## Simple Example

```c++
#include "FXAS21002.h"
#include "mbed.h"

FXAS21002 gyro(PTC11, PTC10);

int main()
{
    float data[3] = {0};
    gyro.gyro_config();
    
    while (true)
    {
        gyro.acquire_gyro_data_dps(&data);
        printf("gyro x: %f, y: %f, z: %f\n", data[0], data[1], data[2]);
        ThisThread::sleep_for(1s);
    }
    return 0;
}
```