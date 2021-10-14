#ifndef SENSOR_LOG_H
#define SENSOR_LOG_H

// Include headers
#include <SPI.h>
#include <SD.h>
#include "sensor_record.h"
#include "time_keeper.h"
#include "byte_queue.h"
#include "ring_buffer.h"

// Manages sensor records stored on the controller
class SensorLog {
public:

	// Maximum number of sensor records to keep in memory
	static constexpr unsigned maxLoadedRecords = 4;

	// Maximum allowed file size
	// When a file exceeds this size a new file will be started
	static constexpr uint32_t maxFileSize = 10ul * 1024ul * 1024ul; // Size in bytes

	// Get the DateTime of the last created record
	DateTime lastCreated() const {
		return m_lastCreated;
	}

	// Get or set the DateTime of the last record which was successfully uploaded to the server
	void lastUploaded(DateTime dt) {
		m_lastUploaded = (dt > 0) ? dt : DateTime(1);
	}
	DateTime lastUploaded() const {
		m_lastUploaded;
	}

	// Check if an upload is required
	// Returns true if there is any data which has not yet been uploaded, or false if everything is already uploaded
	bool isUploadRequired() const {
		return m_lastCreated > m_lastUploaded;
	}

	// Check if the last-uploaded value has been set
	// Returns true if it has been set, or false if not yet
	bool lastUploadedIsSet() const {
		return m_lastUploaded > 0;
	}

	// Reset the last-uploaded value
	// This will prevent any further uploads until the last-upload is next supplied by the database
	// This should be used when the GUID or server is changed
	void lastUploadedReset() {
		m_lastUploaded = 0;
	}

	// Update the sensor log using a key and value pair
	// Returns true if the key-value was used, or false if it does not apply to this class
	bool updateWithKeyValue(String& key, String& value) {
		if (key == F("\"lastRecord\"")) {
			lastUploaded(DateTime(value));
			return true;
		} else {
			return false;
		}
	}

	// Generate a new record and store it on the SD card
	bool record(TimeKeeper& timeKeeper) {

		// Check if the time has been set
		if (!timeKeeper.isValid()) return false;

		// Record data
		SensorRecord r;
		r.readAll(&timeKeeper);

		// Check if using the memory buffer or the SD card
		if (m_buffer) {

			// Add buffer to record
			// If buffer is full then erase the oldest record
			if (m_buffer->isFull()) m_buffer->pop();
			m_buffer->push(r);

			// Record time of new record
			// Return success
			m_lastCreated = r.dateTime;
			return true;

		} else {

			// Save record on SD card
			String fileName = fileNameFromId(m_fileId);
			File f = SD.open(fileName, O_READ | O_WRITE | O_CREAT);
			if (f) {

				// Check if the file has reached the maximum size allowed
				if (f.size() > maxFileSize) {

					// Close this file
					f.close();

					// Increment file ID and open a new file
					fileName = fileNameFromId(++m_fileId);
					f = SD.open(fileName, O_READ | O_WRITE | O_CREAT);
				}

				// Check that file is still open
				// Might have got closed in previous section
				if (f) {

					// Check if the file is empty or the last record is invalid
					// In either case, assume the whole file is garbage and start again
					if ((f.size() == 0) || !getRecordLast(f)) {

						// Move to start of file and write headers
						f.seek(0);
						f.println(SensorRecord::toCsvHeaders());
					}

					// Save the sensor record
					f.println(r.toCsv());

					// Close file
					f.close();

					// Record time of new record
					// Return success
					m_lastCreated = r.dateTime;
					return true;
				}
			}

			// If here the return failure
			return false;
		}
	}

	// Get a number of records from the SD card
	// The fetched records will be stored in the provided 'buffer'
	// A maximum of 'count' records will be fetched, starting at the given DateTime
	// Returns the number of records fetched
	unsigned fetch(DateTime startAt, SensorRecord* buffer, unsigned count) {
		if (count < 1) return 0;
		int found = 0;

		// Check if using the memory buffer or the SD card
		if (m_buffer) {

			// Define working variables
			unsigned i = 0, iMax = m_buffer->size();

			// If there are more records available than requested
			// then move to the first one which matches the request
			while ((iMax - i) > count) {
				if (m_buffer->peek(i).dateTime < startAt) i++; else break;
			}

			// Copy results to output buffer
			while ((i < iMax) && (count > 0)) {
				*buffer = m_buffer->peek(i++);
				buffer++;
				found++;
				count--;
			}

		} else {

			// Define working variables
			uint32_t fileId;
			String s;
			File f;

			// Find the file which contains the first record
			f = getRecordFile(startAt, &fileId);
			if (!f) return 0;

			// Find the first record of the requested records within the file
			getRecord(f, startAt, *buffer);
			found++;

			// Loop to get any subsequent requested records
			while (--count > 0) {
				buffer++;

				// Get next record from same file, if able
				if ((f.position() == f.size()) || !buffer->fromCsv(readLine(f))) {

					// Move to next file, if any
					f.close();
					f = SD.open(fileNameFromId(++fileId), FILE_READ);
					if (!f) return found;

					// Get first record from new file
					*buffer = getRecordFirst(f);
				}
				found++;
			}

			// Finish up
			f.close();
		}
		return found;
	}

