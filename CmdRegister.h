#pragma once
#include "CommandMgr.h"
namespace cmd
{
	template<typename TCommandMgr,typename Tcmd>
	class CmdRegister
	{
	public:
		CmdRegister()
		{
			TCommandMgr::Register(Tcmd::CommandName(), { (NewClassFunc)&Tcmd::NewClass,(DeleteClassFunc)&Tcmd::DeleteClass});
		}
	};
}