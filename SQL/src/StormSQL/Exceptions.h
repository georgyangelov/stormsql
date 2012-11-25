#ifndef _H_EXCEPTIONS_INCLUDED
#define _H_EXCEPTIONS_INCLUDED

#include <string>
#include <stdexcept>

using namespace std;

namespace StormSQL
{
	class Exception
		: public runtime_error
	{
	public:
		Exception(const string& message)
			: runtime_error(message)
		{
		}
	};

	class TableSchemaOldVersion
		: public Exception
	{
	public:
		TableSchemaOldVersion()
			: Exception("Your table schema is created using an older version of StormSQL")
		{
		}
	};

	class FieldExists
		: public Exception
	{
	public:
		FieldExists()
			: Exception("This field already exists")
		{
		}
	};

	class FieldDoesntExists
		: public Exception
	{
	public:
		FieldDoesntExists()
			: Exception("This field doesn't exist")
		{
		}
	};

	class InvalidFieldType
		: public Exception
	{
	public:
		InvalidFieldType()
			: Exception("Invalid field type")
		{
		}
	};
}
#endif

