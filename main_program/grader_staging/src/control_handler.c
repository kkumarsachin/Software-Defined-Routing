/**
 * @control_handler
 * @author  Swetank Kumar Saha <swetankk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Handler for the control plane.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/queue.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../include/router_changes.h"
#include "../include/global.h"
#include "../include/network_util.h"
#include "../include/control_header_lib.h"
#include "../include/author.h"
#include "../include/connection_manager.h"
#include "../include/init.h"
#include "../include/routing_update_response.h"
#include "../include/update_response.h"
#include "../include/crash_response.h"
#ifndef PACKET_USING_STRUCT
    #define CNTRL_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_PAYLOAD_LEN_OFFSET 0x06
#endif

#define MAXBUFLEN 100

/* Linked List for active control connections */
struct ControlConn
{
    int sockfd;
    LIST_ENTRY(ControlConn) next;
}*connection, *conn_temp;
LIST_HEAD(ControlConnsHead, ControlConn) control_conn_list;

int create_router_sock(uint16_t src_router_port)
{     
        //printf("hi = %u\n",src_router_port);
   	int sockfd;
	struct sockaddr_in my_addr;
	// my address information
	struct sockaddr_in their_addr; // connector’s address information
	int addr_len, numbytes;
	char buf[MAXBUFLEN];
        
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	exit(1);
	}
         //printf("socket = %d\n",sockfd);
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int)) < 0)
           ERROR("setsockopt() failed");

        //printf("socket = %d\n",sockfd);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(src_router_port);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	//printf("socket = %d\n",sockfd);
	if (bind(sockfd, (struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1) {	
          printf("error bro\n");
	  perror("bind");
	  exit(1);
        }
	    FD_SET(sockfd, &master_list);
            if(sockfd>head_fd)
             	 head_fd = sockfd;      

        //printf("socket = %d\n",sockfd);
         //printf("socket is %d\n",sockfd);
         return sockfd;	
 }


 

int create_control_sock()
{
    int sock;

    struct sockaddr_in control_addr;
    socklen_t addrlen = sizeof(control_addr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        ERROR("socket() failed");

    /* Make socket re-usable */
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int)) < 0)
        ERROR("setsockopt() failed");

    bzero(&control_addr, sizeof(control_addr));

    control_addr.sin_family = AF_INET;
    control_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    control_addr.sin_port = htons(CONTROL_PORT);

    if(bind(sock, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
        ERROR("bind() failed");

    if(listen(sock, 5) < 0)
        ERROR("listen() failed");

    LIST_INIT(&control_conn_list);

    return sock;
}

int new_control_conn(int sock_index)
{
    int fdaccept, caddr_len;
    struct sockaddr_in remote_controller_addr;

    caddr_len = sizeof(remote_controller_addr);
    fdaccept = accept(sock_index, (struct sockaddr *)&remote_controller_addr, &caddr_len);
    if(fdaccept < 0)
        ERROR("accept() failed");

    /* Insert into list of active control connections */
    connection = malloc(sizeof(struct ControlConn));
    connection->sockfd = fdaccept;
    LIST_INSERT_HEAD(&control_conn_list, connection, next);

    return fdaccept;
}

void remove_control_conn(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next) {
        if(connection->sockfd == sock_index) LIST_REMOVE(connection, next); // this may be unsafe?
        free(connection);
    }

    close(sock_index);
}

bool isControl(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next)
        if(connection->sockfd == sock_index) return TRUE;

    return FALSE;
}

