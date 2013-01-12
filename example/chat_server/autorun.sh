#!/bin/sh
python ../../fflib/lib/generator/idl_generator.py idl/chat.idl include/msg_def.h
make && ./chat_server 127.0.0.1 10241