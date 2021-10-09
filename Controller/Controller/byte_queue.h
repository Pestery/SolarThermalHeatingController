#ifndef BYTE_QUEUE_H
#define BYTE_QUEUE_H

// A ByteQueue is a type of character stream buffer which dynamically resizes as required
// When allocating memory, the buffer uses fixed-sized blocks to try to reduce memory fragmentation
class ByteQueue : public Print {

	// Size of an internal allocation block
	static constexpr unsigned blockSize = 32;

	// Amount of data which can be stored within an allocation block
	static constexpr unsigned dataPerBlock = blockSize - sizeof(void*);

	// The layout of the internal memory allocation block
	struct Block {
		Block* next;
		uint8_t data[dataPerBlock];
	};

public:

	// Write a single byte to the queue
	// Returns the number of bytes written, which may be less than requested
	// This method will always return 1, or 0 if there was a memory allocation failure
	size_t write(uint8_t byte) {

		// Make sure there is space within the buffer
		if (!m_blockWrite) {

			// Allocate a new block
			m_blockWrite = (Block*)malloc(sizeof(Block));
			if (!m_blockWrite) return 0;
			m_blockWrite->next = nullptr;
			m_blockRead = m_blockWrite;
			m_offsetWrite = 0;
			m_offsetRead = 0;

		} else if (m_offsetWrite >= dataPerBlock) {

			// Allocate a new block and place at end
			Block* temp = (Block*)malloc(sizeof(Block));
			if (!temp) return 0;
			temp->next = nullptr;
			m_blockWrite->next = temp;
			m_blockWrite = temp;
			m_offsetWrite = 0;
		}

		// Write the byte to the buffer
		m_blockWrite->data[m_offsetWrite++] = byte;
		m_size++;
		return 1;
	}

	// Write a buffer to the queue
	// Returns the number of bytes written, which may be less than requested
	size_t write(const uint8_t *buffer, size_t size) {
		if (buffer && (size > 0)) {
			size_t written = 0;
			while (size) {
				if (!write(*buffer)) break;
				buffer++;
				size--;
				written++;
			}
			return written;
		}
		return 0;
	}

	// Get the number of bytes which can be written before write() will block
	// This class dynamically resizes as needed, so just return a high number
	int availableForWrite() const {
		return 1000;
	}

	// Read, and remove, the next byte from the buffer
	// Returns the next unsigned byte, or -1 if the buffer was empty
	int read() {
		int result = -1;

		// Check if there is any data
		if (m_size) {

			// Record next byte
			result = m_blockRead->data[m_offsetRead++];
			m_size--;

			// Check if current read block has been exhausted
			if (m_offsetRead >= dataPerBlock) {

				// Free exhausted block
				Block* temp = m_blockRead;
				m_blockRead = m_blockRead->next;
				free(temp);

				// Reset offsets
				if (!m_blockRead) m_blockWrite = nullptr;
				m_offsetRead = 0;
			}
		}
		return result;
	}

	// Discard a number of bytes from the buffer
	void discard(int amount) {
		while ((amount-- > 0) && m_size) read();
	}

	// Returns the amount of data within the buffer
	int available() const {
		return m_size;
	}

	// View a byte within the buffer without removing it
	// Returns the unsigned byte at the requested offset, or -1 if the offset was past the end of the buffer
	int peek(size_t offset = 0) const {

		// Check if there is any data
		if (offset < m_size) {

			// Convert relative offset to absolute offset
			offset += m_offsetRead;

			// Check if offset is within first block of data
			if (offset < dataPerBlock) return m_blockRead->data[offset];

			// Find block which contains the requested data
			Block* temp = m_blockRead->next;
			while (temp) {
				offset -= dataPerBlock;
				if (offset < dataPerBlock) return temp->data[offset];
				temp = temp->next;
			}
		}
		return -1;
	}

	// Remove all data from the buffer
	void clear() {

		// Delete blocks
		while (m_blockRead) {
			Block* temp = m_blockRead->next;
			free(m_blockRead);
			m_blockRead = temp;
		}

		// Reset values
		m_blockRead = nullptr;
		m_blockWrite = nullptr;
		m_offsetRead = 0;
		m_offsetWrite = 0;
		m_size = 0;
	}

	// A roll-back point is used to undo incomplete writes to a buffer
	class Rollback {
		ByteQueue* m_parent;
		ByteQueue::Block* m_blockRead;
		ByteQueue::Block* m_blockWrite;
		uint8_t m_offsetRead;
		uint8_t m_offsetWrite;
		unsigned m_size;
	public:

		// Roll-back point constructor
		Rollback() : m_parent(nullptr) {}
		Rollback(ByteQueue& parent) :
			m_parent(&parent),
			m_blockRead(parent.m_blockRead),
			m_blockWrite(parent.m_blockWrite),
			m_offsetRead(parent.m_offsetRead),
			m_offsetWrite(parent.m_offsetWrite),
			m_size(parent.m_size) {
		}

