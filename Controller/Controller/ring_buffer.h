#ifndef RING_BUFFER_H
#define RING_BUFFER_H

// A class used to manage single ring buffer
class RingBuffer {
public:

	// The type used to store index values within the class.
	typedef uint8_t IndexType;

	// Returns true if the buffer is empty.
	inline bool isEmpty() const __attribute__((always_inline)) {
		return m_head == m_tail;
	}

	// Returns true if the buffer is full.
	inline bool isFull() const __attribute__((always_inline)) {
		return ((m_tail + 1) % m_size) == m_head;
	}

	// Get the amount of available space within the buffer.
	inline int freeSpace() const __attribute__((always_inline)) {
		return (m_size - 1 + m_head - m_tail) % m_size;
	}

	// Get the amount of data within the buffer.
	inline int size() const __attribute__((always_inline)) {
		return (m_tail >= m_head) ? (m_tail - m_head) : (m_tail + m_size - m_head);
	}

	// Get the size of the buffer
	inline int capacity() const __attribute__((always_inline)) {
		return m_size - 1;
	}

	// Add byte to buffer.
	// Returns true on success, or false if the buffer is full.
	bool push(uint8_t newData) {
		if (isFull()) {
			return false;
		} else {
			m_buffer[m_tail] = newData;
			m_tail = (m_tail + 1) % m_size;
			return true;
		}
	}

	// View the next byte within the buffer, but without removing it.
	// This function does not check if the buffer is empty.
	inline uint8_t peek() const __attribute__((always_inline)) {
		return m_buffer[m_head];
	}

	// View the next byte within the buffer (with an offset), but without removing it.
	// This function does not check if the buffer is empty.
	inline uint8_t peek(IndexType offset) const __attribute__((always_inline)) {
		return m_buffer[(m_head + offset) % m_size];
	}

	// Take the next byte from the buffer.
	uint8_t pop() {
		uint8_t result = m_buffer[m_head];
		if (m_head != m_tail) {
			m_head = (m_head + 1) % m_size;
		}
		return result;
	}

	// Discard a number of bytes from the buffer
	void pop(int amount) {
		while ((amount-- > 0) && !isEmpty()) {
			pop();
		}
	}

	// Clear all data within the buffer and leave it empty.
	inline void reset() __attribute__((always_inline)) {
		m_head = 0;
		m_tail = 0;
	}

	// Change the size of the ring buffer
	// This will erase any data which exists within the buffer
	bool resize(IndexType newSize) {
		if (newSize == m_size) {
			reset();
			return true;
		} else {
			uint8_t* newBuffer = (uint8_t*)realloc(m_buffer, newSize);
			if (newBuffer) {
				m_buffer = newBuffer;
				m_size = newSize;
				reset();
				return true;
			} else {
				reset();
				return false;
			}
		}
	}

	// Try to add a string to the ring buffer
	// 'data' is string to copy into the buffer
	// 'startAt' is the optional offset from the start of the buffer where the copy should begin
	// 'length' is the optional maximum amount of data to copy. Set to -1 to ignore
	// Returns the total amount of data copied, which may be less than requested
	int stringPush(const char* data, int startAt = 0, int length = -1) {
		int pushed = 0;
		if (data && (startAt >= 0)) {

			// Move to the start location
			// Make sure there are no null-characters, which mark the end of the string
			while (startAt && *data) {
				data++;
				startAt--;
			}

			// Start copying data
			// End if null-character is found, or target copy length is reached, or if buffer is full
			while (*data && length && push(*data)) {
				data++;
				length--;
				pushed++;
			}
		}
		return pushed;
	}

	// Check if the buffer starts with the 'testString'
	// Returns true on match, or false on mismatch
	bool stringBeginsWith(const char* testString) {
		if (testString && testString[0]) {
			const int iMax = size();
			int i = 0;
			while (true) {

				// Get next character within testString
				// Check if end of testString
				const char c = testString[i];
				if (!c) return true;

				// Check if end of receive buffer
				if (i >= iMax) return false;

				// Check for data mismatch
				// If match then increment offset
				if (c != (char)peek(i++)) return false;
			}
		}
		return false;
	}

	// Check if the buffer starts with the 'testString', and then clear that data if so
	// If the data matches then remove the data and return true
	// If the data does not match then return false
	bool stringCompareAndClearOnMatch(const char* testString) {
		if (testString && stringBeginsWith(testString)) {
			pop(strlen(testString));
			return true;
		} else {
			return false;
		}
	}

	// Find next occurrence of the given string within the buffer
	// Returns the offset of the start of the string within the buffer, or -1 if not found
	int stringIndexOf(const char* testString, int startAt = 0) {
		if (!testString) return -1;

		// Loop through testString
		int i = 0;
		const int recvSize = size();
		while (true) {

			// Get next character within testString
			// Check if end of testString
			const char c = testString[i];
			if (!c) return startAt;

			// Check if end of receive buffer
			if ((startAt + i) >= recvSize) return -1;

			// Check for data mismatch
			// If mismatch then increment startAt value and reset offset
			// If match then increment offset
			if (c != (char)peek(startAt + i)) {
				startAt++;
				i = 0;
			} else {
				i++;
			}
		}
		return -1;
	}

	// Default constructor and destructor
	RingBuffer(IndexType size = 16) :
		m_size(size),
		m_buffer(nullptr) {
			if (m_size < 1) m_size = 1; // Minimum size which should cause isFull() to always return true
			m_buffer = (uint8_t*)malloc(m_size);
			reset();
	}
	~RingBuffer() {
		if (m_buffer) {
			free(m_buffer);
			m_buffer = nullptr;
		}
	}

private:

	// Private data
	IndexType m_head;  // Index of the first byte of data within the buffer
	IndexType m_tail;  // Index of the first byte of free space, after the end of the data within the buffer
	IndexType m_size;  // Size of the buffer
	uint8_t* m_buffer; // Data buffer
};



#endif
