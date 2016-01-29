# Arduino-Scheduler

This library implements an extended sub-set of Arduino Scheduler
class. Multiple loop() functions, tasks, may be started and run in a
collaborative multi-tasking style. The tasks are run until they call
yield() or delay(). The Arduino yield() function is replaced by an
implementation in the library that allows context switching.

Tasks should be viewed as static and continuous. This implementation
does not allocated tasks on the heap. They are allocated on the normal
stack and linked into a cyclic run queue. One-shot tasks are not
implemented. Instead the task start function is extended with a setup
function reference. Tasks are started with

````
Scheduler.start(taskSetup, taskLoop [,taskStackSize]).
````
The tasks will start execution when the main task yields. The
_taskSetup_ is called first and once by the task followed by repeated
calls to _taskLoop_. This works just as the Arduino setup() and loop()
functions. There is also an optional parameter, _taskStackSize_. The
default value depends on the architecture (128 bytes for AVR and 512
bytes for SAM/Teensy 3.1).

The Scheduler is a single-ton. The library creates the single
instance.

![screenshot](https://dl.dropboxusercontent.com/u/993383/Cosa/screenshots/Screenshot%20from%202016-01-29%2015%3A24%3A17.png)

## Install

Download and unzip the Arduino-Scheduler library into your sketchbook
libraries directory. Rename from Arduino-Scheduler-master to Scheduler.

The Scheduler library and examples should be found in the Arduino IDE
File>Sketchbook menu.

## Performance

There are several benchmark sketches in the examples directory. Below
are some of the results.

### Context Switch

Board | us | cycles
------|----|-------
Arduino Uno, Nano, Pro-Mini, etc | 11.00 | 176
Arduino Mega 2560 | 12.64 | 203
Arduino Due | 1.36 | 115
Teensy 3.1 (72 MHz) | 1.10 | 80

### Max Tasks (default stack size)

Board | Tasks | Stack (bytes)
------|-------|--------------
Arduino Uno, Nano, Pro-Mini, etc | 9 | 128
Arduino Mega 2560 | 48 | 128
Arduino Due | 52 | 512
Teensy 3.1 (72 MHz) | 25 | 512

### Memory Footprint

Board | PROGMEM | SRAM (bytes)
------|---------|-------------
Arduino Uno, Nano, Pro-Mini, etc | 546 | 42
Arduino Mega 2560 | 548 | 44
Arduino Due | 224 | NA
Teensy 3.1 (72 MHz) | ??? | ???



