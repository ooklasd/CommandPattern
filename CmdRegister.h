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
            TCommandMgr::Register(Tcmd::CommandName(), std::make_pair(&Tcmd::NewClass,&Tcmd::DeleteClass ));
		}
	};
}