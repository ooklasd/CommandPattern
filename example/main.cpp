#include "Document.h"

int main()
{
	//应该放到某个地方
	Document doc;

	//在某个逻辑中执行
	auto cmd = doc.getCommandManager();
	CommandArg arg,returnData;
	arg["file"] = "XXX";
	return !cmd->execute("TestCMD", arg, returnData);

}