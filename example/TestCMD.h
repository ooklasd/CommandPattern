#pragma once
#include "Command.h"
#include "CommandPattern/ICommand.h"

//TestCMD.hͷ�ļ�
class  TestCMD :public Command
{
public:
	CommandMeta(TestCMD);
	virtual bool doit(const ArgType& arg) override;
};
