#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Timer.h"

Timer::Timer() {
	LARGE_INTEGER freq;
	if (QueryPerformanceFrequency(&freq)) {
		performance_counter_present = true;
		scalar = 1.0 / freq.QuadPart;
	} else {
		performance_counter_present = false;
		scalar = 1.0 / 1000;
	}

	start_time = get_time_internal();
}

Timer::~Timer(){}

void Timer::reset() {
	start_time = get_time_internal();
}

double Timer::get_time() {
	return (double)(get_time_internal() - start_time ) * scalar;
}


long long Timer::get_time_internal() {
	long long time;

	if (performance_counter_present ) {
		LARGE_INTEGER temp;
		QueryPerformanceCounter(&temp);
		time = temp.QuadPart;
	} else {
		time = GetTickCount();
	}
	
	return time;
}
