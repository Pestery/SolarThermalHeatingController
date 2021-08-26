#ifndef ARDUINO_STRING_H
#define ARDUINO_STRING_H

///////////////////////////////////////////////////////////////////
// This file is intended only to be a rough replacement for      //
// functions within the Arduino library, solely used for testing //
///////////////////////////////////////////////////////////////////

// Include headers
#include <string>

// Replace other string related functions
bool isWhitespace(char c) {return std::isspace(c);}
#define F(string) (string)

// A minimal replacement for the String class within the
class String {
	std::string m_data;
public:

	String() = default;
	String(uint32_t rhs) : m_data(std::to_string(rhs)) {}
	String(const char* rhs) : m_data(rhs) {}
	String(const char* data, unsigned length) : m_data(data, length) {}
	String(const String& rhs) = default;
	String(const std::string& rhs) : m_data(rhs) {}

	String& operator = (char rhs) {m_data = rhs; return *this;}
	String& operator = (const char* rhs) {m_data = rhs; return *this;}
	String& operator = (const std::string& rhs) {m_data = rhs; return *this;}
	String& operator = (const String& rhs) = default;

	String& operator += (char rhs) {m_data += rhs; return *this;}
	String& operator += (const char* rhs) {m_data += rhs; return *this;}
	String& operator += (const std::string& rhs) {m_data += rhs; return *this;}
	String& operator += (const String& rhs) {m_data += rhs.m_data; return *this;}

	~String() = default;

	int length() const {return m_data.size();}

	char& operator [] (int i) {return m_data[i];}
	const char& operator [] (int i) const {return m_data[i];}

	friend bool operator == (const char* lhs, const String& rhs) {return rhs == lhs;}

	bool operator == (const char* rhs) const {return m_data == rhs;}
	bool operator == (const String& rhs) const {return m_data == rhs.m_data;}

	int toInt() const {return std::stoi(m_data);}
	int toFloat() const {return std::stof(m_data);}

	void reserve(int s) {}

	const char* c_str() const {return m_data.c_str();}

	int indexOf(char c) const {return indexOf(c, 0);}
	int indexOf(char c, unsigned fromIndex) const {
		for (unsigned i=fromIndex; i<m_data.size(); i++) {
			if (m_data[i] == c) return i;
		}
		return -1;
	}

	String substring(unsigned left) const {
		return substring(left, m_data.size());
	}
	String substring(unsigned left, unsigned right) const {
		if (left > right) std::swap(left, right);
		if (left >= m_data.size()) return String();
		if (right > m_data.size()) right = m_data.size();
		return String(std::string(m_data.c_str() + left, right - left));
	}

	void trim() {
		if (!m_data.empty()) {
			int i = 0, j = length() - 1;
			while ((i < length()) && isWhitespace(m_data[i])) i++;
			while ((j > 0) && isWhitespace(m_data[j])) j--;
			if (i <= j) {
				m_data = substring(i, ++j).m_data;
			} else {
				m_data.clear();
			}
		}
	}

	friend inline std::ostream& operator<<(std::ostream& s, const String& in) {s << in.m_data; return s;}

};

#endif
