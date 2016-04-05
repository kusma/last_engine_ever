#ifndef TIME_PROVIDER_H
#define TIME_PROVIDER_H

class TimeProvider {
public:
	virtual double get_time() = 0;
};

#endif // TIME_PROVIDER_H	
