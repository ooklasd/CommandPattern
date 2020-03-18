#pragma once
#include "enum\enum.h"
namespace cmd
{
	BETTER_ENUM(ERROR_CODE,int, unknown,Good, noCommand, noRedo, noUndo);
	

	class CmdException :public std::exception
	{
	public:
		CmdException(ERROR_CODE code) :std::exception(code._to_string()),_code(code) {}

		const cmd::ERROR_CODE& getCode() const { return _code; }
	protected:
		ERROR_CODE _code;
    };


    
#define _MgrTRYBEGIN_ auto lockMgr = lock();\
                        try{ 

#define _MgrTRYEND_ }\
	catch (CmdException& e)\
	{\
		setErrorMessage(e.what());\
		setErrorCode(e.getCode());\
			if (_errorCallFunc) _errorCallFunc(this);\
			else throw;\
	}\
	catch (std::exception& e)\
	{\
		setErrorCode(ERROR_CODE::unknown);\
		setErrorMessage(e.what());\
		if (_errorCallFunc) _errorCallFunc(this);\
		else throw;\
	}\
	catch (...)\
	{\
		setErrorCode(ERROR_CODE::unknown);\
		if (_errorCallFunc) _errorCallFunc(this);\
		else throw;\
	}
}