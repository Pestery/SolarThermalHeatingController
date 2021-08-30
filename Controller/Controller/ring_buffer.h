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

	// Clear all data within the buffer and leave it empty.
	inline void reset() __attribute__((always_inline)) {
		m_head = 0;
		m_tail = 0;
	}

	// Change the size of the ring buffer
	// This will erase any data which exists within the buffer
	bool resize(IndexType newSize) {
		if (newSize == m_size) {
			return true;
		} else {
			uint8_t* newBuffer = (uint8_t*)realloc(m_buffer, newSize);
			if (newBuffer) {
				m_buffer = newBuffer;
				m_size = newSize;
				reset();
				return true;
			} else {
				return false;
			}
		}
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
