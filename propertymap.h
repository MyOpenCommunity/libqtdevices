/**
 * \file
 * <!--
 * Copyright 2005,2006 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Generic attribute map (implementation).
 *
 * \author Giovanni Bajo <rasky@develer.com>
 * \author Bernardo Innocenti <bernie@develer.com>
 */
#ifndef DEVLIB_PROPERTYMAP_H
#define DEVLIB_PROPERTYMAP_H

/// Configure PropertyMap::get<>(key) to throw exceptions
#define CONFIG_PROPERTYMAP_FAILURE PROPERTYMAP_FAILURE_THROW


#include <string>
#include <sstream>
#include <vector>

// For error policies
#include <cassert>
#include <iostream>
#include <stdexcept>


#ifdef __GNUC__
	#include <ext/hash_map>
	#define PROPERTYMAP_CONTAINER __gnu_cxx::hash_map<std::string, std::string, hash_string>

	/**
	 * Hash function for std::string objects
	 *
	 * \note Inherits from hash<const char *> to take advantage of EBCO.
	 */
	struct hash_string : private __gnu_cxx::hash<const char *>
	{
		size_t operator()(std::string s) const
		{
			return __gnu_cxx::hash<const char *>::operator()(s.c_str());
		}
	};
#else
	#include <map>
	#define PROPERTYMAP_CONTAINER std::map<std::string, std::string>
#endif

#ifndef ATTRIBUTE_NORETURN
	#ifdef __GNUC__
		#define ATTRIBUTE_NORETURN  __attribute__((noreturn))
	#else
		#define ATTRIBUTE_NORETURN  /* nothing */
	#endif
#endif


/**
 * Generic class to handle attribute sets.
 *
 * An attribute set is a map of strings with values stored
 * as strings.  This is useful to pass around application
 * settings fetched from configuration files.
 *
 * PropertyMap provides several ways to lookup values associated
 * with keys and convert these values from raw strings to specific
 * data types such as int or float.
 *
 * The easiest way to retrieve a value is by calling the get() template
 * method with a default value:
 *
 * \code
 *   int value = pm.get("answer", 42);
 * \endcode
 *
 * The value of key "answer" will be parsed and converted to an integer
 * and returned.  If no such key exists, get() returns the default value 42.
 *
 * The template function infers its return type from the default argument,
 * so if you need an unsigned long, you should specify the default as an
 * unsigned long too:
 *
 * \code
 *   unsigned long value = pm.get("answer", 42UL);
 * \endcode
 *
 * There's also an overload of the get() template without a default argument.
 * Because the template cannot infer the return type from its arguments, you
 * must specify it explicitly:
 *
 * \code
 *   int value = pm.get<int>("answer");
 * \endcode
 *
 * If the key "answer" does not exist, this variant of get() will fail by
 * either throwing an exception or by calling assert().
 *
 * To check for existance of a particular key, you can use the has() method:
 *
 * \code
 *   if (pm.has("answer"))
 *       int value = pm.get<int>("answer");
 * \endcode
 */
class PropertyMap
{
// Public definitions
public:
	/**
	 * Interface for policy classes to handle errors such
	 * as missing or invalid parameters.
	 */
	class ErrorPolicy
	{
	public:
		virtual void error(const std::string &reason) = 0;
		virtual ~ErrorPolicy() {}
	};

	/// ErrorPolicy to ignore all errors
	class NullErrorPolicy : public ErrorPolicy
	{
		virtual void error(const std::string & /*reason*/)
		{
			// nop
		}
	};

	/// Generate an assertion failure for any error
	class AssertErrorPolicy : public ErrorPolicy
	{
		virtual void error(const std::string &reason)
		{
			// FIXME: we can't report client information in the assertion
			std::cerr << reason << std::endl;
			assert(false);
		}
	};

	/// Throw a std::runtime_error exception for any error
	class ExceptionErrorPolicy : public ErrorPolicy
	{
		virtual void ATTRIBUTE_NORETURN error(const std::string &reason)
		{
			throw std::runtime_error(reason);
		}
	};


// Protected data members
protected:
	typedef PROPERTYMAP_CONTAINER DataMap;
	DataMap data;

	ErrorPolicy *error_policy;

// Construction
public:
	/**
	 * Create a new PropertyMap object.
	 *
	 * The default error reporting strategy is set to
	 * ExceptionErrorPolicy.  This can be overridden by
	 * passing a different policy object or by calling
	 * PropertyMap::setErrorPolicy().
	 */
	PropertyMap(ErrorPolicy *policy = NULL)
	{
		setErrorPolicy(policy);
	}

	// vdtor
	virtual ~PropertyMap() { /* nop */ }

private:
	/// Convert anything to a string (copied over from stlutil.h)
	template<typename T>
	static inline std::string toString(const T &x)
	{
		std::ostringstream ss;
		ss << x;
		return ss.str();
	}

