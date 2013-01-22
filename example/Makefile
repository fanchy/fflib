
.PHONY:all

all:
	make -C book && make -C  broker && make -C  cqrs && make -C  echo_client && make -C  echo_server && make -C  event_store && make -C  ff_performance && make -C  game_framework   && make -C  ranklist && make -C  rpc && make -C  tutorial

clean:
	make clean -C book 
	make clean -C  broker 
	make clean -C  cqrs
	make clean -C   echo_client
	make clean -C   echo_server 
	make clean -C  event_store
	make clean -C   ff_performance
	make clean -C   game_framework 
	#make clean -C   plugin_msg_broker
	make clean -C   ranklist
	make clean -C   rpc
	make clean -C   tutorial
