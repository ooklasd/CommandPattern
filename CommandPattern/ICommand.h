#pragma once
#include <functional>
namespace cmd
{
	template<typename TArgType,typename TUserData = void>
	class ICommand
	{
	public:
		typedef TArgType ArgType;
		typedef TUserData UserData;

		virtual ~ICommand() {}

		virtual bool doit(const ArgType& arg) = 0;
		virtual void undo() = 0;
		virtual void redo() = 0;
		virtual bool isUndoable() = 0;
		virtual const char* className() const = 0;
		virtual void getResult(ArgType&) = 0;
		virtual bool isCancel() {return _isCancelFunction ? _isCancelFunction(): false;}

		/*static ICommand* NewClass() { return nullptr; }*/
		static void DeleteClass(ICommand* c) { delete c; }

		UserData* getUserdata()const{ return _userdata; }
		virtual void setUserdata(UserData* val) { _userdata = std::move(val); }

		std::function<bool()> getIsCancelFunction() const { return _isCancelFunction; }
		void setIsCancelFunction(std::function<bool()> val) { _isCancelFunction = std::move(val); }
	protected:
		UserData* _userdata = nullptr;
		std::function<bool()> _isCancelFunction;
	};

#define CommandMeta(cmdClass)\
public:\
	static cmd::ICommand<cmdClass::ArgType,cmdClass::UserData>* NewClass() { return new cmdClass(); }\
	static void DeleteClass(cmd::ICommand<cmdClass::ArgType,cmdClass::UserData>* ptr) { delete ptr; }\
	static const char* CommandName() { return #cmdClass; }\
	virtual const char* className()const { return CommandName(); }


	//template<typename TArgType, typename TUserData = void>
	//ICommand<TArgType, TUserData>* NewClass() { return nullptr; }

	//template<typename TArgType, typename TUserData = void>
	//void DeleteClass(ICommand<TArgType, TUserData>* c) { delete c; }

}
