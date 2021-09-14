#ifndef TIMER_H
#define TIMER_H

// Implements a basic timer which triggers with a regular interval
// The timer does not keep perfect time and is allowed to creep/drift
class Timer {
	unsigned long m_previousTrigger;
	unsigned long m_interval;
public:

	// Set the timer interval
	// This is the number of milliseconds between triggers
	inline void setInterval(unsigned long milliseconds) __attribute__((always_inline)) {m_interval = milliseconds;}

	// Check if the timer has triggered
	// If so then this will also update the timer in preparation for the next trigger time
	// The parameter currentTime is value returned by millis()
	bool triggered(unsigned long currentTime) {
		if ((currentTime - m_previousTrigger) >= m_interval) {
			m_previousTrigger = currentTime;
			return true;
		} else {
			return false;
		}
	}

	// Reset the timer so that it will trigger
	// This will cause triggered() to return true the next time it is called, regardless of when it was last called
	inline void reset(unsigned long currentTime) __attribute__((always_inline)) {
		m_previousTrigger = currentTime - m_interval;
	}

	// Constructors
	Timer() : Timer(2000) {}
	Timer(unsigned long triggerIntervalInMilliseconds) :
		m_previousTrigger(0),
		m_interval(triggerIntervalInMilliseconds) {
	}
	Timer(unsigned long triggerIntervalInMilliseconds, unsigned long currentTime) :
		m_previousTrigger(currentTime),
		m_interval(triggerIntervalInMilliseconds) {
	}

};

#endif
