#include <Arduino.h>

#ifdef SERIAL_OUT_SILENT

#define PRINT(s)
#define PRINT_VERBOSE(s)
#define PRINTLN(s)
#define PRINTLN_VERBOSE(s)
#define PRINTF(s, ...)
#define PRINTF_VERBOSE(s, ...)

#else

#define PRINT(s) Serial.print(s)
#define PRINTLN(s) Serial.println(s)
#define PRINTF(s, ...) Serial.printf(s, ##__VA_ARGS__)

#ifdef SERIAL_OUT_VERBOSE
#define PRINT_VERBOSE(s) Serial.print(s)
#define PRINTLN_VERBOSE(s) Serial.println(s)
#define PRINTF_VERBOSE(s, ...) Serial.printf(s, ##__VA_ARGS__)
#else
#define PRINT_VERBOSE(s)
#define PRINTLN_VERBOSE(s)
#define PRINTF_VERBOSE(s, ...)
#endif

#endif
