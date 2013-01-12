#include <assert.h>

#include "task_service.h"

task_service_t::task_service_t(int thread_num_)
{
    m_iocport = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, thread_num_);
    assert(m_iocport && "CreateIoCompletionPort failed!!!!");
}

task_service_t::~task_service_t()
{
    if (m_iocport != NULL)
    {
        ::CloseHandle(m_iocport);
        m_iocport = NULL;
    }
}

int task_service_t::run()
{
    return 0;
}

int task_service_t::stop()
{
    return 0;
}

void task_service_t::associate_device(HANDLE device, DWORD completion_key)
{
    ::CreateIoCompletionPort(device, m_iocport, completion_key, 0);
}

