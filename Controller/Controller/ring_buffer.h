#ifndef RING_BUFFER_H
#define RING_BUFFER_H

// A class used to manage single ring buffer
template <typename T, int maxSize = 4>
class RingBuffer {
public:

	// The type used to store index values within the class.
	typedef uint8_t IndexType;

	// Returns true if the buffer is empty.
	inline bool isEmpty() const __attribute__((always_inline)) {
		return m_empty;
	}

	// Returns true if the buffer is full.
	inline bool isFull() const __attribute__((always_inline)) {
		return !m_empty && (m_head == m_tail);
	}

	// Get the amount of available space within the buffer.
	inline int freeSpace() const __attribute__((always_inline)) {
		return capacity() - size();
	}

	// Get the amount of data within the buffer.
	inline int size() const __attribute__((always_inline)) {
		return (m_head == m_tail) ? (m_empty ? 0 : maxSize) : ((m_tail > m_head) ? (m_tail - m_head) : (m_tail + maxSize - m_head));
	}

	// Get the size of the buffer
	inline int capacity() const __attribute__((always_inline)) {
		return maxSize;
	}

	// Add byte to buffer.
	// Returns true on success, or false if the buffer is full.
	bool push(const T& newData) {
		if (isFull()) {
			return false;
		} else {
			m_buffer[m_tail] = newData;
			m_tail = (m_tail + 1) % maxSize;
			m_empty = false;
			return true;
		}
	}

	// View the next byte within the buffer, but without removing it.
	// This function does not check if the buffer is empty.
	inline const T& peek() const __attribute__((always_inline)) {
		return m_buffer[m_head];
	}

	// View the next byte within the buffer (with an offset), but without removing it.
	// This function does not check if the buffer is empty.
	inline const T& peek(IndexType offset) const __attribute__((always_inline)) {
		return m_buffer[(m_head + offset) % maxSize];
	}

	// Take the next byte from the buffer.
	T pop() {
		IndexType i = m_head;
		if (!m_empty) {
			m_head = (m_head + 1) % maxSize;
			m_empty = (m_head == m_tail);
		}
		return m_buffer[i];
	}

	// Discard a number of bytes from the buffer
	void pop(int amount) {
		while ((amount-- > 0) && !isEmpty()) {
			pop();
		}
	}

	// Clear all data within the buffer and leave it empty.
	void reset() {
		m_head = 0;
		m_tail = 0;
		m_empty = true;
	}

	// Default constructor and destructor
	RingBuffer() {
		reset();
	}
	~RingBuffer() {}

private:

	// Private data
	IndexType m_head;    // Index of the first byte of data within the buffer
	IndexType m_tail;    // Index of the first byte of free space, after the end of the data within the buffer
	bool m_empty;        // True if the buffer is completely empty
	T m_buffer[maxSize]; // Data buffer
};



#endif
