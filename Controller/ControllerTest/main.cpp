#include <algorithm>
#include <chrono>
#include <iostream>
#include "unit_test.h"

// Include headers which contain replacement Arduino functions used for testing
#include "arduino_print.h"
#include "arduino_stream.h"
#include "arduino_pins.h"
#include "arduino_string.h"

// Include headers which contain test code
#include "test_misc.h"
#include "test_json_decoder.h"
#include "test_ring_buffer.h"
#include "test_timer.h"
#include "test_state_flags.h"
#include "test_date_time.h"
#include "test_sensor_record.h"
#include "test_interconnect.h"
#include "test_server_address.h"
#include "test_byte_queue.h"

// Declare functions
std::stringstream& exceptionToStream(const std::exception& error, std::stringstream& stream, unsigned indentLevel, bool indentAutoIncrement) noexcept;

/* Main function */
int main() {
	try {
		UnitTest::runAllTests();
		return EXIT_SUCCESS;
	} catch(const std::exception& e) {
		std::cerr << "Exception in main():" << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << "Exception of non-standard type in main()" << std::endl;
		return EXIT_FAILURE;
	}
}


///////////////////////////////////////////////////////
// Below code is taken from unit_test.cpp            //
// It has been included in this file to ensure there //
// is only one source file, making compiling easier. //
///////////////////////////////////////////////////////

/* Custom error messages. */
const char* UnitTest::ExceptionThrow::what() const noexcept {
	return "Expected command to throw exception, but it did not";
}

/* Run all test classes which have been defined. */
void UnitTest::runAllTests() {

	// Get list of tests
	std::vector<UnitTest*>& list = get();

	// Sort list
	std::sort(list.begin(), list.end(), sortMethod);

	// Define variables
	std::chrono::high_resolution_clock::time_point t1, t2;
	std::chrono::duration<double,std::milli> dt;
	std::stringstream errorOut;
	int total = 0;
	int errors = 0;

	// Run tests
	std::cout << "Running tests..." << std::endl;
	for (UnitTest* t : list) {
		if (!t) continue;
		const char* comment = "FAILED";
		total++;

		// Begin recording test runtime
		std::cout << t->label() << ": ";
		t1 = std::chrono::high_resolution_clock::now();

		// Run test
		// Check for and record any errors
		if (t->isEnabled()) {
			try {
				t->runTest();
				comment = "Pass";
			} catch (const std::exception& e) {
				errors++;
				std::stringstream ss;
				exceptionToStream(e, ss);
				errorOut << t->errorString(ss.str()) << std::endl;
			} catch (...) {
				errorOut << t->errorString("unknown exception") << std::endl;
				errors++;
			}
		} else {
			comment = "SKIPPED";
		}

		// End recording test runtime
		// Display test runtime
		t2 = std::chrono::high_resolution_clock::now();
		dt = t2 - t1;
		std::cout << std::string(40 - t->label().size(), ' ') << comment;
		if (t->isEnabled()) std::cout << " - " << dt.count() << " ms";
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// Output result
	if (errors) {
		std::cout << "---------------- Errors ----------------" << std::endl << std::endl;
		std::cout << errorOut.str();
		std::cout << "------- " << errors << " out of " << total << " tests failed -------" << std::endl;
	} else {
		std::cout << "All " << total << " tests passed." << std::endl;
	}
}

/* Unit test constructor.
 * @param label     Name to use for this test class.
 * @param file      Name of the file in which this class is used.
 * @param line      Line number where this class is defined.
 * @param isEnabled The test will be run if true, or skipped if false. */
UnitTest::UnitTest(const std::string& label, const std::string& file, int line, bool isEnabled) :
	m_label(label),
	m_file(file),
	m_line(line),
	m_isEnabled(isEnabled) {
		std::vector<UnitTest*>& list = get();
		list.push_back(this);
}

/* Unit test destructor. */
UnitTest::~UnitTest() {/* Nothing to do */}

/* Access the list which contains all tests */
std::vector<UnitTest*>& UnitTest::get() {
	static std::vector<UnitTest*> list;
	return list;
}

/* Used to compare the order of two UnitTest classes when sorting. */
bool UnitTest::sortMethod(UnitTest* lhs, UnitTest* rhs) {
	if (lhs && rhs) {
		if (lhs->file() != rhs->file()) return lhs->file() < rhs->file();
		if (lhs->line() != rhs->line()) return lhs->line() < rhs->line();
		if (lhs->label() != rhs->label()) return lhs->label() < rhs->label();
	}
	return lhs < rhs;
}

/* Generate an error string. */
std::string UnitTest::errorString(const std::string& message) const {
	std::stringstream ss;

	// Get shorter filename
	size_t i = m_file.find("/src/");
	if (i == std::string::npos) i = m_file.find("\\src\\");
	if (i == std::string::npos) i = m_file.find("/src\\");
	if (i == std::string::npos) i = m_file.find("\\src/");
	std::string filename = (i == std::string::npos) ? m_file : m_file.substr(i + 1);

	// Form output and return
	ss << filename << "[" << m_line << "]: " << m_label << std::endl << message << std::endl;
	return ss.str();
}

/* Add the error message from an exception to an output stream.
 * If the exception is nested then add all nested exceptions to the output stream. */
void UnitTest::exceptionToStream(const std::exception& error, std::stringstream& stream) {

	// Add the message from the exception to the output
	stream << error.what() << std::endl;

	// Throw the next exception, if there is one
	try {
		std::rethrow_if_nested(error);
	} catch(const std::exception& e) {
		exceptionToStream(e, stream);
	} catch(...) {
		stream << "Non-standard exception thrown" << std::endl;
	}
}
