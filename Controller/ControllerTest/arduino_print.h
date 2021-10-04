#ifndef ARDUINO_PRINT_H
#define ARDUINO_PRINT_H

///////////////////////////////////////////////////////////////////
// This file is intended only to be a rough replacement for      //
// functions within the Arduino library, solely used for testing //
///////////////////////////////////////////////////////////////////

// Include headers
#include <string>
#include "arduino_string.h"

// A minimal replacement for the String class within the Arduino SDK
class Print {
public:

	virtual size_t write(uint8_t) = 0;
	virtual size_t write(const uint8_t *buffer, size_t size) = 0;

	size_t write(const char *str) {
		if (str == nullptr) return 0;
		return write((const uint8_t *)str, strlen(str));
	}
	size_t write(const char *buffer, size_t size) {
		return write((const uint8_t *)buffer, size);
	}

	virtual int availableForWrite() { return 0; }

    size_t print(const std::string& rhs) {return write(rhs.c_str(), rhs.size());}
    size_t print(const String& rhs)      {return write(rhs.c_str(), rhs.length());}
    size_t print(const char* rhs)        {return write(rhs, strlen(rhs));}
    size_t print(char rhs)               {return write(rhs);}
    size_t print(unsigned char rhs)      {return write(rhs);}
    size_t print(int rhs)                {return print(std::to_string(rhs));}
    size_t print(unsigned int rhs)       {return print(std::to_string(rhs));}
    size_t print(long rhs)               {return print(std::to_string(rhs));}
    size_t print(unsigned long rhs)      {return print(std::to_string(rhs));}
    size_t print(long long rhs)          {return print(std::to_string(rhs));}
    size_t print(unsigned long long rhs) {return print(std::to_string(rhs));}
    size_t print(float rhs)              {return print(std::to_string(rhs));}
    size_t print(double rhs)             {return print(std::to_string(rhs));}

    size_t println(const std::string& rhs) {return print(rhs) + write('\n');}
    size_t println(const String& rhs)      {return print(rhs) + write('\n');}
    size_t println(const char* rhs)        {return print(rhs) + write('\n');}
    size_t println(char rhs)               {return print(rhs) + write('\n');}
    size_t println(unsigned char rhs)      {return print(rhs) + write('\n');}
    size_t println(int rhs)                {return print(rhs) + write('\n');}
    size_t println(unsigned int rhs)       {return print(rhs) + write('\n');}
    size_t println(long rhs)               {return print(rhs) + write('\n');}
    size_t println(unsigned long rhs)      {return print(rhs) + write('\n');}
    size_t println(long long rhs)          {return print(rhs) + write('\n');}
    size_t println(unsigned long long rhs) {return print(rhs) + write('\n');}
    size_t println(float rhs)              {return print(rhs) + write('\n');}
    size_t println(double rhs)             {return print(rhs) + write('\n');}
    size_t println()                       {return write('\n');}

};

#endif
