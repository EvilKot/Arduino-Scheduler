#include <Task.h>

int led = 13;
int interval = 500;

Task<>* blinkTask;

void setup()
{
	pinMode(led, OUTPUT);
	// Init blink task.
	blinkTask = new Task<>(blink);
	
}

void loop()
{
	// Start (Restart) the blink task, wait for completion.
	blinkTask->Start();
	await(blinkTask);

	Serial.println("Blinked!");
}

void blink()
{
	digitalWrite(led, HIGH);
	delay(interval);
	digitalWrite(led, LOW);
	delay(interval);
}