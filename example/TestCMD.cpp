#include "TestCMD.h"
#include "CommandManager.h"


//TestCMD.cpp文件,此时ArgType为上面Json::Value
_XXX_CMD_REGISTER(TestCMD);
bool TestCMD::doit(const ArgType& arg)
{
	//处理逻辑
	auto doc = getDocument();

	return true;
}