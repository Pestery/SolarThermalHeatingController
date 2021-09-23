#ifndef JSON_DECODER_H
#define JSON_DECODER_H

// Include headers
#include "misc.h"

// The JsonDecoder class is used to decode JSON strings.
class JsonDecoder {
public:

	// Get last found name
	String& name() {return m_name;}

	// Get last found value
	String& value() {return m_value;}

	// Check if there was an error while decoding
	// Returns true if there was an error, or false if no errors
	bool hadError() const {return m_hadError;}

	// Get the next name-value pair within the JSON string
	// Returns true on success, or false if the end of the data was reached
	bool fetch() {

		// Skip leading white-spaces
		skipWhiteSpaces();

		// Define working variables
		unsigned nameStart = m_index;
		unsigned nameEnd = 0;
		unsigned valueStart = 0;
		unsigned valueEnd = 0;
		int indentation = 0;
		bool escaped = false;
		bool inString = false;
		bool foundSeparator = false;

		// Loop until end of string is reached, or exit early
		while (m_index < m_length) {

			// Get next character
			const char c = m_source[m_index++];

			// Check for start of value
			if ((valueStart == 0) && foundSeparator && !isWhitespace(c)) {
				valueStart = m_index - 1;
			}

			// Check escape character status
			if (c == '\\') {
				escaped = !escaped;
			} else if (escaped) {
				escaped = false;
			} else {

				// Check for quote characters
				if (c == '"') {
					inString = !inString;
				} else if (!inString) {

					// Check what type of character was found

					if ((c == '{') || (c == '[')) {

						// Found brackets which mark the start JSON objects or arrays
						// Increase indentation level
						// Ignore everything which is not on indentation level zero
						indentation++;

					} else if ((c == '}') || (c == ']')) {

						// Found brackets which mark the end JSON objects or arrays
						// Decrease indentation level
						// Ignore everything which is not on indentation level zero
						// Make sure we have not gone below zero, because that would indicate the end of our data
						if (--indentation < 0) {

							// Do some general checks to make sure everything is in order
							// The name-value separator should have been found
							if (foundSeparator) {

								// The end position for the value should be recorded if it has not already
								if (valueEnd == 0) valueEnd = m_index - 1;

								// Check the lengths of the name and value strings
								if ((nameStart < nameEnd) && (valueStart < valueEnd)) {
									m_name = Misc::makeString(m_source + nameStart, nameEnd - nameStart);
									m_value = Misc::makeString(m_source + valueStart, valueEnd - valueStart);
									m_index = m_length; // Reached end of data
									return true;
								}
							}

							// If here then an error occurred
							errorOccured();
							return false;
						}

					} else if (indentation != 0) {

						// If here then the indentation is not zero
						// This means we are in a child JSON object or array
						// Nothing to do here until the indentation is back to zero

					} else if (c == ':') {

						// Found the marker which separates names and values ':'
						// Record that the separator was found, and check for multiple separators, which would be an error
						// If the end position of the name is not set yet then set that too
						if (!foundSeparator) {
							foundSeparator = true;
							if (nameEnd == 0) nameEnd = m_index - 1;
						} else {
							errorOccured();
							return false;
						}

					} else if (c == ',') {

						// Found the marker which separates lines ','
						// Make sure the line is not empty
						// If still at the starting position then it is empty
						if (nameStart == (m_index - 1)) {
							m_name = String();
							m_value = String();
							return true;
						}

						// Do some general checks to make sure everything is in order
						// The name-value separator should have been found
						if (foundSeparator) {

							// The end position for the value should be recorded if it has not already
							if (valueEnd == 0) valueEnd = m_index - 1;

							// Check the lengths of the name and value strings
							if ((nameStart < nameEnd) && (valueStart < valueEnd)) {
								m_name = Misc::makeString(m_source + nameStart, nameEnd - nameStart);
								m_value = Misc::makeString(m_source + valueStart, valueEnd - valueStart);
								return true;
							}
						}

						// If here then an error occurred
						errorOccured();
						return false;

					} else if (isWhitespace(c)) {

						// Found white-space character
						// This could be after the name, or before or after the value
						// If anywhere else then probably an error
						if (nameEnd == 0) {

							// White space after name and before name-value separator
							nameEnd = m_index - 1;

						} else if (foundSeparator && (valueStart == 0)) {

							// White space after name-value separator and before value
							// Nothing to do here

						} else if ((valueStart != 0) && (valueEnd == 0)) {

							// White space after value and before end of line
							valueEnd = m_index - 1;

						} else {

							// If here then found a white space probably in the middle of something
							// This is not allowed and is an error
							errorOccured();
							return false;
						}

						// Skip any remaining white spaces
						skipWhiteSpaces();

					} else {

						// Found non-white-space character
						// Might be the start of the value, or just part of something else
						if (foundSeparator && (valueStart == 0)) {

							// Found start of value
							valueStart = m_index - 1;
						}
					}
				}
			}
		}
		return false;
	}

	// JSON decoder constructor
	// The source is expected to be a null-terminated string
	// The source must continue to exist while this class is using it
	JsonDecoder(const String& source) : JsonDecoder(source.c_str()) {}
	JsonDecoder(const char* source) :
		m_source(source),
		m_length(source ? strlen(source) : 0),
		m_index(0),
		m_withinQuotes(false),
		m_hadError(false),
		m_name(),
		m_value() {

		// Loop through start of source and check for '{' character
		// Skip everything up to and including this character
		skipWhiteSpaces();
		if ((m_index < m_length) && (m_source[m_index] == '{')) {
			m_index++;
		}
	}

private:

	// A shorthand method to flag an error
	inline void errorOccured() __attribute__((always_inline)) {
		m_hadError = true;
		m_index = m_length;
	}

	// Move the index forward to skip white space characters
	inline void skipWhiteSpaces() __attribute__((always_inline)) {
		while ((m_index < m_length) && isWhitespace(m_source[m_index])) m_index++;
	}

	// Private data
	const char* m_source;
	unsigned m_length;
	unsigned m_index;
	bool m_withinQuotes;
	bool m_hadError;
	String m_name;
	String m_value;
};

#endif
