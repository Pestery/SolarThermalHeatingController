#ifndef DATE_TIME_H
#define DATE_TIME_H

// The DateTime class holds information about a date and time.
class DateTime {
public:

	// Type used to store Unix time stamp (number of seconds since 1/1/1970 at UTC)
	typedef uint32_t Type;

	// Conversion factors
	static constexpr Type secondsPerMinute() {return 60;}
	static constexpr Type secondsPerHour() {return 3600;}
	static constexpr Type secondsPerDay() {return 86400;}
	static constexpr Type secondsPerWeek() {return 604800;}
	static constexpr Type secondsPerMonth() {return 2629743;}
	static constexpr Type secondsPerYear() {return 31556926;}

	// Reset the time value
	void reset() {m_time = 0;}

	// Get the raw data value
	Type data() const {return m_time;}

	// Generate a string representation of this data within this class
	String toString() const {
		//return String(m_time);
		// TODO: Make this better
		return F("2021-09-25T11:10:45");
	}

	// A constructor which uses a string in the same format as that returned by toString()
	DateTime(const String& rhs) :
		m_time(0) {

		// Iterate over the length
		const int imax = rhs.length();
		bool atStart = true;
		for (int i=0; i<imax; i++) {

			// Get current character
			// Make sure it is within the allow range of characters
			char c = rhs[i];
			if (('0' <= c) && (c <= '9')) {

				// Update the time point by adding this value
				m_time = (m_time * 10) + (c - '0');
				atStart = false;

			} else if (isWhitespace(c)) {

				// Found white-space character
				// Check if still at start of string
				// If not then exit here (found trailing white-spaces instead of leading white-spaces)
				if (!atStart) break;

			} else {

				// Error. Invalid character found
				m_time = 0;
				break;
			}
		}
	}

	// Constructors and assignment operators
	DateTime() {}
	DateTime(Type rhs) :
		m_time(rhs) {
	}
	DateTime(const DateTime& rhs) :
		m_time(rhs.m_time) {
	}
	DateTime& operator = (Type rhs) {
		m_time = rhs;
		return *this;
	}
	DateTime& operator = (const DateTime& rhs) {
		if (this != &rhs) m_time = rhs.m_time;
		return *this;
	}
	DateTime& operator = (const String& rhs) {
		*this = DateTime(rhs);
		return *this;
	}

	// Cast operator
	operator Type() const {return m_time;}

	// Algorithmic operators
	friend DateTime operator + (Type lhs, const DateTime& rhs) {return DateTime(lhs + rhs.m_time);}
	friend DateTime operator - (Type lhs, const DateTime& rhs) {return DateTime(lhs - rhs.m_time);}
	friend DateTime operator + (const DateTime& lhs, Type rhs) {return DateTime(lhs.m_time + rhs);}
	friend DateTime operator - (const DateTime& lhs, Type rhs) {return DateTime(lhs.m_time - rhs);}
	friend DateTime operator + (const DateTime& lhs, const DateTime& rhs) {return DateTime(lhs.m_time + rhs.m_time);}
	friend DateTime operator - (const DateTime& lhs, const DateTime& rhs) {return DateTime(lhs.m_time - rhs.m_time);}
	DateTime& operator += (Type rhs) {m_time += rhs; return *this;}
	DateTime& operator -= (Type rhs) {m_time -= rhs; return *this;}
	DateTime& operator += (const DateTime& rhs) {m_time += rhs.m_time; return *this;}
	DateTime& operator -= (const DateTime& rhs) {m_time -= rhs.m_time; return *this;}

private:
	Type m_time; // Unix time stamp. Number of seconds since 1/1/1970 at UTC
};

#endif
