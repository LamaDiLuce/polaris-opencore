#pragma once

#include <type_traits>

#include <Arduino.h>

#include "CoreCommon.h"

class CoreLogging
{
public:
  CoreLogging() = delete;

  // Write a formatted log message, using printf format and arguments.
  // Supports the same argument types as printf, plus String and Status
  // Example:
  // int a = ...; String b = ...;
  // CoreLogging::writeLine("Value a=%d, b=%s\n", a, b);
  // A maximum message length of 64 characters is supported.
  template<typename... Args>
  static void writeLine(const char* message, const Args&... args);

  static const char* statusToString(Status status);

private:
  static void writeToSerial(const char* message);
};

/*
The following templates are used to provide an overloaded convertArg() function,
selected at compile time, that converts the argument to a printf-friendly argument.

To add support for logging additional types, implement the relevant template specializations.
*/

template<typename T>
struct ConvertedArgType
{
  using type = T;
};

template<typename T>
inline typename ConvertedArgType<T>::type convertArg(const T& arg)
{
  return arg;
}

/* Allow logging Arduino strings: */

template<>
struct ConvertedArgType<String>
{
  using type = const char*;
};

template<>
inline const char* convertArg<String>(const String& arg)
{
  return arg.c_str();
}

/* Allow logging Status objects: */

template<>
struct ConvertedArgType<Status>
{
  using type = const char*;
};

template<>
inline const char* convertArg<Status>(const Status& arg)
{
  return CoreLogging::statusToString(arg);
}

// Template specialization for no-arguments
template<>
inline void CoreLogging::writeLine(const char* message)
{
  writeToSerial(message);
}

template<typename... Args>
inline void CoreLogging::writeLine(const char* message, const Args&... args)
{
  constexpr uint BUFFER_SIZE = 64;
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, message, convertArg(args)...);
  writeToSerial(buffer);
}