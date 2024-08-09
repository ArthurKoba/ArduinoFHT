**AVR Fast Hartley Transform** for AVR chips.
=============================================

[![arduino-library-badge](https://www.ardu-badge.com/badge/AvrFHT.svg?)](https://www.ardu-badge.com/AvrFHT)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/kobaproduction/library/AvrFHT.svg)](https://registry.platformio.org/libraries/kobaproduction/AvrFHT)
[![Russian Translate](https://img.shields.io/badge/README-RUSSIAN_TRANLATE-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/KobaProduction/AvrFHT?_x_tr_sl=en&_x_tr_tl=ru)


Fast Hartley Transform (FHT) for **AVR** chips.
This package is a modification of the outdated but popular 
**[ArduinoFHT](http://wiki.openmusiclabs.com/wiki/ArduinoFHT)** library,
which has been modified for ease of use and some memory optimizations.

What has been modified:
1. The library architecture has been changed.
Now, if you need a component, you need to import its header. 
Inside it is the declaration of functions, and the allocation of memory for buffers and value maps.
2. Removed the map files of the calculated values and placed directly in the headers.
3. Since the _**fht_mag_lin8**_ function works relatively poorly, a _**fht_mag_lin8_no_asm**_ function was developed 
that is not written in assembly. Its essence is the use of **Fast RSS**, which was borrowed from the **[ApproxFFT library](https://www.instructables.com/ApproxFFT-Fastest-FFT-Function-for-Arduino/)**, 
which also has a refined look and feel and use of **PROGMEM**.
4. Added support for **PlatformIO**, as well as the placement of the library in the Arduino repository.

### Need help!

What you need help with:
1. Testing of all types of transformations based on different input data sizes, as well as varieties of AVR chips.
2. Conversion of assembly codes to C code, for the possibility of porting to other architectures, 
as well as the ability to add 512 sample transformations. In particular, the transformation of a butterfly is interesting.
3. Writing and finalizing documentation.

### Found a bug?

Open an issue on this GitHub page, we will try to solve your problems as soon as possible. 
We kindly ask you to check the source library for a recurrence of a bug before notifying about it. 
If it is present there, it is unlikely that without the help of a person who knows the assembler, 
it will be possible to fix it. 
