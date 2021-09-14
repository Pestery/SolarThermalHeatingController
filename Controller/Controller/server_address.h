#ifndef SERVER_ADDRESS_H
#define SERVER_ADDRESS_H

// Include headers
#include "misc.h"

// A class used to hold and manage server address information
// This is separate from the main server class for the purpose of unit testing
class ServerAddress {
public:

	// Maximum length of a server address (http://server-address/page-address/more/page/address)
	static constexpr int maxHostLength = 45 + 1; // Maximum length of IPv6 address (45), plus null character (+1)

	// Maximum length of a web page address (http://server-address/page-address/more/page/address)
	static constexpr int maxPathLength = 96 - 4 - maxHostLength;

	// Check if the connection should be secured (HTTPS or HTTP)
	// Returns true for secured (HTTPS), or false for unsecured (HTTP)
	inline bool secure() const __attribute__((always_inline)) {
		return m_secure;
	}

	// Get the host, or server address
	inline const char* host() const __attribute__((always_inline)) {
		return m_host;
	}

	// Get the port number to use when connecting
	inline uint16_t port() const {
		return m_port ? m_port : (m_secure ? 443 : 80);
	}

	// Get the path, or address of the web page within the host site
	inline const char* path() const __attribute__((always_inline)) {
		return m_path;
	}

	// Get a string representation of the current server address
	String get() const {
		String result;
		result.reserve(32);
		result = F("http");
		if (m_secure) result += 's';
		result += F("://");
		result += m_host;
		if (m_port) {
			result += ':';
			result += (int)m_port;
		}
		result += m_path;
		return result;
	}

	// Decode new address string
	// Returns true if address is valid and has been saved to local variables, or false on failure (no changes)
	inline bool set(const String& address) __attribute__((always_inline)) {return set(address.c_str(), address.length());}

	// Decode new address string
	// Returns true if address is valid and has been saved to local variables, or false on failure (no changes)
	bool set(const char* address, const int length) {
		bool secure;
		String host;
		String temp;
		uint16_t port;
		const char *i, *j;

		// A sample address:
		// http://localhost:5000/api/System

		// Find the first colon in the address
		// It should be followed by two forward slashes
		// This is the separator between the protocol type and the rest of the address
		i = strchr(address, ':'); // Find first colon
		if (!i || // If colon not found then error
			(i[1] != '/') || // If colon not followed by two forward slashes then error
			(i[2] != '/')) return false;
		temp = Misc::makeString(address, i - address);
		address = i + 3; // Move address pointer forward, past the protocol section

		// Check what type of protocol was requested
		temp.toLowerCase();
		if (temp == F("http")) {
			secure = false;
		} else if (temp == F("https")) {
			secure = true;
		} else {
			return false;
		}
		temp = String();

		// Get the host name and optional port number
		// This will also locate the separator used to separate the host component from the path component
		i = strchr(address, ':');
		j = strchr(address, '/');
		if (!j) return false; // No forward slash, so no path component, so error
		if (i && (i < j)) {

			// A colon was found in the host component
			// Get the port number
			if ((i + 1) == j) {
				port = 0;
			} else {
				temp = Misc::makeString(i + 1, j - i - 1);
				port = temp.toInt();
				temp = String();
			}

			// Get host name/address
			host = Misc::makeString(address, i - address);

		} else {

			// No colon, or colon is after path separator
			host = Misc::makeString(address, j - address);
			port = 0;
		}

		// Check length of address are not too long
		if ((host.length() + 1) > maxHostLength) return false;
		if ((strlen(j) + 1) > maxPathLength) return false;

		// If here then success
		// Copy data to local values and return
		strcpy(m_host, host.c_str());
		strcpy(m_path, j);
		m_port = port;
		m_secure = secure;
		return true;
	}

	// Default constructor
	ServerAddress() {
		memset(m_host, 0, maxHostLength);
		memset(m_path, 0, maxPathLength);
		m_port = 0;
		m_secure = false;
	}

private:
	char m_host[maxHostLength];
	char m_path[maxPathLength];
	uint16_t m_port;
	bool m_secure;
};

#endif
