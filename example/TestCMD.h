#pragma once
#include "Command.h"
#include "CommandPattern/ICommand.h"

//TestCMD.hм╥нд╪Ч
class  TestCMD :public Command
{
public:
	CommandMeta(TestCMD);
	virtual bool doit(const ArgType& arg) override;
};
