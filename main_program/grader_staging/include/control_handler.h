#ifndef CONTROL_HANDLER_H_
#define CONTROL_HANDLER_H_


//struct sockaddr_in neighbor_addr;
//int num_of_bytes=0;

int create_control_sock();
int new_control_conn(int sock_index);
bool isControl(int sock_index);
bool control_recv_hook(int sock_index);

//struct ROUTING_UPDATE_PKT update_pkt;
//struct Forwarding_Table FRouter[5];


#endif
