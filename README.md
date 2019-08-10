# SimpleTCP
简单的TCP并发服务端-客户端程序</br>

# 文件说明
utils.h 与 utils.cpp 声明并实现了一些跨平台的API</br>
server.cpp 服务端程序入口，为每个客户端TCP连接创建一个线程，接收数据并echo</br>
client.cpp 客户端程序入口，包含两个线程：主线程将标准输入的数据发送给服务端，接收线程接收服务端的数据并显示在标准输出</br>

# 编译说明
使用C++11的 thread mutex 与 unordered_map</br>
Linux 编译请加入编译选项 -std=c++11</br>
Windows 建议用VS2015创建工程编译。在“项目属性 -> C/C++ -> 预处理器 -> 预处理器定义”中定义宏WINDOWS</br>

# 使用说明
客户端标准输入为"exit"时断开与服务器的连接</br>