	// Add a number of not-yet-uploaded records to the provided buffer
	// The records will be uploaded in the form of a JSON array
	void fetchForUpload(ByteQueue& outBuffer, int maxRecordsToAdd = 4) {

		// Error check
		if (maxRecordsToAdd < 1) maxRecordsToAdd = 1;

		// Add start-of-array marker
		outBuffer.print('[');

		// Get some records to upload
		SensorRecord records[maxRecordsToAdd];
		unsigned found = fetch(m_lastUploaded + DateTime::Type(1), records, maxRecordsToAdd);
		if (found > 0) {

			// Loop through found records
			bool anythingAdded = false;
			for (unsigned i=0; i<found; i++) {

				// If the record is newer than the last uploaded, then add it to the buffer
				if (records[i].dateTime > m_lastUploaded) {
					if (anythingAdded) outBuffer.print(',');
					records[i].toJson(outBuffer);
					anythingAdded = true;
				}
			}
		}

		// Add end-of-array marker
		outBuffer.print(']');
	}

	// List all files on the SD card
	static void listAllFiles() {
		File f = SD.open("/");
		listAllFiles(f, 0);
		f.close();
	}

	// List all files with a directory on the SD card
	static void listAllFiles(File directory, int indent) {
		while (true) {

			// Get next file
			File f = directory.openNextFile();
			if (!f) return;

			// Increment the correct number of tabs
			for (int i=0; i<indent; i++) Serial.print('\t');

			// Check if found directory or file
			if (f.isDirectory()) {
				Serial.print(f.name());
				Serial.println('/');
				listAllFiles(f, indent + 1);
			} else {
				Serial.print(f.name());
				Serial.print('\t');
				Serial.println(f.size());
			}
			f.close();
		}
	}

	// Initialise the sensor log
	// This will try to read from the SD card
	// Returns true on success, or false on failure
	bool begin(bool useSdCard) {
		if (useSdCard) {

			// Make sure the target directory exists
			if (!makeRecordDirectory()) return false;

			// Get the latest record file
			// New records will be added to this file
			m_fileId = latestFileId();

			// Get the last record from within the file, if any
			// If the file does not contain any records then erase it and move to the previous file, if any
			while(true) {

				// Try to get record from latest file
				SensorRecord sr = getRecordLast(m_fileId);
				if (sr) {

					// Record exists
					// Record time and exit loop
					m_lastCreated = sr.dateTime;
					break;

				} else {

					// File does not contain any records, so delete it
					SD.remove(fileNameFromId(m_fileId));
					if (m_fileId > 1) {

						// Move to the previous file
						m_fileId--;

					} else {

						// There are no previous files
						// Use a default time and exit loop
						m_lastCreated = 0;
						break;
					}
				}
			}

			// Return success
			return true;

		} else {

			// Create buffer and return result
			typedef RingBuffer<SensorRecord,4> Type;
			m_buffer = (Type*)malloc(sizeof(Type));
			if (m_buffer) {
				m_buffer->reset();
				return true;
			} else {
				return false;
			}
		}
	}

	// Default constructor
	SensorLog() :
		m_lastUploaded(0),
		m_lastCreated(0),
		m_fileId(1),
		m_buffer(nullptr) {
	}
	~SensorLog() {
		if (m_buffer) {
			free(m_buffer);
			m_buffer = nullptr;
		}
	}

private:

