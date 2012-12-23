#include "Operations.h"

using namespace std;
using namespace StormSQL::SQL::Expressions;

hash_map<string, OperationInfo> IOperation::GetStandardOperations()
{
	hash_map<string, OperationInfo> ops;
	
	/* 
	 * Operators 
	 */

	/* Logic */
	ops["or"] = OperationInfo(Or(), 0, 2, true, false);
	ops["and"] = OperationInfo(And(), 1, 2, true, false);

	/* Comparison */
	ops["<"] = OperationInfo(LowerThan(), 3, 2, true, false);
	ops["<="] = OperationInfo(LowerThanEqual(), 3, 2, true, false);
	ops[">"] = OperationInfo(GreaterThan(), 3, 2, true, false);
	ops[">="] = OperationInfo(GreaterThanEqual(), 3, 2, true, false);
	ops["="] = OperationInfo(Equals(), 3, 2, true, false);
	ops["!="] = OperationInfo(NotEqual(), 3, 2, true, false);

	/* Mathematical */
	ops["+"] = OperationInfo(Plus(), 4, 2, true, false); // Can also concatenate strings
	ops["-"] = OperationInfo(Minus(), 4, 2, true, false);
	ops["*"] = OperationInfo(Multiply(), 5, 2, true, false);
	ops["/"] = OperationInfo(Divide(), 5, 2, true, false);

	/*
	 * Functions
	 */

	/* Conversion */
	ops["toint"] = OperationInfo(ToInt(), 999, 1, false, true);
	ops["tostr"] = OperationInfo(ToStr(), 999, 1, false, true);
	ops["tobool"] = OperationInfo(ToBool(), 999, 1, false, true);

	/* Strings */
	ops["strcat"] = OperationInfo(StrCat(), 999, 2, false, true);
	ops["strlen"] = OperationInfo(StrLen(), 999, 1, false, true);
	ops["substr"] = OperationInfo(SubStr(), 999, 3, false, true);

	return ops;
}

void OperationInfo::copy(const OperationInfo& obj)
{
	if (obj.op)
		op = obj.op->Clone();
	else
		op = NULL;

	priority = obj.priority;
	arguments = obj.arguments;
	leftAssoc = obj.leftAssoc;
	isFunction = obj.isFunction;
}

OperationInfo::OperationInfo()
{
	op = NULL;
}
				
OperationInfo::OperationInfo(const IOperation& _op, int _priority, int _arguments, bool _leftAssoc, bool _isFunction)
	: op(_op.Clone()), priority(_priority), arguments(_arguments), 
	leftAssoc(_leftAssoc), isFunction(_isFunction)
{
}

OperationInfo::OperationInfo(const OperationInfo& obj)
{
	copy(obj);
}

OperationInfo::~OperationInfo()
{
	if (op)
		delete op;
}

OperationInfo& OperationInfo::operator = (const OperationInfo& obj)
{
	if (this != &obj)
	{
		if (op)
			delete op;
		copy(obj);
	}

	return *this;
}