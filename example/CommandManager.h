#pragma once

#include "Command.h"
#include "CommandPattern/CommandMgr.h"
class CommandManager :public cmd::CommandMgr<Command>
{
public:
	using Inherit = cmd::CommandMgr<Command>;

	CommandManager(UserData* userData):Inherit(userData){}
	//ͨ���Ὠ��������쳣�������
	virtual bool execute(const char* cmdName, const ArgType& arg, ArgType& result) override
	{
		return Inherit::execute(cmdName,arg,result);
	}
};

//����ע��궨��
#define _XXX_CMD_REGISTER(name) cmd::CmdRegister<CommandManager,name> __##name##CMDReg;