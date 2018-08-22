#pragma once
#include <list>
#include <stack>
#include <map>
#include "ICommand.h"
#include <mutex>
#include "cmdexception.h"

typedef void* (*NewClassFunc)();

typedef void(*DeleteClassFunc)(void*);

typedef void(*ErrorCallBack)(void*);

namespace cmd
{
	template<typename TCmd>
	class CommandMgr
	{
	public:
		typedef typename TCmd::ArgType ArgType;
		typedef typename TCmd::UserData UserData;
		typedef std::pair<NewClassFunc, DeleteClassFunc> NewDeleteFunc;

		CommandMgr(UserData* userData)
			:_userData(userData)
		{

		}

		//ִ��ĳ��������������
		virtual bool execute(const char* cmdName, const ArgType& arg, ArgType& result = ArgType())
		{
			_MgrTRYBEGIN_
				if (_executeBeforeCall != nullptr)
					_executeBeforeCall(arg);

			if (cmdName == nullptr)
				throw CmdException(ERROR_CODE::noCommand);

			//�ҵ�����
			NewDeleteFunc newdeleteFunc;
			{
				std::lock_guard<std::mutex> l(CmdListMutex());
				auto itf = CmdMap().find(cmdName);
				if (itf == CmdMap().end())
				{
					throw CmdException(ERROR_CODE::noCommand);
				}
				else
				{
					newdeleteFunc = itf->second;
				}
			}

			//�½�����
			NewClassFunc& newFunc = newdeleteFunc.first;
			DeleteClassFunc& deleteFunc = newdeleteFunc.second;
			TCmd* curCmd = (TCmd*)newFunc();
			if (curCmd == nullptr)
			{
				throw CmdException(ERROR_CODE::noCommand);
			}
			curCmd->setUserdata(_userData);

			bool isDone = curCmd->doit(arg);
			curCmd->getResult(result);

			if (!isDone)
			{
				//��û�����ָ���ɾ��ָ��
				deleteFunc(curCmd);
				curCmd = nullptr;
				return false;
			}

			if (isDone && curCmd->isUndoable())
			{
				//ִ�в��������
				_undoQueue.push_back(curCmd);
				if (_undoQueue.size() > _maxQueueLen)
				{
					_undoQueue.pop_front();
				}

				//�����������
				while (!_redoQueue.empty())
				{
					_redoQueue.pop();
				}
			}

			if (curCmd->isUndoable() == false)
			{
				deleteFunc(curCmd);
				curCmd = nullptr;
			}
			//���ؽ��
			if(_executeAfterCall != nullptr)
			_executeAfterCall(result);
			return isDone;

			_MgrTRYEND_

			return false;
		}
			
		//��������
		virtual void undo(size_t Step = 1)
		{
			_MgrTRYBEGIN_
				if (_undoQueue.empty())
					throw CmdException(ERROR_CODE::noUndo);

				while (Step > 0 && _undoQueue.empty() == false)
				{
					auto curCmd = _undoQueue.back();
					curCmd->undo();
					_undoQueue.pop_back();
					_redoQueue.push(curCmd);
					--Step;
				}
			_MgrTRYEND_
		}

		//��������
		virtual void redo(size_t Step = 1)
		{
			_MgrTRYBEGIN_
				if (_redoQueue.empty())
					throw CmdException(ERROR_CODE::noRedo);

					while (Step > 0 && _redoQueue.empty() == false)
					{
						auto curCmd = _redoQueue.top();
						curCmd->redo();
						_redoQueue.pop();
						_undoQueue.push_back(curCmd);
						--Step;
					}
			_MgrTRYEND_
		}

		static bool Register(const char* cmdName, NewDeleteFunc newfuncs)
		{
			if (cmdName == nullptr || newfuncs.first == nullptr || newfuncs.second == nullptr)
				return false;
			std::lock_guard<std::mutex> l(CmdListMutex());
			CmdMap()[cmdName] = std::move(newfuncs);
			return true;
		}

		//��ȡ/����ִ�д�����
		const ErrorCallBack& getErrorCallFunc() const { return _errorCallFunc; }
		virtual void setErrorCallFunc(ErrorCallBack val) { _errorCallFunc = std::move(val); }
		
		//��ȡ/���ó��ض�����󳤶�
		const size_t& getMaxQueueLen() const { return _maxQueueLen; }
		virtual void setMaxQueueLen(size_t val) { _maxQueueLen = std::move(val); }
	

		const unsigned& getErrorCode() const { return _errorCode; }
		const std::string& getErrorMessage() const { return _errorMessage; }

		const std::function<void(const ArgType &)>& getExecuteBeforeCall() const { return _executeBeforeCall; }
		virtual void setExecuteBeforeCall(std::function<void(const ArgType &)> val) { _executeBeforeCall = std::move(val); }
		
		const std::function<void(const ArgType &)>& getExecuteAfterCall() const { return _executeAfterCall; }
		virtual void setExecuteAfterCall(std::function<void(const ArgType &)> val) { _executeAfterCall = std::move(val); }
	protected:
		virtual void setErrorCode(unsigned val) { _errorCode = std::move(val); }
		virtual void setErrorMessage(std::string val) { _errorMessage = std::move(val); }
		
		typedef std::map<std::string, NewDeleteFunc> CommandMap;

		static CommandMap& CmdMap() {
			static CommandMap g_cmdMap;
			return g_cmdMap;
		}
		static std::mutex& CmdListMutex() { 
			static std::mutex g_cmdListMutex;
			return g_cmdListMutex; 
		}

		std::stack<TCmd*> _redoQueue;
		std::list<TCmd*> _undoQueue;

		size_t _maxQueueLen = 1000;//redo��undo�������еĺ�
		ErrorCallBack _errorCallFunc = nullptr;//��������ʱ���

		unsigned _errorCode;
		std::string _errorMessage;

		UserData* const _userData = nullptr;

		//ִ������ǰ
		std::function<void(const ArgType&)> _executeBeforeCall;

		//ִ�������
		std::function<void(const ArgType&)> _executeAfterCall;
	};
}