		// Use the roll-back point to erase any data which was written after the point was created
		// Note: If any data has been removed from the buffer since the roll-back point was created then the point will become invalid
		void rollback() {

			// Check if roll-back point is valid
			if (m_parent) {

				// Check if roll-back point saved an empty buffer
				if (m_size == 0) {
					m_parent->clear();
				} else {

					// Check nothing has been read from the buffer
					if ((m_parent->m_blockRead == m_blockRead) && (m_parent->m_offsetRead == m_offsetRead)) {

						// Make a record the the first unused block after the recorded write block
						Block* temp = m_blockWrite ? m_blockWrite->next : nullptr;

						// Copy over data backed up data
						if (m_blockWrite) m_blockWrite->next = nullptr;
						m_parent->m_blockRead = m_blockRead;
						m_parent->m_blockWrite = m_blockWrite;
						m_parent->m_offsetRead = m_offsetRead;
						m_parent->m_offsetWrite = m_offsetWrite;
						m_parent->m_size = m_size;

						// Free any unneeded blocks
						while (temp) {
							m_blockWrite = temp->next;
							free(temp);
							temp = m_blockWrite;
						}
					}

					// Invalidate this roll-back point
					m_parent = nullptr;
				}
			}
		}
	};

	// Set a roll-back point
	// This can be used to erase an incomplete write from the buffer
	// Note: If any data has been removed from the buffer after the roll-back point was created then the point will become invalid
	Rollback createRollbackPoint() {
		return Rollback(*this);
	}

	// Try to add a string to the ring buffer
	// 'data' is string to copy into the buffer
	// 'startAt' is the optional offset from the start of the buffer where the copy should begin
	// 'length' is the optional maximum amount of data to copy. Set to -1 to ignore
	// Returns the total amount of data copied, which may be less than requested
	int stringWrite(const char* data, int startAt = 0, int length = -1) {
		int written = 0;
		if (data && (startAt >= 0)) {

			// Move to the start location
			// Make sure there are no null-characters, which mark the end of the string
			while (startAt && *data) {
				data++;
				startAt--;
			}

			// Start copying data
			// End if null-character is found, or target copy length is reached, or if buffer is full
			while (*data && length && write(*data)) {
				data++;
				length--;
				written++;
			}
		}
		return written;
	}

	// Check if the buffer starts with the 'testString'
	// Returns true on match, or false on mismatch
	bool stringBeginsWith(const char* testString) {
		if (testString && testString[0]) {
			const int iMax = available();
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
			discard(strlen(testString));
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
		const int recvSize = available();
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

	// Convert this ByteQueue to a string
	// After calling this method, the class will be left in the same state as if clear() was called
	String convertToString() {
		String result;
		if (m_size) {
			result.reserve(m_size);
			while (m_size) result += (char)read();
		}
		return result;
	}

	// Create and return a copy of this class
	ByteQueue duplicate() const {
		ByteQueue out;
		for (size_t i=0; i<m_size; i++) out.write(peek(i));
		return out;
	}

	// Default constructors, assignment operators and destructor
	ByteQueue() :
		m_blockRead(nullptr),
		m_blockWrite(nullptr),
		m_offsetRead(0),
		m_offsetWrite(0),
		m_size(0) {
	}
	ByteQueue(const ByteQueue& rhs) = delete;
	ByteQueue(ByteQueue&& rhs) :
		m_blockRead(rhs.m_blockRead),
		m_blockWrite(rhs.m_blockWrite),
		m_offsetRead(rhs.m_offsetRead),
		m_offsetWrite(rhs.m_offsetWrite),
		m_size(rhs.m_size) {
			rhs.m_blockRead = nullptr;
			rhs.m_blockWrite = nullptr;
			rhs.m_offsetRead = 0;
			rhs.m_offsetWrite = 0;
			rhs.m_size = 0;
	}
	ByteQueue& operator = (const ByteQueue& rhs) = delete;
	ByteQueue& operator = (ByteQueue&& rhs) {
		if (&rhs != this) {
			clear();
			m_blockRead = rhs.m_blockRead;
			m_blockWrite = rhs.m_blockWrite;
			m_offsetRead = rhs.m_offsetRead;
			m_offsetWrite = rhs.m_offsetWrite;
			m_size = rhs.m_size;
			rhs.m_blockRead = nullptr;
			rhs.m_blockWrite = nullptr;
			rhs.m_offsetRead = 0;
			rhs.m_offsetWrite = 0;
			rhs.m_size = 0;
		}
		return *this;
	}
	~ByteQueue() {
		clear();
	}

private:

	// Private data
	Block* m_blockRead;
	Block* m_blockWrite;
	uint8_t m_offsetRead;
	uint8_t m_offsetWrite;
	unsigned m_size;
};

#endif