	/**
	 * Convert \a value from string to type \a T
	 *
	 * In case of conversion errors, convert() complains using the active
	 * error policy.
	 *
	 * \param key    Name of the key, used only to report errors.
	 * \param value  String containing the value to be converted.
	 *
	 * \return The value converted to type \a T.
	 */ 
	template <typename T>
	T convert(const std::string &key, const std::string &value) const
	{
		std::istringstream iss(value);
		T ret;

		// Clear base to enable automatic parsing of numeric bases (hex, oct, dec, etc.)
		iss.setf(static_cast<std::ios_base::fmtflags>(0), std::ios_base::basefield);

		iss >> ret;

		if (iss.fail())
			error_policy->error("PropertyMap: key '" + key + "' has invalid value");

		if (!iss.eof())
			error_policy->error("PropertyMap: key '" + key + "' contains garbage after valid value");

		return ret;
	}

// overridables
public:

	/// Set a value \a value for the key \a key.
	virtual void store(const std::string &key, const std::string &val)
	{
		data[key] = val;
	}

	/**
	 * Copy the value of \a key in the variable \a value.
	 *
	 * \note When the lookup fails, the value of the parameter is left
	 *       unchanged.
	 *
	 * \return false if \a key doesn't exist, otherwise true.
	 */
	virtual bool retrieve(const std::string &key, std::string &value) const
	{
		DataMap::const_iterator iter = data.find(key);
		if (iter == data.end())
			return false;

		value = iter->second;
		return true;
	}

// Public operations
public:
	/// Set \a key to \a value.
	template <class T>
	void set(const std::string &key, const T &value)
	{
		store(key, toString(value));
	}

	/// Check whether \a key exists.
	bool has(const std::string &key) const
	{
		/// \todo Optimize away dummy copy
		std::string dummy;
		return retrieve(key, dummy);
	}

	/**
	 * Retrieve value of \a key.
	 *
	 * \note This overload is both an optimization and needed
	 *       for correctness in case of strings containing spaces.
	 */
	std::string get(const std::string &key) const
	{
		std::string val;
		bool found;

		if (!(found = retrieve(key, val)))
			error_policy->error("PropertyMap: key '" + key + "' missing");

		return val;
	}

	/**
	 * Retrieve value of \a key converted to type \a T.
	 *
	 * \see convert()
	 */
	template <typename T>
	T get(const std::string &key) const
	{
		return convert<T>(key, get(key));
	}

	/// Retrieve value of \a key, or \a def if not found.
	template <typename T>
	T get(const std::string &key, const T &def) const
	{
		std::string val;

		if (retrieve(key, val))
			return convert<T>(key, val);

		return def;
	}

	/**
	 * Overload of get(key, def) for std::string.
	 *
	 * \see get(const std::string &key) const
	 */
	std::string get(const std::string &key, const std::string &def) const
	{
		std::string val = def;
		retrieve(key, val);
		return val;
	}

	/// Overload of get(key, def) for naked C strings
	std::string get(const std::string &key, const char *def) const
	{
		return get(key, std::string(def));
	}

	/**
	 * Overload of get(key, def) for naked C strings.
	 *
	 * \note Without this non-const char * variant, overload
	 *       resolution would select the template function
	 *       instead of converting to const char *.
	 */
	std::string get(const std::string &key, char *def) const
	{
		return get(key, std::string(def));
	}

	/**
	 * Get a list of all the keys that are present in the map.
	 */
	std::vector<std::string> keys() const
	{
		std::vector<std::string> k;
		keys(k);
		return k;
	}

	/**
	 * Get a list of all the keys that are present in the map.
	 *
	 * \note \a result is not cleared: keys are appended to the vector.
	 */
	void keys(std::vector<std::string> &result) const
	{
		for (DataMap::const_iterator it = data.begin(); it != data.end(); ++it)
			result.push_back(it->first);
	}

	void erase(const std::string &key)
	{
		data.erase(key);
	}

	void clear(void)
	{
		data.clear();
	}

	/**
	 * Change the current ErrorPolicy.
	 *
	 * Passing NULL restores the default policy (ExceptionErrorPolicy).
	 *
	 * \note The PropertyMap does *not* assume ownership of the object
	 *       passed in.  It is the caller's responsability to deallocate
	 *       it when it's no longer needed.
	 */
	void setErrorPolicy(ErrorPolicy *new_policy)
	{
		if (!(error_policy = new_policy))
		{
			static ExceptionErrorPolicy default_error_policy;
			error_policy = &default_error_policy;
		}
	}
};


/**
 * Specialization of get(key) for boolean properties.
 *
 * Automatic conversion of strings with boolean meaning into
 * a bool value.
 *
 * \return true  if key has value "true", "1", "on", "yes"
 * \return false if key has value "false", "0", "off", "no"
 *
 * \note Declared at namespace scope to workaround a GCC 3.2 bug
 *       with member template specialization
 *
 * \todo Maybe add uppercase variants or make it fully case insensitive
 *
 * \see get(key) const
 */
template <>
inline bool PropertyMap::convert<bool>(const std::string &key, const std::string &value) const
{
	if (     value == "true"  || value == "1" || value == "on"  || value == "yes" || value == "y")
		return true;
	else if (value == "false" || value == "0" || value == "off" || value == "no"  || value == "n")
		return false;

	error_policy->error("PropertyMap: key '" + key + "' has not a valid boolean value");
	return false;
}

#endif // DEVLIB_PROPERTYMAP_H
