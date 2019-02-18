#pragma once
#pragma once
#include "osg/ref_ptr"
#include <map>
#include <mutex>
#include <functional>
namespace cmd
{
    template<typename TTCmd>
    class CMDNotify
    {
    public:
        using NotifyFunc = std::function<void(const std::string&, const TTCmd*)>;
        using NotifyList = std::list<NotifyFunc>;

        CMDNotify() {}
        virtual ~CMDNotify() {}

        bool addNotifyGlobal(NotifyFunc callback)
        {
            if (callback == nullptr) return false;
            _callBackGlobal.emplace_back(std::move(callback));
            return true;
        }

        bool setOrAddNotify(const std::string& actionName, NotifyFunc callback)
        {
            if (callback == nullptr) return false;
            auto l = lock();
            _callBackMap[actionName].emplace_back(std::move(callback));
            return true;
        }

        void clearNotify(const std::string& actionName)
        {
            auto l = lock();
            _callBackMap[actionName].clear();
        }

        //通知全局
        virtual bool notifyGlobal(const TTCmd* pcmd) const
        {
            for (auto& func : _callBackGlobal)
            {
                if (func == nullptr) continue;
                func(actionName, pcmd);
            }
        }

        //通知全局
        virtual bool notify(const std::string& actionName, const TTCmd* pcmd) const
        {
            auto l = lock();
            if (auto callback = getNotifyList(actionName))
            {
                for (auto& func : *callback)
                {
                    if (func == nullptr) continue;
                    func(actionName, pcmd);
                }
            }

            notifyGlobal();

            return true;
        }

    protected:
        const NotifyList* getNotifyList(const std::string& actionName) const
        {
            auto l = lock();
            auto itf = _callBackMap.find(actionName);
            return itf == _callBackMap.end() ? nullptr : &(itf->second);
        }

        auto lock() const
        {
            return std::unique_lock<std::recursive_mutex>((_callBackMapMutex));
        } 

    private:
        mutable std::recursive_mutex _callBackMapMutex;
        std::map<std::string, NotifyList> _callBackMap;
        NotifyList _callBackGlobal;
    };
}