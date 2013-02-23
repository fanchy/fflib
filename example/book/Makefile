
.PHONY:all

all:
	make -C log  && make -C lua  &&make -C task_queue && make -C sqlite && make -C count && make -C count_client

clean:
	make clean -C log
	make clean -C lua
	make clean -C task_queue
	make clean -C sqlite
	make clean -C count
	make clean -C count_client
