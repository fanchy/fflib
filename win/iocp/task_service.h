
#ifndef __TASK_SERVICE_H__
#define __TASK_SERVICE_H__

#include <Windows.h>

class task_service_t
{
public:
    task_service_t(int thread_num_ = 1);
    ~task_service_t();

    int  run();
    int  stop();

    void associate_device(HANDLE device, DWORD completion_key);
    void post();
private:
    HANDLE		m_iocport;
};
#endif
