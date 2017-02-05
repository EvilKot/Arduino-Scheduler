/**
* @file RCore.Task.cpp
* @version 0.6
*
* @section License
* RCore.Task is based heavily on Arduino-Scheduler 1.2 by Mikael Patel, Copyright (C) 2015-2017 (https://github.com/mikaelpatel/Arduino-Scheduler).
* Copyright (C) 2017, EvilKot
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*/

#include "Task.h"
#include <Arduino.h>

// Configuration: SRAM and heap handling
#if defined(TEENSYDUINO) && defined(__MK20DX256__)
#undef ARDUINO_ARCH_AVR
#define TEENSY_ARCH_ARM
#define RAMEND 0x20008000

#elif defined(TEENSYDUINO) && defined(__MK64FX512__)
#undef ARDUINO_ARCH_AVR
#define TEENSY_ARCH_ARM
#define RAMEND 0x20020000

#elif defined(TEENSYDUINO) && defined(__MK66FX1M0__)
#undef ARDUINO_ARCH_AVR
#define TEENSY_ARCH_ARM
#define RAMEND 0x20030000

#elif defined(ARDUINO_ARCH_AVR)
extern int __heap_start, *__brkval;
extern char* __malloc_heap_end;
extern size_t __malloc_margin;

#elif defined(ARDUINO_ARCH_SAM) && !defined(RAMEND)
#define RAMEND 0x20088000

#elif defined(ARDUINO_ARCH_SAMD) && !defined(RAMEND)
#define RAMEND 0x20008000
#endif



CoreTask::CoreTask(Action action) {
	this->next = this->prev = this;
	this->action = action;
	this->Setup();
}

CoreTask CoreTask::main = CoreTask(NULL);
CoreTask* CoreTask::running = &CoreTask::main;

size_t CoreTask::stackTop = CoreTask::DEFAULT_STACK_SIZE;

void CoreTask::Setup()
{
	if (running != &main) return;

	size_t stackSize = DEFAULT_STACK_SIZE + sizeof(this);

	size_t frame = RAMEND - (size_t)&frame;
	uint8_t curStack[stackTop - frame];
	if (main.stack == NULL) main.stack = curStack;

#if defined(ARDUINO_ARCH_AVR)
	// Check that the task can be allocated
	int HEAPEND = (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
	int STACKSTART = ((int)curStack) - stackSize;
	HEAPEND += __malloc_margin;
	if (STACKSTART < HEAPEND) return;

	// Adjust heap limit
	__malloc_heap_end = (char*)STACKSTART;
#else
	// Check that the task can be allocated
	if (s_top + stackSize > STACK_MAX) return;
#endif

	// Adjust stack top for next task allocation
	stackTop += stackSize;

	// Initiate task with given functions and stack top
	this->stack = (curStack - stackSize);
	this->Inject();
}

void CoreTask::Inject()
{
	if (setjmp(this->context)) {
		while (true) {
			this->Execute();

			this->isCompleted = true;
			this->prev->next = this->next;
			this->next->prev = this->prev;
			yield();
		}
	}
}

void CoreTask::Execute()
{
	this->action();
}

void CoreTask::Start()
{
	this->isCompleted = false;

	this->next = &CoreTask::main;
	this->prev = CoreTask::main.prev;
	this->prev->next = this;
	this->next->prev = this;

	yield();
}

void CoreTask::Start(Action action) {
	this->action = action;
	this->Start();
}

bool CoreTask::IsCompleted() {
	return this->isCompleted;
}

void CoreTask::Yield()
{
	if (setjmp(running->context)) return;

	running = running->next;
	longjmp(running->context, true);
}

extern "C"
void yield(void)
{
	CoreTask::Yield();
}
