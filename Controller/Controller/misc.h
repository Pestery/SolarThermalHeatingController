#ifndef MISC_H
#define MISC_H

// This namespace contains general miscellaneous functions which may not fit elsewhere
namespace Misc {

	// A small function to create strings
	// This exists within the specification, but does not seem to work within the Arduino IDE
	inline String makeString(const char* buffer, unsigned int length) {
		String result;
		result.reserve(length);
		while (length--) result += *(buffer++);
		return result;
	}

	// Get next sub-section within a larger string
	// 'source' is the larger source string which is being searched
	// 'startAt' is the index location within that string at which to start. This value will get incremented and should initially be set to zero.
	// 'separator' is the character which marks the break point
	// Returns the string section beginning at 'startAt', and ending just before the next 'separator' or the end of the string, whichever is found first
	String getNextSubString(const String& source, int& startAt, char separator = ',') {
		String result;
		if (startAt >= 0) {

			// Find next comma
			int finish = source.indexOf(separator, startAt);
			if (finish < 0) {

				// Reached end of source string
				if (startAt < source.length()) result = source.substring(startAt);
				startAt = -1;

			} else {

				// Somewhere in middle of source string
				if (finish > startAt) result = source.substring(startAt, finish);
				startAt = finish + 1;
			}
		}
		return result;
	}
}


#endif
