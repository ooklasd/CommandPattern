#pragma once

#include"CommandManager.h"

class Document
{
public:
	//构造时候将自己传入命令管理器，作为所有资源的入口
	Document() { _CommandManager = std::make_unique<CommandManager>(this); }

	//返回命令管理器，
	CommandManager* getCommandManager() { return _CommandManager.get(); }
protected:
	//命令管理器，用于命令执行、撤回等功能
	std::unique_ptr<CommandManager> _CommandManager;
};