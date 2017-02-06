#include <Task.h>

int LED = 13;
int BLINK_INTERVAL = 500;

Task<>* blinkTask;
Task<>* ledTask;

void setup()
{
	blinkTask = new Task<>(blink);
	ledTask = new Task<>(NULL);

	blinkTask->Start();
}

void loop()
{
	yield();
}

void blink()
{
	pinMode(LED, OUTPUT);
	bool enable = false;

	for (;;)
	{
		enable = !enable;

		enable ? ledTask->Start(ledOn) : ledTask->Start(ledOff);
		await(ledTask);		
	}
}

void ledOn()
{
	Serial.println("Led on.");
	digitalWrite(LED, HIGH);
	delay(BLINK_INTERVAL);
}

void ledOff()
{
	Serial.println("Led off.");
	digitalWrite(LED, LOW);	
	delay(BLINK_INTERVAL);
}