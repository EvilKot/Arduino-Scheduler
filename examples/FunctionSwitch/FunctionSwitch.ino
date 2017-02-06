#include <Task.h>

Task<char>* listenerTask;

void setup()
{
	listenerTask = new Task<char>(listen);
}

void loop()
{
	listenerTask->Start();
	char result = await(listenerTask);
	
	Serial.print("I recieved: ");
	Serial.println(result);
}

char listen()
{
	waitfor(Serial.available() > 0);
	return Serial.read();
}