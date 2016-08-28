#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "comm.h"
#include "udp_data.h"

class udp_client{
	public:
		udp_client(std::string _remote_ip = "127.0.0.1",unsigned short _port = 8888);
		int init_client();
	
		////接收客户端响应(广播来的信息), 从out_msg传出
		int reliable_recv_msg(std::string &out_msg);
		//发送数据到服务器的套接字描述符
		int reliable_send_msg(const std::string &in_msg);

		~udp_client();
		
		 //处理好友列表	
	        void add_friend(const std::string &_key);
	        void del_friend(const std::string &_key);
	        std::vector<std::string>& get_friends_list()
	        {
	            return this->friends_list;
	        }
	private:
		//判断好友是否存在于列表中
		bool is_friend_exist(const std::string &_key);        
	private:
		int sock;
		std::string remote_ip;
		unsigned short remote_port;
		std::vector<std::string> friends_list;
};
