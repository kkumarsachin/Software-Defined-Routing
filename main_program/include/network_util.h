#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes);
ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes);
//ssize_t sendALLUDP(int sock_index, char *buffer, ssize_t nbytes, struct sockaddr *from, int fromlen );
//ssize_t recvALLUDP(int sock_index, char *buffer, ssize_t nbytes, struct sockaddr *from, int *fromlen);

#endif
