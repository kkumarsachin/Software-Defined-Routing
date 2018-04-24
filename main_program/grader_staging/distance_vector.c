#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/queue.h>
#include <unistd.h>
#include <string.h>

#include "../include/connection_manager.h"
#include "../include/global.h"
#include "../include/control_handler.h"
#include "../include/control_header_lib.h"

distanceVector( struct ROUTING_UPDATE_PKT *neighborrouter)


{

  uint16_t source_ID;
  struct ROUTING_UPDATE_PKT neighbor_router;
  //struct Forwarding_Table FRouter[5];

neighbor_router.num_of_update_fields = neighborrouter->num_of_update_fields;  
neighbor_router.source_router_port = neighborrouter->source_router_port;
neighbor_router.source_router_ip_add = neighborrouter->source_router_ip_add;

for(int i =0; i<5;i++)
{
   neighbor_router.router_u[i].router_u_port = neighbor_router->router_u[i].router_u_port;  
   neighbor_router.router_u[i].router_u_ID = neighbor_router->router_u[i].router_u_ID;
   neighbor_router.router_u[i].cost_u = ntohs(neighbor_router->router_u[i].cost_u);
 }
  
for(int k =0; k<5;k++)
 if(update_pkt.router_u[k].router_u_port == neighbor_router.source_router_port )
    {
       source_ID = update_pkt.router_u[k].router_u_ID;
      for(int j =0;j<5;j++)
         if(neighbor_router.router_u[j].cost_u != 65535)
         neighbor_router.router_u[j].cost_u = neighbor_router.router_u[j].cost_u +ntohs(update_pkt.router_u[k].cost_u) ;
     }
printf("initial\n");

for(int r =0 ; r<5;r++)
printf("%u %u %u\n",ntohs(FRouter[r].Frouter_ID),ntohs(FRouter[r].Fcost),ntohs(FRouter[r].next_hop));


for(int l =0;l<5;l++)
 if(ntohs(update_pkt.router_u[l].cost_u) > neighbor_router.router_u[l].cost_u)
   {
     update_pkt.router_u[l].cost_u = htons(neighbor_router.router_u[l].cost_u);
     FRouter[l].Fcost = htons(neighbor_router.router_u[l].cost_u);
     FRouter[l].next_hop = source_ID;
     
     }
printf("After\n");

for(int r =0 ; r<5;r++)
printf("%u %u %u %u\n",ntohs(FRouter[r].Frouter_ID),ntohs(FRouter[r].Fcost),ntohs(FRouter[r].next_hop),ntohs(update_pkt.router_u[r].cost_u));

 }
