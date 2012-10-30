#ifndef ELEKTRA_KDB_EXCEPT_HPP
#define ELEKTRA_KDB_EXCEPT_HPP

#ifndef USER_DEFINED_EXCEPTIONS

namespace kdb {

class Exception : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "Exception thrown by Elektra";
	}
};

class KeyException : public Exception
{
public:
	virtual const char* what() const throw()
	{
		return "Exception thrown by a Key";
	}
};

class KeyInvalidName : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "Invalid Keyname";
	}
};

class KeyMetaException : public KeyException
{
public:
	virtual const char* what() const throw()
	{
		return "Exception thrown by Key Meta Data related Operations";
	}
};

class KeyNoSuchMeta : public KeyMetaException
{
public:
	virtual const char* what() const throw()
	{
		return "No such meta data";
	}
};

class KeyBadMeta : public KeyMetaException
{
public:
	virtual const char* what() const throw()
	{
		return "Could not convert bad meta data";
	}
};

}

#endif

#endif
