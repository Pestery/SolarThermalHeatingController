#ifndef UNIT_TEST_H
#define UNIT_TEST_H

/* Include headers */
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

/** A macro used to declare the start of a unit-testing section.
 *  The @p label is the name used to identify the test section.
 *  Usage:
 *    TEST(some_label_here) {
 *        some test code here
 *    }
 */
#define TEST(label) TEST_HELPER1(label, __COUNTER__, true)

/** A macro used to declare the start of a @b disabled unit-testing section.
 *  The @p label is the name used to identify the test section.
 *  Usage:
 *    TEST(some_label_here) {
 *        some test code here
 *    }
 */
#define TEST_DISABLED(label) TEST_HELPER1(label, __COUNTER__, false)


/** Make sure two values are equal.
 *  Throws a C++ exception on error. */
#define CHECK_EQUAL(lhs, rhs) { \
    try { \
        if (!((lhs) == (rhs))) throw std::runtime_error("Expected values to be equal"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure two values are equal.
 *  This variant shows the values if there was an error.
 *  Throws a C++ exception on error. */
#define CHECK_EQUAL_SHOW(lhs, rhs) { \
    try { \
        if (!((lhs) == (rhs))) { \
            std::stringstream ss; \
            ss << "Expected values to be equal" << std::endl; \
            ss << "lhs: " #lhs " is (" << (lhs) << ")" << std::endl; \
            ss << "rhs: " #rhs " is (" << (rhs) << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure two values are not-equal.
 *  Throws a C++ exception on error. */
#define CHECK_NOT_EQUAL(lhs, rhs) { \
    try { \
        if (!((lhs) != (rhs))) throw std::runtime_error("Expected values to be not-equal"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure two values are not-equal.
 *  This variant shows the values if there was an error.
 *  Throws a C++ exception on error. */
#define CHECK_NOT_EQUAL_SHOW(lhs, rhs) { \
    try { \
        if (!((lhs) != (rhs))) { \
            std::stringstream ss; \
            ss << "Expected values to be not-equal" << std::endl; \
            ss << "lhs: " #lhs " is (" << (rhs) << ")" << std::endl; \
            ss << "rhs: " #rhs " is (" << (rhs) << ")"; \
            throw std::runtime_error(ss.str()); \
        } \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure the boolean value is true.
 *  Throws a C++ exception on error. */
#define CHECK_IS_TRUE(boolean) { \
    try { \
        if (!(boolean)) throw std::runtime_error("Expected boolean to be true, but it is false"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure the boolean value is false.
 *  Throws a C++ exception on error. */
#define CHECK_IS_FALSE(boolean) { \
    try { \
        if (boolean) throw std::runtime_error("Expected boolean to be false, but it is true"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure the pointer value is null.
 *  Throws a C++ exception on error. */
#define CHECK_IS_NULL(boolean) { \
    try { \
        if ((pointer) != nullptr) throw std::runtime_error("Expected pointer to be null, but it is non-null"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure the pointer value is non-null.
 *  Throws a C++ exception on error. */
#define CHECK_NOT_NULL(pointer) { \
    try { \
        if ((pointer) == nullptr) throw std::runtime_error("Expected pointer to be non-null, but it is null"); \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/** Make sure the command throws a given error message.
 *  Throws a C++ exception on error. */
#define CHECK_THROW(command, errorMessage) { \
    try { \
        command; \
        throw UnitTest::ExceptionThrow(); \
    } catch(const UnitTest::ExceptionThrow& e) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } catch(const std::exception& e) { \
        if (strcmp(e.what(), (errorMessage))) { \
            std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__) + ", wrong exception message")); \
        } \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    } \
}


/* Check segment macros
 * These are used to define a check-segment within a larger test
 * If an error is thrown then a label is added to it which marks the line number */
#define CHECK_SEGMENT_BEGIN \
    try {
#define CHECK_SEGMENT_END \
    } catch (const std::exception& e) { \
        std::stringstream ss; \
        ss << "Exception in segment ending on line " << __LINE__ << std::endl << e.what(); \
        throw std::runtime_error(ss.str()); \
    }

/* Surround a statement with a try-catch block
 * Re-throws any thrown error with the line number added */
#define CHECK_TRY_CATCH(statement) \
    try { \
        statement; \
    } catch(...) { \
        std::throw_with_nested(std::runtime_error("On line " + std::to_string(__LINE__))); \
    }


/** A unit test framework class used with the test macros.
 *  Other classes inherit this class and are then defined as global variables.
 *  When they are created, they are added to a global list to be checked when runAllTests() is called. */
class UnitTest {
public:

    /** Run all test classes which have been defined. */
    static void runAllTests();

    /** Get the name of this test class. */
    const std::string& label() const {return m_label;}

    /** Get the file which contains this test class. */
    const std::string& file() const {return m_file;}

    /** Get the line number where this test class is defined. */
    int line() const {return m_line;}

    /** True if the test is enabled, or false if it should be skipped. */
    bool isEnabled() const {return m_isEnabled;}

    /** Unit test constructor.
     *  @param label     Name to use for this test class.
     *  @param file      Name of the file in which this class is used.
     *  @param line      Line number where this class is defined.
     *  @param isEnabled The test will be run if true, or skipped if false. */
    UnitTest(const std::string& label, const std::string& file, int line, bool isEnabled);

    /** Unit test destructor. */
    virtual ~UnitTest();

    /** An exception which is used within CHECK_THROW. */
    class ExceptionThrow : public std::exception {
    public:
        ExceptionThrow() = default;
        ExceptionThrow(ExceptionThrow&& rhs) noexcept = default;
        ExceptionThrow(const ExceptionThrow& rhs) = default;
        ExceptionThrow& operator = (ExceptionThrow&& rhs) noexcept = default;
        ExceptionThrow& operator = (const ExceptionThrow& rhs) noexcept = default;
        ~ExceptionThrow() = default;
        const char* what() const noexcept;
    };

protected:

    /** Run this unit test.
     *  This is called from within runAllTests().
     *  Throws a C++ exception on error */
    virtual void runTest() = 0;

private:

    /** Access the list which contains all tests */
    static std::vector<UnitTest*>& get();

    /** Used to compare the order of two UnitTest classes when sorting. */
    static bool sortMethod(UnitTest* lhs, UnitTest* rhs);

    /** Generate an error string. */
    std::string errorString(const std::string& message) const;

    /** Add the error message from an exception to an output stream.
     *  If the exception is nested then add all nested exceptions to the output stream. */
    static void exceptionToStream(const std::exception& error, std::stringstream& stream);

    /* Private data */
    std::string m_label;
    std::string m_file;
    int m_line;
    bool m_isEnabled;

};


/* Define some helper macros which assist the TEST macro */
#define TEST_HELPER1(label, counter, enabled) TEST_HELPER2(label, counter, enabled)
#define TEST_HELPER2(label, counter, enabled) \
    class UnitTest_##counter##_##label : UnitTest { \
    public: \
        UnitTest_##counter##_##label(const std::string& label, const std::string& file, int line, bool isEnabled) : UnitTest(label, file, line, isEnabled) {} \
        void runTest(); \
    }; \
    UnitTest_##counter##_##label UnitTest_##counter##_##label##_handle(#label, __FILE__, __LINE__, enabled); \
    void UnitTest_##counter##_##label::runTest()


#endif // UNIT_TEST_HPP
