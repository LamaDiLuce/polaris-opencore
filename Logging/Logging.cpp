#include "Arduino.h"
#include "Logging.h"

//Constructor
Logging::Logging()
{
}

/*
 * Public methods
 */

//Int
void Logging::init(bool debug)
{
	debug_mode = debug;
	if (debug_mode)
	{
		//Serial.begin(9600);
		while (!Serial)
			;
	}
}

//Write message without return
void Logging::write(String message)
{

	if (debug_mode)
		Serial.print(message);
}

//Write message with return
void Logging::writeLine(String message, String prefix)
{

	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.println(message);
	}
}

//Write label + value (String) with return
void Logging::writeParamString(String param, String value, String prefix)
{

	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.print(param);
		Serial.print(": ");
		Serial.println(value);
	}
}

//Write label + value (int) with return
void Logging::writeParamInt(String param, int value, String prefix)
{

	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.print(param);
		Serial.print(": ");
		Serial.println(value, 0);
	}
}

//Write status decoded with return
void Logging::writeParamStatus(Status status, String prefix)
{
	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.print("Status: ");
		switch (status)
		{
		case Status::armed:
		{
			Serial.println("Armed");
		}
		break;
		case Status::disarmed:
		{
			Serial.println("Disarmed");
		}
		break;
		case Status::waitArm:
		{
			Serial.println("Waiting Arm");
		}
		break;
		case Status::waitDisarm:
		{
			Serial.println("Waiting Disarm");
		}
		break;
		case Status::arming:
		{
			Serial.println("Arming");
		}
		break;
		case Status::disarming:
		{
			Serial.println("Disarming");
		}
		break;
		case Status::armingWithChangeColor:
		{
			Serial.println("Arming With Change Color");
		}
		break;
		case Status::waitArmWithChangeColorNext:
		{
			Serial.println("Wait arm With Change Color Next");
		}
		break;
		case Status::waitArmWithChangeColor:
		{
			Serial.println("Wait arm With Change Color");
		}
		break;
		case Status::disarmedInRecharge:
		{
			Serial.println("In Recharging");
		}
		break;
		}
	}
}

//Write label + value (float) with return
void Logging::writeParamFloat(String param, float value, String prefix)
{

	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.print(param);
		Serial.print(": ");
		Serial.println(value, 4);
	}
}

//Write label + value (long) with return
void Logging::writeParamLong(String param, long value, String prefix)
{

	if (debug_mode)
	{
		if (prefix != "")
		{
			Serial.print(prefix);
		}
		Serial.print(param);
		Serial.print(": ");
		Serial.println(value);
	}
}
