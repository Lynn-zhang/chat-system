#pragma once

#include <iostream>
#include <string>
#include <map>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"
#include "data_pool.h"
#include "udp_data.h"

class udp_server{
	private:
		int recv_msg(std::string &_out_msg);
		int send_msg(const std::string &_in_msg, const struct sockaddr_in &client, socklen_t len);
		//记录在线用户
		bool register_user(const struct sockaddr_in &client, std::string&);

	public:
		//构造函数，初始化数据成员
		udp_server(unsigned short _port=8888); 
		int init_server();
		
		//生产者线程调用：
		//服务器将从客户端接收到的消息推送到消息池中，
		//并调用register_user()函数分析该用户是需要插入到用户队列中还是要被剔除.
		int reliable_recv_msg();
		
		//消费者线程调用：
		//服务器将消息池中的数据(利用send_msg()函数)广播给所有在线用户
		int broadcast_msg();
		
		~udp_server();

	private:
		int sock; //套接字描述符
		unsigned short port; //服务器主机端口号
		data_pool pool;  //消息池
		std::map<std::string, struct sockaddr_in> online_user;  //利用map保存所有的客户端
};

