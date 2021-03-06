﻿#pragma once
#include <list>
#include <stack>
#include <map>
#include "ICommand.h"
#include <mutex>
#include "cmdexception.h"
#include <functional>

//typedef void(*NewClassFunc)(void*);
//typedef void(*DeleteClassFunc)(void*);
//typedef void(*ErrorCallBack)(void*);

template<typename T>
using NewClassFunc =  T* (*)();

template<typename T>
using DeleteClassFunc = void (*)(T *);

using ErrorCallBack = void(*)(void*);
// #define COMMAND_MANAGER_LIST_LOCK
namespace cmd
{
	template<typename TTCmd>
	class CommandMgr
	{
	public:
        using TCmd = TTCmd;
		typedef typename TCmd::ArgType ArgType;
		typedef typename TCmd::UserData UserData;
        using ICommand = ICommand<ArgType, UserData>;
		typedef std::pair<NewClassFunc<ICommand>, DeleteClassFunc<ICommand>> NewDeleteFunc;

		using ErrorCallBack = std::function<void(CommandMgr<TTCmd>*)>;

		CommandMgr(UserData* userData)
			:_userData(userData)
		{

		}

		//执行某个命令，并传入参数
		virtual bool execute(const char* cmdName, const ArgType& arg, ArgType& result) {
			return execute(cmdName,arg,result,nullptr);
		}
		virtual bool execute(const char* cmdName, const ArgType& arg, ArgType& result, std::function<bool()> cancelCall)
		{
			_MgrTRYBEGIN_
				if (_executeBeforeCall != nullptr)
					_executeBeforeCall(arg);

			if (cmdName == nullptr)
				throw CmdException(ERROR_CODE::noCommand);

			//找到命令
			NewDeleteFunc newdeleteFunc;
			{
#ifdef COMMAND_MANAGER_LIST_LOCK
				std::lock_guard<std::mutex> l(CmdListMutex());
#endif // COMMAND_MANAGER_LIST_LOCK
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

			//新建命令
			auto& newFunc = newdeleteFunc.first;
            auto& deleteFunc = newdeleteFunc.second;
            ICommand* curCmd = newFunc();
			if (curCmd == nullptr)
			{
				throw CmdException(ERROR_CODE::noCommand);
			}
			curCmd->setUserdata(_userData);

            bool isDone = false;
			try
            {
				curCmd->setIsCancelFunction(cancelCall);
                isDone = curCmd->doit(arg);
                curCmd->getResult(result);
				curCmd->setIsCancelFunction(nullptr);
			}
            catch (...)
            {
				curCmd->setIsCancelFunction(nullptr);
                if (curCmd->isUndoable())
                {
                    curCmd->undo();
                }
                throw;
            }

            if (!isDone)
            {
                //若没有完成指令，则删除指令
                deleteFunc(curCmd);
                curCmd = nullptr;
                return false;
            }

            else if (curCmd->isUndoable())
            {
                //执行并进入队列
                _undoQueue.push_back(curCmd);
                if (_undoQueue.size() > _maxQueueLen)
                {
                    _undoQueue.pop_front();
                }

                //清空重做队列
                while (!_redoQueue.empty())
                {
                    _redoQueue.pop();
                }
            }
            else
			{
				deleteFunc(curCmd);
				curCmd = nullptr;
			}

			//返回结果
			if(_executeAfterCall != nullptr)
			_executeAfterCall(result);
			return isDone;

			_MgrTRYEND_

			return false;
		}
			
		//撤回命令
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

		//重做命令
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

		//获取/设置执行错误函数
		const ErrorCallBack& getErrorCallFunc() const { return _errorCallFunc; }
		virtual void setErrorCallFunc(ErrorCallBack val) { _errorCallFunc = std::move(val); }
		
		//获取/设置撤回队列最大长度
		const size_t& getMaxQueueLen() const { return _maxQueueLen; }
		virtual void setMaxQueueLen(size_t val) { _maxQueueLen = std::move(val); }
	

		const unsigned& getErrorCode() const { return _errorCode; }
		const std::string& getErrorMessage() const { return _errorMessage; }

		const std::function<void(const ArgType &)>& getExecuteBeforeCall() const { return _executeBeforeCall; }
		virtual void setExecuteBeforeCall(std::function<void(const ArgType &)> val) { _executeBeforeCall = std::move(val); }
		
		const std::function<void(const ArgType &)>& getExecuteAfterCall() const { return _executeAfterCall; }
		virtual void setExecuteAfterCall(std::function<void(const ArgType &)> val) { _executeAfterCall = std::move(val); }
        const std::stack<ICommand *>& getRedoQueue() const { return _redoQueue; }
        const std::list<ICommand *>& getUndoQueue() const { return _undoQueue; }

        std::unique_lock<std::recursive_mutex> lock() {
            return std::unique_lock<std::recursive_mutex>(_mutex);
        }
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

        std::recursive_mutex _mutex;
		std::stack<ICommand*> _redoQueue;
		std::list<ICommand*> _undoQueue;

		size_t _maxQueueLen = 1000;//redo和undo两个队列的和
		ErrorCallBack _errorCallFunc = nullptr;//发生错误时候的

		unsigned _errorCode;
		std::string _errorMessage;

		UserData* const _userData = nullptr;

		//执行命令前
		std::function<void(const ArgType&)> _executeBeforeCall;

		//执行命令后
		std::function<void(const ArgType&)> _executeAfterCall;
	};
}