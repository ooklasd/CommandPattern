#include "TestCMD.h"
#include "CommandManager.h"


//TestCMD.cpp�ļ�,��ʱArgTypeΪ����Json::Value
_XXX_CMD_REGISTER(TestCMD);
bool TestCMD::doit(const ArgType& arg)
{
	//�����߼�
	auto doc = getDocument();

	return true;
}