# Lesson 1: basics examples and features

The following examples will show how to start working with Contiki and Zolertia nodes.

The examples are written to be followed in order, so be sure to start from the very first and make your way to the last in an ordered fashion.

## Lesson objectives

The objective of the lessons are:

* Show you how a Contiki's applications are written and implemented
* How to use LEDs (light emitting diodes) and the user button
* Show how to use the different timers in Contiki
* Learn how Contiki processes are created and communicate amongst each other
* Learn about the on-board sensors in the Zolertia nodes
* Show how to use the analogue to digital converter, and implement your own analogue sensors
* Control the General Purpose Input/Output (GPIO) pins

We assume you have a working Contiki development environment, if you need help configuring the toolchain and others, please visit this page:

https://github.com/Zolertia/Resources/wiki

## Some handy commands

The following commands will make your life easier when working with Contiki and the Zolertia nodes:

### Print a list of connected devices:

To get a list of the devices currently connected over USB, and be able to address individually upon flashing or open a terminal for debugging, use the command below:

````
make motelist
using saved target 'z1'
../../../../tools/zolertia/motelist-zolertia
-------------- ---------------- ---------------------------------------------
Reference      Device           Description
-------------- ---------------- ---------------------------------------------
Z1RC5058       /dev/ttyUSB0     Silicon Labs Zolertia Z1

````

### Open a serial connection for debugging

As you will learn in the first lesson `01-hello-world`, you can print debug or general information to the screen using the serial-to-USB connection.  To open a connection you need to know the USB port to which the device is connected (use the command above).

````
make login MOTES=/dev/ttyUSB0
````

If you don't include the port, as default it will try to use the first port it founds, or the `/dev/ttyUSB0`.  If you have an USB connection already established, opening a second one will scramble both!

### Restart a Zolertia Z1 device without pressing the reset button

From the command line just type:

````
make z1-reset MOTES=/dev/ttyUSB0
````

If you don't specify an USB port, it will reset ALL devices connected!

## Compiling and flashing applications

Let's start with the first example `01-hello-world`.  To compile and flash the application to a Zolertia Z1 node type the following:

````
make 01-hello-world.upload
````

If you execute as above it will use the default compilation target and first-found USB port.  You will notice the examples have a `Makefile.target`, enabling as default the `z1` as our compilation target.  To explicitely define a target and USB port at compilation time use:

````
make TARGET=z1 01-hello-world.upload MOTES=/dev/ttyUSB0
````

You can also combine commands and in a single instruction compile, flash and debug with:

````
make TARGET=z1 01-hello-world.upload MOTES=/dev/ttyUSB0 && make login MOTES=/dev/ttyUSB0
````

If you have more than one Z1 connected to the USB ports at the same time, and do not specify a port when flashing, it will programm ALL devices connected! This is useful when you need to program several devices with the same code, but cumbersome if you forgot you are running an application on another device, so take note!

## Working without writting a line of code: using pre-compiled images and list connected devices

Notice there is a directory named "Binaries", in here you will find already compiled images/binaries for you to flash directly to the Zolertia Z1 nodes.  These binaries are useful in case you need to test with a proven working binary, or avoid compiling and start working right away.

These following tools will come handy.

### List connected devices

Use directly the same script the `make login` command uses:

````
./motelist
````

### Program a binary to the Zolertia Z1 nodes

````
./flash-z1 --z1 -c /dev/ttyUSB0 -r -e -I -p 01-basics/Binaries/01-hello-world.ihex
````

The `flash-z1` script takes as arguments the USB port and the binary to flash.

## Changing the Zolertia Z1 Node and MAC addresses

As default the Zolertia Z1 nodes uses the reference ID number (see the above `motelist` command result) to generate both its Node and MAC addresses.  To change to a different one use:

````
make clean && make burn-nodeid.upload nodeid=5072 nodemac=5072 MOTES=/dev/ttyUSB0 && make z1-reset MOTES=/dev/ttyUSB0 && make login MOTES=/dev/ttyUSB0
````

Where `5072` is just a made up number.

Note: DO NOT use 0 at the beginning of the address, as this is actually a compiler flag will give you an error, for example instead of `0158` just type `158`.

You will see an output like this:

````
Starting 'Burn node id'
Burning node id 158
Restored node id 158
````
