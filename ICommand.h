#pragma once
#include "export.h"
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
		virtual const char* className() = 0;
		virtual void getResult(ArgType&) = 0;

		/*static ICommand* NewClass() { return nullptr; }*/
		static void DeleteClass(ICommand* c) { delete c; }

		const UserData* getUserdata() const { return _userdata; }
		UserData* getUserdata(){ return _userdata; }
		virtual void setUserdata(UserData* val) { _userdata = std::move(val); }

	protected:
		UserData* _userdata = nullptr;
	};

#define CommandMeta(cmd)\
public:\
	static void* NewClass() { return new cmd(); }\
	static void DeleteClass(void* ptr) { delete (cmd*)ptr; }\
	static const char* CommandName() { return #cmd; }\
	virtual const char* className() { return CommandName(); }


	//template<typename TArgType, typename TUserData = void>
	//ICommand<TArgType, TUserData>* NewClass() { return nullptr; }

	//template<typename TArgType, typename TUserData = void>
	//void DeleteClass(ICommand<TArgType, TUserData>* c) { delete c; }

}