	// Move to start of current line within the file
	// Search for the previous newline character and position the file cursor after that, or at the start of the file
	void moveToPreviousNewLine(File& f) {

		// Get current position
		// If already at the start of the file then exit here
		uint32_t p = f.position();
		if (p == 0) return;

		// Check if the cursor is at the end of the file
		// If so then move it back one because there will be no existing data at that position
		if (p == f.size()) f.seek(--p);

		// Loop until newline character is found
		while (true) {
			if (p == 0) {

				// Found start of file. Exit here
				return;

			} else if (f.peek() == '\n') {

				// Found newline character
				// Move forward by one and then exit
				f.seek(++p);
				return;

			} else {

				// Did not find anything useful
				// Move backward by one
				f.seek(--p);
			}
		}
	}

	// Read the next line from the file
	// If the line does not end with a newline character then it will be treated as incomplete and not returned
	// Leaves the file cursor on the next line after the line
	String readLine(File& f) {
		String s;
		int c;
		while (true) {
			c = f.read();
			if (c == -1) return String();
			if (c == '\n') return s;
			if (c != '\r') s += (char)c;
		}
	}

	// Get the first record from within a given file
	// Leaves the file cursor on the next line after the record
	SensorRecord getRecordFirst(uint32_t fileId, uint32_t* outPosition = nullptr) {
		File f = SD.open(fileNameFromId(fileId));
		SensorRecord out;
		out.dateTime = 0;
		if (f) {
			out = getRecordFirst(f, outPosition);
			f.close();
		}
		return out;
	}
	SensorRecord getRecordFirst(File& f, uint32_t* outPosition = nullptr) {
		SensorRecord sr;

		// Move to start of file
		f.seek(0);

		// Skip first line which should be the headers
		while (true) {
			int c = f.read();
			if (c == -1) return sr;
			if (c == '\n') break;
		}

		// Get the next line, decode it, and return the record
		// If the first attempt failed then try once more before giving up
		for (int i=0; i<2; i++) {
			if (outPosition) *outPosition = f.position();
			String s = readLine(f);
			if (sr.fromCsv(s)) return sr;
		}
		sr.dateTime.reset();
		return sr;
	}

	// Get the last record from within a given file
	// Leaves the file cursor on the next line after the record
	SensorRecord getRecordLast(uint32_t fileId, uint32_t* outPosition = nullptr) {
		File f = SD.open(fileNameFromId(fileId));
		SensorRecord out;
		out.dateTime = 0;
		if (f) {
			out = getRecordLast(f, outPosition);
			f.close();
		}
		return out;
	}
	SensorRecord getRecordLast(File& f, uint32_t* outPosition = nullptr) {
		SensorRecord sr;

		// Move to end of file
		// Then move backward at avoid any newline and carriage return characters
		uint32_t p = f.size();
		while (p > 0) {
			f.seek(--p);
			int c = f.peek();
			if ((c != '\n') && (c != '\r')) break;
		}

		// Enter loop to try and get last record within file
		while (true) {

			// Move backward to just after the previous newline character, or the start of the file
			moveToPreviousNewLine(f);
			p = f.position();
			if (p == 0) break;

			// Try to get a record from the current location
			if (sr.fromCsv(readLine(f))) {
				if (outPosition) *outPosition = p;
				return sr;
			}

			// If here then failed to get a record
			// Move back to before the previous newline character
			if (p < 2) break;
			f.seek(p - 2);
		}

		// If here then failed to find record
		sr.dateTime.reset();
		return sr;
	}

	// Find the file which contains a given DateTime
	// Will return a file if it contains the range of times which encapsulate the requested DateTime
	// If no suitable file is found then an empty file pointer will be returned instead
	File getRecordFile(DateTime dt, uint32_t* outFileId = nullptr) {
		SensorRecord sr;
		File f;

		// Check if the record is within the most recent file
		f = SD.open(fileNameFromId(m_fileId), FILE_READ);
		if (f) {
			sr = getRecordFirst(f);
			if (sr && (dt >= sr.dateTime)) {
				if (outFileId) *outFileId = m_fileId;
				return f;
			}
			f.close();
		}

		// Check if the requested time is before the first record
		f = SD.open(fileNameFromId(1), FILE_READ);
		if (f) {
			sr = getRecordLast(f);
			if (dt <= sr.dateTime) {
				if (outFileId) *outFileId = 1;
				return f;
			}
			f.close();
		}

		// Loop through all files to find the one which contains the record
		uint32_t id, idMin = 1, idMax = m_fileId;
		do {

			// Get next file to check
			id = idMin + ((idMax - idMin) / 2);

			// Open file
			f = SD.open(fileNameFromId(id));
			if (!f) return f;

			// Check if requested record is before this file
			sr = getRecordFirst(f);
			if (!sr || (dt < sr.dateTime)) {
				idMax = id - 1;
			} else {

				// Check if the requested record is within this file
				sr = getRecordLast(f);
				if (dt <= sr.dateTime) {
					if (outFileId) *outFileId = id;
					return f;
				} else {
					idMin = id + 1;
				}
			}

			// Close file before end of loop
			f.close();

		} while (idMin < idMax);

		// If here the just return the lowest file found
		if (outFileId) *outFileId = idMin;
		return SD.open(fileNameFromId(idMin));
	}

