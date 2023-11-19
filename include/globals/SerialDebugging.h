#ifndef Debugging_h
#define Debugging_h

#include <Arduino.h>
#if SHOW_DEBUG or SHOW_INFO or SHOW_ERROR or SHOW_LEAK or SHOW_RAW or SHOW_GOOD
#include <Streaming.h>
#endif

#ifdef SHOW_GOOD
#define GOOD(...) Serial << "[32m[GOOD] " << __VA_ARGS__ << "[0m" << endl
#else
#define GOOD(...)
#endif

#ifdef SHOW_DEBUG
#define DEBUG(...) Serial << "[DEBG] " << __VA_ARGS__ << endl
#else
#define DEBUG(...)
#endif

#ifdef SHOW_INFO
#define INFO(...) Serial << "[INFO] " << __VA_ARGS__ << endl
#else
#define INFO(...)
#endif

#ifdef SHOW_ERROR
#define ERROR(...) Serial << "[31m[EROR] " << __VA_ARGS__ << "[0m" << endl
#else
#define ERROR(...)
#endif

#ifdef SHOW_RAW
#define RAW(...) Serial << __VA_ARGS__
#else
#define RAW(...)
#endif

#ifdef SHOW_LEAK
#define LEAK(...)                                                                                  \
	Serial << "[33m[LEAK] " << __VA_ARGS__ << " w/ " << esp_get_minimum_free_heap_size() << "[0m" \
			 << endl
#else
#define LEAK(...)
#endif

#endif