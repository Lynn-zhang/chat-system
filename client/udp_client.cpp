#include "udp_client.h"

udp_client::udp_client(std::string _remote_ip, unsigned short _remote_port)
	:remote_ip(_remote_ip),remote_port(_remote_port), sock(-1)
{
}

//初始化socket套接字描述符	
int udp_client::init_client()
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		exit(1);
	}
	return 0;
}
//接收客户端响应(广播来的信息), 从out_msg传出
int udp_client::reliable_recv_msg(std::string &out_msg)
{
	char buf[G_BLOCK_SIZE];
	memset(buf, '\0', sizeof(buf));
	struct sockaddr_in remote;
	socklen_t len = sizeof(remote);
	ssize_t _size = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&remote, &len);
	if(_size < 0){
		out_msg = "";
		return 1;
	}else if( _size >= 0 ){
		out_msg = buf;
	}
	return 0;
}
//发送数据到服务器的套接字描述符
int udp_client::reliable_send_msg(const std::string &in_msg)
{
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(remote_port);
	remote.sin_addr.s_addr = inet_addr(remote_ip.c_str());
	ssize_t _size = sendto(sock, in_msg.c_str(), in_msg.size(), 0, (struct sockaddr*)&remote, sizeof(remote));
	if(_size < 0){
		return 1;
	}else{
	}
	return 0;
}

udp_client::~udp_client()
{
	if(sock > 0){
		close(sock);
	}
}
//遍历好友列表，判断好友是否在列表中
bool udp_client::is_friend_exist(const std::string &_key)
{
	std::vector<std::string>::iterator _iter=friends_list.begin();
	for( ; _iter != friends_list.end(); _iter++){
		if( *_iter == _key ){
			return true;
		}
	}
	return false;
}

void udp_client::add_friend(const std::string &_key)
{
	if(is_friend_exist(_key)){
		//Do Nothing
	}else{
		friends_list.push_back(_key);
	}
}

void udp_client::del_friend(const std::string &_key)
{
	if(is_friend_exist(_key)){
		std::vector<std::string>::iterator _iter = friends_list.begin();
		for( ; _iter != friends_list.end(); ){
			if( *_iter == _key ){
				_iter = friends_list.erase(_iter);
				return;
			}else{
				_iter++;
			}
		}
	}else{
		//Do Nothing
	}
}



