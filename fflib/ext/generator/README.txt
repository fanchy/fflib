1. 修改 example.idl 文件
2. 使用idl_generator.py 生成c++头文件
	idl_generator.py example.idl msg_def.h
3. 编译示例程序
	g++ main.cpp -o app_test
