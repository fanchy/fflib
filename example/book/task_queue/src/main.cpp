#include <iostream>
#include <unistd.h>
using namespace std;

#include "base/task_queue_impl.h"
#include "base/thread.h"
#include "net/epoll_impl.h"

using namespace ff;

class foo_t
{
public:
	void print(int data)
	{
		cout << "helloworld, data:" <<data << " thread id:"<< ::pthread_self() << endl;
	}
	void print_callback(int data, void (*callback_)(int))
	{
		callback_(data);
	}
	static void check(int data)
	{
		cout << "helloworld, data:" <<data << " thread id:"<< ::pthread_self() << endl;
	}
};

//  ���̵߳��������
void test_1()
{
	thread_t thread;
	task_queue_t tq;

	thread.create_thread(task_binder_t::gen(&task_queue_t::run, &tq), 1);

	foo_t foo;
	for (int i = 0; i < 100; ++i)
	{
		cout << "helloworld, thread id:"<< ::pthread_self() << endl;
		tq.produce(task_binder_t::gen(&foo_t::print, &foo, i));
		sleep(1);
	}
	thread.join();
}

//! ���̶߳��������
void test_2()
{
	thread_t thread;
	task_queue_t tq[3];

	for (unsigned int i = 0; i < sizeof(tq)/sizeof(task_queue_t); ++i)
	{
		thread.create_thread(task_binder_t::gen(&task_queue_t::run, &(tq[i])), 1);
	}

	foo_t foo;
	cout << "helloworld, thread id:"<< ::pthread_self() << endl;
	for (unsigned int j = 0; j < 100; ++j)
	{
		tq[j % (sizeof(tq)/sizeof(task_queue_t))].produce(task_binder_t::gen(&foo_t::print, &foo, j));
		sleep(1);
	}
	thread.join();
}

//! ���̵߳��������
void test_3()
{
	thread_t thread;
	task_queue_t tq;

	thread.create_thread(task_binder_t::gen(&task_queue_t::run, &tq), 3);

	foo_t foo;
	cout << "helloworld, thread id:"<< ::pthread_self() << endl;
	for (unsigned int j = 0; j < 100; ++j)
	{
		tq.produce(task_binder_t::gen(&foo_t::print, &foo, j));
		sleep(1);
	}
	thread.join();
}

//! �첽�ص�
void test_4()
{
	thread_t thread;
	task_queue_t tq;

	thread.create_thread(task_binder_t::gen(&task_queue_t::run, &tq), 1);

	foo_t foo;
	cout << "helloworld, thread id:"<< ::pthread_self() << endl;
	for (unsigned int j = 0; j < 100; ++j)
	{
		tq.produce(task_binder_t::gen(&foo_t::print_callback, &foo, j, &foo_t::check));
		sleep(1);
	}
	thread.join();
}

// �ӿ�
/*
void socket_impl_t::async_send(const string& msg_)
{
	tq.produce(task_binder_t::gen(&socket_impl_t::send, &this, msg_));
}
void socket_impl_t::send(const string& msg_)
{
	//do send code
}
*/
int main(int argc, char* argv[])
{
	thread_t thread;
	epoll_impl_t ep;

	thread.create_thread(task_binder_t::gen(&epoll_impl_t::event_loop, &ep), 1);

	do
	{
		sleep(1);
		ep.interupt_loop();
	}while(1);
	thread.join();
	//test_1();
	//test_2();
	//test_3();
	test_4();
    return 0;
}
