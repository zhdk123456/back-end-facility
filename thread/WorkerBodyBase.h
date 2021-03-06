#ifndef _WORKER_BODYBASE_H_
#define _WORKER_BODYBASE_H_

#include "ITask.h"
#include "sys/Defs.h"
#include "misc/NonCopyable.h"
#include "misc/SpinlockQueue.h"

#include <semaphore.h>

class NotifyerBase
{
    public:
        virtual int Notify(int type = 0) = 0;
};

class WorkerBodyBase: public ITask, public noncopyable
{
    public:
        explicit WorkerBodyBase(NotifyerBase* notifyer = NULL);
        virtual ~WorkerBodyBase();

        bool PostTask(ITask*);
        void ClearAllTask();

        int GetTaskNumber() const;
        virtual int GetContainerSize() const = 0;

        virtual bool StopRunning();

        bool IsRunning() const;
        int  TaskDone() const { return done_; }
        void EnableNotify(bool enable = true) { notify_ = enable; }

        // take care of calling this function.
        // multitasking-opertion on mailbox_ will
        // greatly reduce performance.
        // don't call it unless really necessary.
        ITask* TryGetTask();

    protected:
        virtual void PrepareWorker() {}
        virtual bool HandleTask(ITask*) = 0;
        virtual bool HasTask() = 0;
        virtual void CleanRunTask(ITask*);

        // be aware: this function may block if there is no task
        virtual ITask* GetTaskFromContainer() = 0;
        virtual bool   PushTaskToContainer(ITask*) = 0;

        inline void SignalPost();
        inline void SignalConsume();
        inline bool SignalConsumeTimeout(int);
        inline bool TryConsume();
        inline int  Notify();
        inline bool PostExit();

    private:
        void Run();
        int NotifyDone();

        // get task from mailbox or cmd_
        // may block when there is no task.
        // caller take responsibility to free the task.
        // return false if get task from cmd_.
        bool GetRunTask(ITask*& task);

        // set up flag to make worker loop exit;
        // will be called in worker thread only.
        inline void SetExitState();

        inline bool PostInternalCmd(ITask*);
        inline bool PushInternalCmd(ITask*);
        inline ITask* GetInternalCmd();
        inline void ProcessInternalCmd(ITask*);

        // is running task.
        mutable volatile int isRuning_;

        // timeout for sem_timewait
        const int timeout_;

        const int reqThreshold_;

        // semaphore for waiting for task.
        sem_t sem_;

        // task processed.
        volatile int done_;

        // notify thread pool I am free now, send me some tasks
        volatile bool notify_;
        NotifyerBase* notifyer_;

        // note this is for internal usage only.
        // supporting worker internal activities: exit.
        SpinlockQueue<ITask*> cmd_;

        // exit flag.
        // this variable will be accessed in the worker thread.
        // It will be changed by DummyExitTask only.
        bool ShouldStop_;

        friend class DummyExitTask;
};

class WorkerManagerBase
{
    public:
        WorkerManagerBase() {}
        virtual ~WorkerManagerBase() {}

    protected:
        virtual int SetWorkerNotify(NotifyerBase*, int type = 0) = 0;

        friend class Worker;
};

#endif

