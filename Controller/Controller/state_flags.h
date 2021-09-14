#ifndef STATE_FLAGS_H
#define STATE_FLAGS_H

// A small class used to make managing state flags a bit easier
class StateFlags {
public:

	// The data type used for the state flags
	typedef uint8_t Type;

	// Set some state values (turn them on)
	inline void set(Type bitFlags) __attribute__((always_inline)) {
		m_flags |= bitFlags;
	}

	// Reset some state values (turn them off)
	inline void reset(Type bitFlags) __attribute__((always_inline)) {
		m_flags &= ~bitFlags;
	}

	// Returns true if ALL designated state flags are set (turned on)
	inline bool isSet(Type bitFlags) __attribute__((always_inline)) {
		return (m_flags & bitFlags) == bitFlags;
	}

	// Returns true if ANY designated state flags are set (turned on)
	inline bool anySet(Type bitFlags) __attribute__((always_inline)) {
		return m_flags & bitFlags;
	}

	// Get the raw data
	inline Type& data() __attribute__((always_inline)) {return m_flags;}

	// Constructors
	StateFlags() : m_flags(0) {}
	StateFlags(Type bitFlags) : m_flags(bitFlags) {}

private:
	Type m_flags;
};

#endif
