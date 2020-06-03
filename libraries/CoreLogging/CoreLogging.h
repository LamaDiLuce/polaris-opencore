#include "Arduino.h"
#include "CoreCommon.h"

#ifndef Logging_h
#define Logging_h

class CoreLogging
{
	public:

		//Constructor
		CoreLogging();
		//Init
		void init(bool debug);
		//Write message without return
		void write(String message);
		//Write message with return
		void writeLine(String message, String prefix = "");
		//Write label + value (String) with return
		void writeParamString(String param, String value, String prefix = "");
		//Write label + value (int) with return
		void writeParamInt(String param, int value, String prefix = "");
		//Write label + value (float) with return
		void writeParamFloat(String param, float value, String prefix = "");
		//Write status decoded with return
		void writeParamStatus(Status status, String prefix = "");
		//Write label + value (long) with return
		void writeParamLong(String param, long value, String prefix = "");

	private:

		bool debug_mode = false;

};
#endif