	// Get a record from a file using a DateTime
	// If an exact match for the DateTime is not found then get the closest value
	// Return zero if found, -1 if the DateTime is before the first record (copy first record), +1 if the DateTime is after the last record (copy last record)
	int getRecord(uint32_t fileId, DateTime dt, SensorRecord& out, uint32_t* outPosition = nullptr) {
		File f = SD.open(fileNameFromId(fileId));
		int res = -2;
		if (f) {
			res = getRecord(f, dt, out, outPosition);
			f.close();
		}
		return res;
	}
	int getRecord(File& f, DateTime dt, SensorRecord& out, uint32_t* outPosition = nullptr) {
		uint32_t p, pMin, pMax;

		// Check the first record within the file
		out = getRecordFirst(f, &pMin);
		if (out.dateTime == dt) {
			if (outPosition) *outPosition = pMin;
			return 0; // Exact match
		} else if (dt < out.dateTime) {
			if (outPosition) *outPosition = pMin;
			return -1; // Requested record is before this file
		}

		// Check the last record within the file
		out = getRecordLast(f, &pMax);
		if (out.dateTime == dt) {
			if (outPosition) *outPosition = pMax;
			return 0; // Exact match
		} else if (out.dateTime < dt) {
			if (outPosition) *outPosition = pMax;
			return 1; // Requested record is after this file
		}

		// If here then the record is within this file
		// Search the file for it and return the nearest match
		while(true) {

			// Move to next position to check
			p = pMin + ((pMax - pMin) / 2);
			f.seek(p);
			moveToPreviousNewLine(f);
			p = f.position();

			// Get the record from this position
			// Check if the found record is too high or low, or just right
			if (out.fromCsv(readLine(f))) {
				if (outPosition) *outPosition = p;
				if (out.dateTime == dt) {

					// Found record
					return 0;

				} else if (p == pMin) {

					// Getting close to minimum resolution
					// Advance the potion to the next record to prevent infinite loop
					p = f.position();
					pMin = p;
					if (pMin >= pMax) return 0;

				} else if (out.dateTime < dt) {

					// Found record is too low
					pMin = p;

				} else {

					// Found record is too high
					pMax = p;
				}
			} else {
				out.dateTime.reset();
				return 0;
			}
		}
	}

	// Get the name of the folder where records should be kept
	String recordDirectory() {
		return String(F("RECORDS"));
	}

	// Make sure the record directory exists
	// Returns true if it does exist or was created
	// Returns false if it does not exist and could not be created
	bool makeRecordDirectory() {
		String d(recordDirectory());

		// Check if anything exists
		if (SD.exists(d)) {
			File f = SD.open(d);
			if (f) {

				// Check if the object is a directory or a file
				// If it is a file then try to delete it
				if (f.isDirectory()) {
					f.close();
					return true;
				} else {
					f.close();
					SD.remove(d);
				}
			}
		}

		// If here then the directory does not exist, so try to create it
		return SD.mkdir(d);
	}

	// Generate file name from file number
	String fileNameFromId(uint32_t id) {
		String fileName(recordDirectory());
		fileName += '/';
		fileName += id;
		fileName += F(".CSV");
		return fileName;
	}

	// Get the number used to name the latest record file
	uint32_t latestFileId() {

		// Define values
		uint32_t min = 1;
		uint32_t max = 1e6;
		uint32_t id;

		// Find file
		do {

			// Get next number to check
			id = min + ((max - min) / 2);

			// Check if file exists
			// Adjust search range accordingly
			if (SD.exists(fileNameFromId(id))) {
				min = id + 1;
			} else {
				max = id;
			}
		} while (min != max);
		return (min > 1) ? (min - 1) : 1;
	}

	// Private data
	DateTime m_lastUploaded;
	DateTime m_lastCreated;
	uint32_t m_fileId;
	RingBuffer<SensorRecord,4>* m_buffer;
};

#endif
