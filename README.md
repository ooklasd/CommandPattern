# CmdPattern

* 命令模式，只需要头文件便可以。
* 可以采用命名控件实现多个命令管理器，多套命令。
* 命令管理器是线程安全的。
* 允许自定义命令数据格式，如xml、json、string 等等。
* 允许自定义操作目标，如 Document、Project等等

本人在使用过程中实现多个命令管理器，实现异步同步的命令执行器，这个都会使得命令模式更加便捷(不开放，请自行实现)

## 使用方法：

### 1、建立自定义命令,作为其他命令的基类，ICommand<命令数据，传递的指针>，可以采用其他的数据类型
```
#include "CommandPattern/ICommand.h"
#include "CommandPattern/CmdRegister.h"
#include "json/value.h"
class Document;
class Command :public cmd::ICommand<Json::Value, Document>
{
public:
    Document* getDocument()const{return this->getUserdata();}
}
```

### 2、建立自定义命令管理器，也就是命令的堆栈
```
#include "Command.h"
#include "CommandPattern/CommandMgr.h"
class CommandManager :public cmd::CommandMgr<Command>
{
public:
    using Inherit = cmd::CommandMgr<Command>;

    CommandManager(UserData* userData);
    //通常会建立自身的异常处理机制
    virtual bool execute(const char* cmdName, const ArgType& arg, ArgType& result = ArgType()) override;
};

//命令注册宏定义
#define _CMD_REGISTER(name) cmd::CmdRegister<CommandManager,name> __##name##CMDReg;
```


### 3、放入命令管理器到文档类
```
#include<CommandManager.h>

class Document
{
public:
    //构造时候将自己传入命令管理器，作为所有资源的入口
    Document(){_CommandManager = std::make_unique<CommandManager>(this);}
    
    //返回命令管理器，
    CommandManager* getCommandManager(){ return _CommandManager.get(); }
protect:
    //命令管理器，用于命令执行、撤回等功能
	std::unique_ptr<CommandManager> _CommandManager;
};

```

### 4、建立和注册命令

```
//TestCMD.h头文件
class  TestCMD :public Command
{
public:
    CommandMeta(TestCMD);
    virtual bool doit(const ArgType& arg) override;
};

//TestCMD.cpp文件,此时ArgType为上面Json::Value
__RTK_CMD_REGISTER(TestCMD);
bool TestCMD::doit(const ArgType& arg)
{
    //处理逻辑
    auto doc = getDocument();
    
    return true;
}

```

### 5、触发命令
```
//应该放到某个地方
Document doc;

//在某个逻辑中执行
auto cmd= doc.getCommandManager();
Json::Value arg;
arg["file"]= "XXX";
cmd->execute(“TestCMD”,arg);

```

## 真实例子（待补充）


