#include "Document.h"

int main()
{
	//Ӧ�÷ŵ�ĳ���ط�
	Document doc;

	//��ĳ���߼���ִ��
	auto cmd = doc.getCommandManager();
	CommandArg arg,returnData;
	arg["file"] = "XXX";
	return !cmd->execute("TestCMD", arg, returnData);

}