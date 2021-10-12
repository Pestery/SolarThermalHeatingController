#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H

// Include headers
#include "date_time.h"

// The TimeKeeper class holds information about the current Epoch time.
class TimeKeeper {
public:

	// Type used to store Unix time stamp (number of seconds since 1/1/1970 at UTC)
	typedef DateTime::Type Type;

	// Get a DateTime value of the current Epoch time
	DateTime current() {
		return DateTime(m_referenceTime + (Type)((millis() - m_referenceMillis) / 1000));
	}

	// Update the record of the current Epoch time
	void current(const DateTime& t) {
		m_referenceMillis = millis();
		m_referenceTime = t;
	}

	// Update the record of the current Epoch time
	void current(const String& t) {
		m_referenceMillis = millis();
		m_referenceTime = DateTime(t);
	}

	// Returns true if the time has been set
	bool isValid() const {
		return m_referenceTime != 0;
	}

	// Default constructor
	TimeKeeper() :
		m_referenceTime(0),
		m_referenceMillis(0) {
	}

private:
	Type          m_referenceTime;
	unsigned long m_referenceMillis;
};

#endif
