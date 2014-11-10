#pragma once

#include <Poco/SharedPtr.h>

#include <DB/DataStreams/IProfilingBlockInputStream.h>
#include <DB/Columns/ColumnConst.h>



namespace DB
{


/** Добавляет в блок недостающие столбцы со значениями по-умолчанию.
  * Эти столбцы - материалированные (не константы).
  */
class AddingDefaultBlockInputStream : public IProfilingBlockInputStream
{
public:
	AddingDefaultBlockInputStream(
		BlockInputStreamPtr input_,
		NamesAndTypesListPtr required_columns_)
		: required_columns(required_columns_)
	{
		children.push_back(input_);
	}

	String getName() const override { return "AddingDefaultBlockInputStream"; }

	String getID() const override
	{
		std::stringstream res;
		res << "AddingDefault(" << children.back()->getID();

		for (NamesAndTypesList::const_iterator it = required_columns->begin(); it != required_columns->end(); ++it)
			res << ", " << it->name << ", " << it->type->getName();

		res << ")";
		return res.str();
	}

protected:
	Block readImpl() override
	{
		Block res = children.back()->read();
		if (!res)
			return res;
		res.addDefaults(required_columns);
		return res;
	}

private:
	NamesAndTypesListPtr required_columns;
};

}
