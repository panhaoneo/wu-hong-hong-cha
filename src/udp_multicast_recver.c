// filename: recver.cc
// it's a demo to receive multicast udp data with the one of multi net interfaces....
 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
 
 
int main(void)
{
    const char* group_ip = "230.13.0.2"; // 组播地址
    int group_port = 3456;              // 组播的端口
 
    char recmsg[256];
 
    int n;
    int socket_fd;
    struct sockaddr_in group_addr;      //group address
    struct sockaddr_in local_addr;      //local address
    struct ip_mreq mreq;
    socklen_t addr_len = sizeof(group_addr);
    u_int yes;
 
    socket_fd=socket(AF_INET,SOCK_DGRAM,0);
    if(socket_fd < 0)
    {
        perror("socket multicast!");
        exit(1);
    }
 
    /*set up the local address*/
    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");   // 设定本地监听必须是0.0.0.0 这里是关键！
    local_addr.sin_port = htons(group_port);             //this port must be the group port
 
    // 设置端口复用
    if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
    {
        perror("Reusing ADDR failed");
        exit(1);
    }
 
    /*bind local address*/
    if(bind(socket_fd,(struct sockaddr *)&local_addr,sizeof(local_addr)) == -1)
    {
        perror("Binding the multicast!");
        exit(1);
    }
    
    /*use the setsocketopt() to request joining the multicast group*/
    mreq.imr_multiaddr.s_addr=inet_addr(group_ip);
    //mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    mreq.imr_interface.s_addr=inet_addr("180.0.0.109"); // 设定用新网卡来接收组播. 这里是关键！
    if (setsockopt(socket_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
    {   
/* fuck boost: 
udp_socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address_v4::from_string(group_ip),
boost::asio::ip::address_v4::from_string("180.0.0.109"))); */
        perror("setsockopt multicast!");
        exit(1);
    }
 
    /*set up the destination address*/
    memset(&group_addr,0,sizeof(struct sockaddr_in));
    group_addr.sin_family = AF_INET;
    group_addr.sin_port = htons(group_port);
    group_addr.sin_addr.s_addr = inet_addr(group_ip);
 
    /*loop to send or recieve*/
    while(1)
    {
 
       bzero(recmsg, sizeof(recmsg)-1);
       n = recvfrom(socket_fd, recmsg, sizeof(recmsg)-1, 0,
               (struct sockaddr *) &group_addr, &addr_len);
       if (n < 0) {
           printf("recvfrom err in udptalk!\n");
           exit(4);
       } else {
           /* success recieve the information */
           recmsg[n] = 0;
           printf("peer:%s \n", recmsg);
       }
 
    }
 
    return 0;
}