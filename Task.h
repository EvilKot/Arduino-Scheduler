/**
* @file Task.h
* @version 0.6
*
* @section License
* RCore-Task is based heavily on Arduino-Scheduler 1.2 by Mikael Patel, Copyright (C) 2015-2017 (https://github.com/mikaelpatel/Arduino-Scheduler).
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

#ifndef _CORETASK_h
#define _CORETASK_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>

//typedef void(*Action)();

class  CoreTask
{
private:
	
#if defined(TEENSYDUINO) && (defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__))
	/** Default stack size and stack max. */
	static const size_t DEFAULT_STACK_SIZE = 512;
	static const size_t STACK_MAX = 16384;

#elif defined(ARDUINO_ARCH_AVR)
	/** Default stack size. */
	static const size_t DEFAULT_STACK_SIZE = 128;

#elif defined(ARDUINO_ARCH_SAM)
	/** Default stack size and stack max. */
	static const size_t DEFAULT_STACK_SIZE = 512;
	static const size_t STACK_MAX = 32768;

#elif defined(ARDUINO_ARCH_SAMD)
	/** Default stack size and stack max. */
	static const size_t DEFAULT_STACK_SIZE = 512;
	static const size_t STACK_MAX = 16384;

#elif defined(ARDUINO_ARCH_ESP8266)
	/** Default stack size and stack max. */
	static const size_t DEFAULT_STACK_SIZE = 512;
	static const size_t STACK_MAX = 32768;

#else
#error "Scheduler.h: board not supported"
#endif


	static CoreTask main;
	static CoreTask* running;
	static size_t stackTop;

	CoreTask* next;
	CoreTask* prev;
	jmp_buf context;
	const uint8_t* stack;

	void Setup();
	virtual void Inject();

protected:
	typedef void(*Action)();
	Action action;

	bool isCompleted;
	virtual void CoreTask::Execute();

public:

	CoreTask(Action action);

	virtual void Start();
	void Start(Action action);
	bool IsCompleted();
	static void Yield();
};

template <typename TResult = void>
class Task : public CoreTask //, CoreTaskUtils<TResult> ---> template<typename T> class CoreTaskUtils : public CoreTask { public: T Await() { while(!isCompleted) yeild(); return T; } };
{
protected:
	typedef TResult(*Func)();
	TResult result;

	virtual void Execute() override
	{
		this->result = reinterpret_cast<typename Task<TResult>::Func>(this->action)();
	}

public:

	Task(Func func) : CoreTask(func) {
		this->result = TResult();
	}

	virtual void Start() {
		this->result = TResult();
		CoreTask::Start();
	}

	void Start(Func func) {
		CoreTask::Start(func);
	}

	TResult Result() {
		return this->result;
	}

	TResult Await() {
		while (!this->isCompleted)
			CoreTask::Yield();
		return this->result;
	}

};

template<>
class Task<void> : public CoreTask
{
public:
	Task(Action action) : CoreTask(action) { }

	void Await() {
		while (!this->isCompleted)
			CoreTask::Yield();
	}
};

#define await(task) task->Await();
#define waitfor(condition) while (!(condition)) yield()

#endif