bool control_recv_hook(int sock_index)
{
 
    int num_of_bytes=0;
    char *cntrl_header, *cntrl_payload;
    uint8_t control_code;
    uint16_t payload_len;
    uint16_t source_router_port;
   struct sockaddr_in neighbor_addr;
    
    
    //int router_socket;
    /* Get control header */
    cntrl_header = (char *) malloc(sizeof(char)*CNTRL_HEADER_SIZE);
    bzero(cntrl_header, CNTRL_HEADER_SIZE);

    if(recvALL(sock_index, cntrl_header, CNTRL_HEADER_SIZE) < 0){
        remove_control_conn(sock_index);
        free(cntrl_header);
        return FALSE;
    }

    /* Get control code and payload length from the header */
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_HEADER) != CNTRL_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_HEADER *header = (struct CONTROL_HEADER *) cntrl_header;
        control_code = header->control_code;
        payload_len = ntohs(header->payload_len);
    #endif
    #ifndef PACKET_USING_STRUCT
        memcpy(&control_code, cntrl_header+CNTRL_CONTROL_CODE_OFFSET, sizeof(control_code));
        memcpy(&payload_len, cntrl_header+CNTRL_PAYLOAD_LEN_OFFSET, sizeof(payload_len));
        payload_len = ntohs(payload_len);
    #endif

    free(cntrl_header);

    /* Get control payload */
    if(payload_len != 0){
        cntrl_payload = (char *) malloc(sizeof(char)*payload_len);
        bzero(cntrl_payload, payload_len);

        if(recvALL(sock_index, cntrl_payload, payload_len) < 0){
            remove_control_conn(sock_index);
            free(cntrl_payload);
            return FALSE;
        }
    }
 
    
        struct CONTROL_PAYLOAD_INIT *payload = (struct CONTROL_PAYLOAD_INIT *) cntrl_payload;
        
	
      
  
         

    switch(control_code){
        case 0: author_response(sock_index);
                break;

        
        case 1: 
               
                //number_of_routers = ntohs (payload->num_of_routers);
               //printf("no of routers %u\n",number_of_routers);
               
                // Copy it to the routing table
                // run distance vector
                // extract the update packet for routers
                //create the forwarding table
                //send the init response  
                 //timer = 0;
                 printf("inti entered\n");
                 tv.tv_sec = ntohs(payload->periodic_interval);
                 tv.tv_usec = 0;
                 dump_routing_table(payload);
                 initialize_distance_matrix();
                 source_router_port = initialize_update_pkt(payload);
                 initialize_forwarding_table (payload);
                 distance_vector();
                 
                 routing_table_update();
          
                 printf("number is .......%u\n",number_of_routers);
                 

		 router_socket = create_router_sock(source_router_port);
               /*
                 for(int i = 0;i<number_of_routers;i++)
                  {
                    if(system_table[i].isneighbor)
                       {
                         printf("neighbor is  %u with %u\n",ntohs(system_table[i].router_ID),ntohs(system_table[i].router_port));
                         
                         //from beej guide
                         neighbor_addr.sin_family = AF_INET;
			 neighbor_addr.sin_port = system_table[i].router_port; 
		         neighbor_addr.sin_addr = *(struct in_addr *)&system_table[i].router_ip_add;
			 bzero(&(neighbor_addr.sin_zero), 8);
                         //sizeof(update_pkt)
                         num_of_bytes = sendto(router_socket, &update_pkt,((number_of_routers*12)+8) ,0,(struct sockaddr *)&neighbor_addr, sizeof(neighbor_addr));
                         
                        while(num_of_bytes != ((number_of_routers*12)+8))
                        num_of_bytes += sendto(router_socket, &update_pkt+num_of_bytes,((number_of_routers*12)+8)-num_of_bytes ,0,(struct sockaddr *)&neighbor_addr, sizeof(neighbor_addr));

                         printf("number of bytes = %d\n",num_of_bytes);
                       }
                   }
			
                 */
			//printf("hello %d\n",router_socket);    
		//initialize_router(router_socket);
	       init_response(sock_index);
               break;
  case 2: //copy the value of the 
          printf("entered the case 2\n");
          
          /*
                 for(int i = 0;i<number_of_routers;i++)
                  {
                    if(system_table[i].isneighbor)
                       {
                         printf("neighbor is  %u with %u\n",ntohs(system_table[i].router_ID),ntohs(system_table[i].router_port));
                       
                         
                         //from beej guide
                         neighbor_addr.sin_family = AF_INET;	
			 neighbor_addr.sin_port = system_table[i].router_port; 
		         neighbor_addr.sin_addr = *(struct in_addr *)&system_table[i].router_ip_add;
			 bzero(&(neighbor_addr.sin_zero), 8);
                         //sizeof(update_pkt)
                         num_of_bytes = sendto(router_socket, &update_pkt,((number_of_routers*12)+8) ,0,(struct sockaddr *)&neighbor_addr, sizeof(struct sockaddr));
                         printf("number of bytes = %d\n",num_of_bytes);
                       }
                   }
			
                */
          routing_update_response(sock_index);
          break;

case 3 :
         update_response(sock_index);
            for(int i=0;i<number_of_routers;i++)
              if(system_table[i].router_ID == payload->num_of_routers)
               {
                 vector_table[source_node][i] = ntohs(payload->periodic_interval);
                 vector_table[i][source_node] = ntohs(payload->periodic_interval);
                next[i] =  ntohs (payload->num_of_routers);
                 
                 //vector_table[i][source_node] = ntohs(payload->periodic_interval);
                 printf("change between %u and %u or %u is %u\n",source_node ,i ,ntohs(system_table[i].router_ID),ntohs(payload->periodic_interval ));

               }
            //printf("id = %u, cost = %u^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",updated_router_id ,updated_link_cost);

            printf("before dv\n");
             for(int i=0;i<number_of_routers;i++)
               {printf("\n");
                 for(int j =0;j<number_of_routers;j++)
                 
                   printf("%u        ",vector_table[i][j]);  
                }
                printf("\n");
                printf("\n");

            distance_vector();

             printf("after dv\n");
            for(int i=0;i<number_of_routers;i++)
               {printf("\n");
                 for(int j =0;j<number_of_routers;j++)
               
                   printf("%u        ",vector_table[i][j]);  
                }
                printf("\n");
                printf("\n");

            routing_table_update();
            update_packet_table();
            update_forwarding_table();
            


         break;
case 4 : 
          crash_response(sock_index);
          exit(0);
          break;

    }

    if(payload_len != 0) free(cntrl_payload);
    return TRUE;
}

 


