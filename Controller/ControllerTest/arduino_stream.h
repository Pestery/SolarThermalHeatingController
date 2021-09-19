#ifndef ARDUINO_STREAM_H
#define ARDUINO_STREAM_H

///////////////////////////////////////////////////////////////////
// This file is intended only to be a rough replacement for      //
// functions within the Arduino library, solely used for testing //
///////////////////////////////////////////////////////////////////

// Include headers
#include <string>

// A minimal replacement for the String class within the
class Stream {
	std::string m_writeBuffer;
	std::string m_readBuffer;
public:

	const std::string& debugGetWriteBuffer() const {return m_writeBuffer;}

	void debugResetWriteBuffer() {m_writeBuffer.clear();}
	void debugResetReadBuffer() {m_readBuffer.clear();}

	void debugSetReadBuffer(const char* buffer) {m_readBuffer = buffer;}
	void debugSetReadBuffer(const std::string& buffer) {m_readBuffer = buffer;}

	void write(char b) {
		m_writeBuffer += b;
	}
	int read() {
		int result;
		if (m_readBuffer.empty()) {
			result = -1;
		} else {
			result = m_readBuffer[0];
			m_readBuffer = m_readBuffer.substr(1);
		}
		return result;
	}

	int available() const {return m_readBuffer.size();}
	int availableForWrite() const {return 50;}
};

#endif
