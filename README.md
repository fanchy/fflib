# FFLib

FFLib is a lightweight c++ framework. I make codes clean as best as I can. KISS is my principle.I usually program 
with c++. I am a game server developer. So FFlib is mainly designed for game server developing. But some components
in FFLib is in common use. For example socket&net module, log module, thread&lock, tools for performance & memory 
leak check.

## Project Goals
 * Wrap Net operations which use linux epoll (ET), asynchronous & nonblock. no third party lib dependency.
 * wrap thread & lock & task queue, to make easier to program with multithread. lightweight wrap, even Embed System
   can use it easyly.
 * provide some tool for optimize program. performance monitor is to help developer find which part of code is consuming 
   more time and execute more. Object Count component will record numbers of all type realtime while will not 
   influence the performance of the program. It will be help to analyze user action and to analyze which part of 
   program is using most memory.
 * I will try some code for learn CQRS.
 * rpc related code integrated here, actually it is copy of FFRPC.
 * some wrap for db, xml operations.

## Log Module feature
 * it is a asynchronous log component, when you execute log interface, log content will post to log compoent thread, so
   main thread is not blcoked. When we develop distributed systen for game server, we expect to response user requeset
   as soon as possible. That's why we must need a asynchronous log component.
 * it use prinf style to format log content. As you know， sprintf is not type safe. But I implement it as type safe,
   thanks for c++ overloading, it not difficult but just to define nine interface for maximum nice arguments.
 * it can be set whether log contents will be print on console or output to file. even more, this can be modifed when
   runing.
 * it can be set log level, as you know FATAL,ERROR,WARN,INFO,TRACE,DEBUG which can be excute when runing.
 * when it chosen to print on console, it will print with different color for contents of different level.
 * it can be set to enable and disable some log class which can be excute when runing.
 
## Log Example
``` c++
	LOG.start("-log_path ./log -log_filename log -log_class FF,XX -log_print_screen true "
                  "-log_print_file true -log_level 6");
	LOGDEBUG(("XX", "FFFFF"));
	LOGTRACE(("XX", "FFFFF"));
	LOGINFO(("XX", "FFFFF"));
	LOGWARN(("XX", "FFFFF"));
	LOGERROR(("XX", "FFFFF"));
	LOGFATAL(("XX", "FFFFF"));
	LOG.mod_class("TT", true);

	LOGFATAL(("TT", "FFFFF"));
	LOGFATAL(("FF", "DSDFFFFF%s", string("SFWEGGGGGGGGG")));
	LOG.stop();
```

## Task Queue
 * when we use mutltithread, task queue is always needed. Task Queue is the tie for different. Multithread will 
   be uncoupled.
 * Programing Based task queue when use multithread.
 * it is easy to implement some pattern for multithread.such as one thread one task queue, N threads one task queue,
   N threads N task queue, N threads M task queue.

## Task Queue Example
``` c++
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
```
## Net module
 * It is asynchronous socket lib based ET epoll. You will know ET epoll is a finish state machine for io.
 * It use multithread by task queue component.
 * though I writes little word here, it is most important component of fflib.

``` c++
    int msg_broker_service_t::handle_broken(socket_ptr_t sock_);
    int msg_broker_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_);
    msg_broker_service_t broker_service;

    if (broker_service.start(argv[3]))
    {
        return -1;
    }

    if (NULL == net_factory_t::listen(buff, &broker_service))
    {
        cout <<"acceptor open failed:" << buff <<"\n";
        return 1;
    }
    else
    {
        cout <<"acceptor open ok, wait to listen\n";
    }
```

## Performance Monitor
 * It is a tool of collect performance of C++ program in realtime.
 * Some macro provied. The featur of c++ that stack object will be destructed anyway when code jump out of function,
   makes it possible to auto compute the end time sub start time of function.
 * Performance data will be output to file(CSV format) by timer. I also has some tools to analyze data, try FFUI.
 * thread safe
 * Each cost of Performance is equal to two of gettimeofday.
 * In my opinion, Performance Monitor should be always setup. There two advantages. first that it is a reference to optimize
   program, second that it can be found the problem where of program makes progr runing slow when some conditions.
## Performance Monitor Example
``` c++
	void foo()
	{
		AUTO_PERF();
	}
	singleton_t<performance_daemon_t>::instance().start("perf.txt", 5);
	foo();
	singleton_t<performance_daemon_t>::instance().stop();
```

## Performance Monitor Example CVS OutPut
time,mod,max_cost[us],min_cost[us],per_cost[us],request_per_second,exe_times
20120606-17:01:41,dumy,515,174,254,3937,390
20120606-17:01:41,foo,5924,4,506,1976,1030
20120606-17:01:41,test,304,8,243,4115,185
time,mod,max_cost[us],min_cost[us],per_cost[us],request_per_second,exe_times
20120606-17:11:41,dumy,1086,222,280,5571,312
20120606-17:11:41,foo,5707,194,503,1988,770
20120606-17:11:41,test,807,8,265,3773,142
time,mod,max_cost[us],min_cost[us],per_cost[us],request_per_second,exe_times
20120606-17:21:41,dumy,1086,222,680,2571,512
20120606-17:21:41,foo,5707,194,403,1388,470
20120606-17:21:41,test,807,8,265,4773,442

## Extra Tools
 * singleton
 * signal
 * daemon ops
 * string ops
 * smart_ptr
 * timer

Starting! See ....

	^_^
-----------------------

FFlib 是轻量级的C++类库，所谓轻量是指代码结构清晰，阅读性强，绝对的精巧极致，当
从底层到细节都遵循了极简原则时候，类库的性能自然而然达到了很高的水平。FFlib中总
结了非常多的现代化的C++ 技巧，Right Now C++11都呼之欲出了，C++er也要与时俱进了。


本人从事网络游戏开发工作，在设计开发分布式并发的C++服务器程序时，总结了很多经验
和模式，一方面希望自己的心得更有条理的整理出来，另一方面感觉到有些东西设计的还是
很有价值的，也希望能够和业界同仁共赏析, 这就是FFlib诞生的原因。

本类库的主要涉及：
	分布式：探讨了一种broker模式，参见知然工作室：
http://www.cnblogs.com/zhiranok/archive/2012/08/19/fflib_master_slave.html

	多线程并发:
如何使用多线程更加优雅、低耦合？任务队列可以组合出多种模式，玩转多线程。

	定时器： 
通用的定时器设计，如何更加高效、易用？使用epoll
实现的原生定时器，区区几百行代码而已。

	网络库:
边缘触发ET模式的epoll设计，最直接最简单的epoll用法，同样负责人告诉你，同样是最高效的用法（之一吧）。遗憾的是，网上充斥了太多的epoll错误的用法，即使某些业界大牛，也没有使用epoll至简至美的用法，希望对大家正确理解epoll有帮助。

	日志组件:
这个绝对是FFlib中最亮点之一，和网络库是最为本人津津乐道，sprintf的日志风格，但是保障了类型安全，异步调用，参数容错，这个在现有的日志库里几乎没有。

	AI、分类算法等：
主要是写着玩的，有的是C++写到，有的是python，比如A星算法、朴素贝叶斯、隐马尔科夫等。

	LUA: 
本库设计开发了lua
绑定、嵌入的lua库。最常用的lua操作都支持，同样的本库致力于教科书式的实现，代码保持很高的阅读性！

	CQRS: 
了解领域驱动设计DDD的应该了解一些CQRS，本人目前积极的探索CQRS的实践，希望针对大型C++服务器程序的复杂性提供一个设计框架！


oh nice !!!!!!!!
欢迎大家使用本库代码.
