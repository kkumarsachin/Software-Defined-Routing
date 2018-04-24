/**
 * @connection_manager
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
 * Connection Manager listens for incoming connections/messages from the
 * controller and other routers and calls the desginated handlers.
 */

#include <sys/select.h>
#include<time.h>
#include<sys/time.h>
#include "../include/connection_manager.h"
#include "../include/global.h"
#include "../include/global.h"
#include "../include/control_handler.h"
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
#include "../include/control_header_lib.h"
#include "../include/router_changes.h"
#define NEIGH_ROUT_SIZE 68


//fd_set master_list, watch_list;
//int head_fd;

struct ROUTING_UPDATE_PKT neighbor_update_pkt;

void main_loop()
{
    int selret, sock_index, fdaccept;
    struct sockaddr_in their_addr;
    int neighbor_node;
    socklen_t addresslength = sizeof(their_addr);
    int num_of_bytes_received ;
    struct timeval start, end;
    int total_time_taken;
     //period =1;
     //timer = 0;
     //int flag = 0;
    tv.tv_sec = 50;
    tv.tv_usec = 0;
    
    printf("i am in connection manager %u\n",period);
    while(TRUE){
        
        printf("when i entered while time is %lu and %lu\n",tv.tv_sec, tv.tv_usec);
        watch_list = master_list;
        selret = select(head_fd+1, &watch_list, NULL, NULL, &tv);
        printf("when i entered select time is %lu and %lu\n",tv.tv_sec, tv.tv_usec);
        printf("select retrun value is %d............\n",selret);

        if (selret < 0) {

            ERROR("select failed.");
                           //printf("control = %d, router = %d\n",control_socket,router_socket);
             }
        else if (selret == 0 )

            {  
              
              //timer++;
              //printf("timer values of %d\n",timer); 
              //if(timer == period){
               int num_of_bytes=0;
               struct sockaddr_in neighbor_addr;
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
                          timer = 0;
                          //flag = 1;
                       }
                   }
                  
                   tv.tv_sec = period;
                   tv.tv_usec = 0;
		 printf("when i entered timeout happens is %lu, %lu\n",tv.tv_sec, tv.tv_usec);
                   
	     
              //}
             }
            
     
     else{
        /* Loop through file descriptors to check which ones are ready */
          gettimeofday(&start, NULL); 
       printf("when i entered some socket 0is %lu and %lu\n",tv.tv_sec, tv.tv_usec);
        for(sock_index=0; sock_index<=head_fd; sock_index+=1){
            
            if(FD_ISSET(sock_index, &watch_list)){
                printf("socket index = %d\n",sock_index);
                /* control_socket */
                if(sock_index == control_socket){
                    fdaccept = new_control_conn(sock_index);

                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd) head_fd = fdaccept;
                }

                /* router_socket */
                else if(sock_index == router_socket){
                    
                     
                    //if(flag ==1){
                     printf("maga enter aythu.......\n");
                    num_of_bytes_received = recvfrom(sock_index, &neighbor_update_pkt, ((number_of_routers*12)+8) ,0,(struct sockaddr *)&their_addr,&addresslength);
            
                     while(num_of_bytes_received != ((number_of_routers*12)+8))
                      num_of_bytes_received+= recvfrom(sock_index, &neighbor_update_pkt+num_of_bytes_received, ((number_of_routers*12)+8)-num_of_bytes_received,0,(struct sockaddr *)&their_addr,&addresslength);
                      
                    printf("received = %d\n",num_of_bytes_received);
                    printf("%u\n",ntohs(neighbor_update_pkt.num_of_update_fields));
                    for(int i =0;i<number_of_routers;i++)
                      printf("%u %u %u %u %u %u %u\n",ntohs(neighbor_update_pkt.num_of_update_fields),ntohs(neighbor_update_pkt.source_router_port),ntohs(neighbor_update_pkt.source_router_ip_add),ntohs(neighbor_update_pkt.router_u[i].router_u_port),ntohs(neighbor_update_pkt.router_u[i].padding_u),ntohs(neighbor_update_pkt.router_u[i].router_u_ID),ntohs(neighbor_update_pkt.router_u[i].cost_u));
                    
                     
                     for(int i =0; i<number_of_routers;i++)
                         if( system_table[i].router_port == neighbor_update_pkt.source_router_port)
                             neighbor_node = system_table[i].ID;
                     printf("neighbor node is %d\n",neighbor_node);
                     for(int k = 0;k<number_of_routers;k++)
                        vector_table[neighbor_node][k] = ntohs(neighbor_update_pkt.router_u[k].cost_u);
                    
                     for(int i=0;i<number_of_routers;i++)
                         {printf("\n");
                           for(int j =0;j<number_of_routers;j++)
                               printf("%u        ",vector_table[i][j]);  
                          }
                       printf("\n");
                       printf("\n");

                      for(int k =0;k<number_of_routers;k++)
  			{
			   //next[k] = ntohs(system_table[k].next_hop);
			   printf("before next hop %d is %d\n",k,next[k]);
			}

                    distance_vector();

                    for(int i=0;i<number_of_routers;i++)
                         {printf("\n");
                           for(int j =0;j<number_of_routers;j++)
                               printf("%u        ",vector_table[i][j]);  
                          }
                      printf("\n");
                       printf("\n");
                     for(int k =0;k<number_of_routers;k++)
  			{
			   //next[k] = ntohs(system_table[k].next_hop);
			   printf("After next hop %d is %d\n",k,next[k]);
			}
                    
                     printf("\n");
                    printf("end\n");
     
                   routing_table_update();
                   update_packet_table();
                   update_forwarding_table();
                   //gettimeofday(&, NULL)
                // }
                    
                }

                /* data_socket */
                else if(sock_index == data_socket){
                    //new_data_conn(sock_index);
                }

                /* Existing connection */
                else{
                    if(isControl(sock_index)){
                        if(!control_recv_hook(sock_index)) FD_CLR(sock_index, &master_list);
                    }
                    //else if isData(sock_index);
                    else ERROR("Unknown socket index");
                }
             }
           gettimeofday(&end, NULL);
           
           total_time_taken = ((end.tv_sec - start.tv_sec)*1000000 +(end.tv_usec-start.tv_usec)); 
           printf("time taken is %d\n",total_time_taken);
           printf("old time %lu\n",tv.tv_usec);
           tv.tv_usec = tv.tv_usec+total_time_taken;
           printf("new time %lu\n",tv.tv_usec);
         } 
       }
    }
}

void init()
{
    control_socket = create_control_sock();

    //router_socket and data_socket will be initialized after INIT from controller

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the control socket */
    FD_SET(control_socket, &master_list);
    head_fd = control_socket;

    main_loop();
}



/*void initialize_router(router_socket)

{
  FD_SET(router_socket, &master_list);
     
  main_loop();

  }
 */
