##C#-like tasks for Arduino.

Based on mikaelpatel's Arduino-Scheduler (https://github.com/mikaelpatel/Arduino-Scheduler).

May be unstable :)

####General aspects:
1. Task is persistent, shouldn't be deleted.
2. There are limits for number of created tasks. See original Arduino-Scheduler for further details.

###Basic usage:
Define a gloabl variable:
````
Task<int>* task;
````
In setup(), create a new task instance and assign the function with the same return type:
````
Task<int>* task = new Task<int>(someFunction);
````
Start/Restart the task:
````
task->Start();
````
Wait for completion and get result:
````
int result = await(task);
````
