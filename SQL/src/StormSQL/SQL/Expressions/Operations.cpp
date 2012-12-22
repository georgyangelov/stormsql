#include "Operations.h"

using namespace std;
using namespace StormSQL::SQL::Expressions;

hash_map<string, OperationInfo> IOperation::GetStandardOperations()
{
	hash_map<string, OperationInfo> ops;
	ops["and"] = OperationInfo(And(), 1, 2, true, false);
	ops["="] = OperationInfo(Equals(), 3, 2, true, false);
	ops["+"] = OperationInfo(Plus(), 4, 2, true, false);

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