#pragma once

#include"CommandManager.h"

class Document
{
public:
	//����ʱ���Լ������������������Ϊ������Դ�����
	Document() { _CommandManager = std::make_unique<CommandManager>(this); }

	//���������������
	CommandManager* getCommandManager() { return _CommandManager.get(); }
protected:
	//�������������������ִ�С����صȹ���
	std::unique_ptr<CommandManager> _CommandManager;
};