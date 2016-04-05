#ifndef _TIMER_H_
#define _TIMER_H_

#include "TimeProvider.h"

class Timer : public TimeProvider {
public:
	Timer();
	~Timer();

	void reset();
	double get_time();

private:
	long long get_time_internal();
	bool performance_counter_present;	
	long long start_time;
	double scalar;
};

#endif // _TIMER_H_
