#pragma once

#include "CommandPattern/ICommand.h"
#include "CommandPattern/CmdRegister.h"
#include <unordered_map>

using CommandArg = std::unordered_map<std::string, std::string>;
class Document;
class Command :public cmd::ICommand<CommandArg, Document>
{
public:
	Document* getDocument()const { return this->getUserdata(); }

	virtual void undo() override{}
	virtual void redo() override {}
	virtual bool isUndoable() override { return false; }
	virtual void getResult(ArgType& ret) override
	{
		ret = _reuslt;
	}
protected:
	ArgType _reuslt;
};