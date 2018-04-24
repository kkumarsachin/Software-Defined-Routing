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

#ifndef PACKET_USING_STRUCT
    #define CNTRL_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_PAYLOAD_LEN_OFFSET 0x06
#endif

#define MAXBUFLEN 100

uint16_t src_id;
uint32_t src_ip;
uint16_t src_rport;
//uint16_t vector_table[5][5];
//uint16_t next[5];
//int source_node;
//uint16_t number_of_routers;
//struct ROUTING_UPDATE_PKT update_pkt;
//struct Forwarding_Table *FRouter;
//struct router_table *system_table;
//int period;     

void dump_routing_table( struct CONTROL_PAYLOAD_INIT *payload)
 {
   period = ntohs(payload->periodic_interval);
   printf("period is %u\n",period);
   number_of_routers = ntohs (payload->num_of_routers);
 system_table = malloc(number_of_routers*sizeof(struct router_table));
  printf("start route table.......%u\n",number_of_routers);
              for(int i=0;i<number_of_routers;i++)
                { 
                   system_table[i].ID = i;
                   system_table[i].router_ID = payload->router[i].router_ID;
                   system_table[i].router_port = payload->router[i].router_port;
                   system_table[i].data_port = payload->router[i].data_port;
		   system_table[i].link_cost = payload->router[i].link_cost;
		   system_table[i].router_ip_add = payload->router[i].router_ip_add;
		   system_table[i].padding = 0x00;
 
                   if(ntohs(payload->router[i].link_cost == 0))
                     { 
                       system_table[i].isneighbor = 0;
                       system_table[i].next_hop = payload->router[i].router_ID;
                       src_id = payload->router[i].router_ID;
                       src_ip = payload->router[i].router_ip_add;
                       src_rport = payload->router[i].router_port;
                        source_node = system_table[i].ID;
                      }
 
                    else if(ntohs(payload->router[i].link_cost == 65535))
                     {
                       system_table[i].isneighbor = 0;
                       system_table[i].next_hop = 65535;
                      }
                   else {
                      system_table[i].isneighbor = 1;
		      system_table[i].next_hop = payload->router[i].router_ID; 
                     }
                  }
              
             for(int k =0 ;k <number_of_routers;k++)
                {
  			system_table[k].source_router_id =  src_id ;
                        system_table[k].source_router_ip_address =  src_ip;   
 			system_table[k].source_router_port= src_rport;
                 
                 }
             for(int i=0;i<number_of_routers;i++)
                printf("%d %u %u %u %u %u %u %u %u %u %u %u\n",system_table[i].ID, ntohs(system_table[i].source_router_id ),ntohs(system_table[i].source_router_ip_address),ntohs(system_table[i].source_router_port),ntohs(system_table[i].router_ID),ntohs(system_table[i].router_port),ntohs(system_table[i].data_port),ntohs(system_table[i].link_cost),ntohs(system_table[i].router_ip_add),ntohs(system_table[i].padding),system_table[i].isneighbor,ntohs(system_table[i].next_hop));
  
//printf("source is %d\n",source_node);

printf("end route table........\n");
//end of dumping the routing table
   
//return 0;

}


void initialize_distance_matrix()

{
   //start initialzig the distance vector

 //printf("start distance vector table........\n"); 
   
   for(int i=0;i<number_of_routers;i++)
     for(int j =0;j<number_of_routers;j++)
       vector_table[i][j] = 65535;
   //printf("test\n");
   for(int k = 0;k<number_of_routers;k++)
      vector_table[source_node][k] = ntohs(system_table[k].link_cost);
    //printf("test\n");
  /*  for(int i=0;i<number_of_routers;i++)
      {printf("\n");
     for(int j =0;j<number_of_routers;j++)
       printf("%u        ",vector_table[i][j]);  
    }
  */
  //for(int k)
   
 // printf("\n");

//printf("end distance vector table........\n"); 
//end distance vector


//initalize next_hop id


for(int k =0;k<number_of_routers;k++)
{
next[k] = ntohs(system_table[k].next_hop);
printf("next hop %d is %d\n",k,next[k]);
}
 
//initalize next_hop id
 }



int initialize_update_pkt(struct CONTROL_PAYLOAD_INIT *payload)
{
    
    //update_pkt = malloc(sizeof(struct ROUTING_UPDATE_PKT) + number_of_routers*sizeof(struct router_update_details) );      
   //update packet
// printf("start update  packet..........\n");
	    for(int i = 0;i<number_of_routers;i++)
 		{
		   if(payload->router[i].link_cost ==0)
		    { 
                        //printf("%u\n",ntohs(payload->num_of_routers));
			//printf("%u\n",ntohs(update_pkt.num_of_update_fields));
			update_pkt.num_of_update_fields = payload->num_of_routers;
 			update_pkt.source_router_ip_add = payload->router[i].router_ip_add;                      
 			 update_pkt.source_router_port= payload->router[i].router_port;
                          printf("souce = %u\n",ntohs(update_pkt.source_router_port)); 
		         //memcpy( update_pkt->source_router_port ,&source_port,sizeof(update_pkt->source_router_port));
		    }
			//printf("source is %u\n",ntohs(update_pkt.source_router_port));
		     update_pkt.router_u[i].router_u_ip_add = payload->router[i].router_ip_add;
		     update_pkt.router_u[i].router_u_port = payload->router[i].router_port;
	             update_pkt.router_u[i].padding_u = 0;	
		     update_pkt.router_u[i].router_u_ID = payload->router[i].router_ID;
		     update_pkt.router_u[i].cost_u = payload->router[i].link_cost;
		  printf("%u %u %u %u\n",ntohs(update_pkt.router_u[i].router_u_port),ntohs(update_pkt.router_u[i].padding_u),ntohs(update_pkt.router_u[i].router_u_ID),ntohs(update_pkt.router_u[i].cost_u));
	
		 } 
	

//printf("end update  packet..........\n");
return ntohs(update_pkt.source_router_port);
//update packet


}
   

void initialize_forwarding_table (struct CONTROL_PAYLOAD_INIT *payload)

{
   FRouter = malloc(number_of_routers*sizeof(struct Forwarding_Table));
 // start forwarding table
printf("start forwarding table .....\n");
		 
	         for(int i=0;i<number_of_routers;i++)

 	         {
	            FRouter[i].Frouter_ID = payload->router[i].router_ID;
		    FRouter[i].Fpadding = 0;
	            if(ntohs(payload->router[i].link_cost) != 65535 )
		{ //printf("cost = %u     \n",ntohs(payload->router[i].link_cost));
 
	            FRouter[i].next_hop = payload->router[i].router_ID;   }
	           
		    else 
		    FRouter[i].next_hop = 65535;

	            FRouter[i].Fcost = payload->router[i].link_cost;
	          
                    printf("%u %u %u %u\n",ntohs(FRouter[i].Frouter_ID),ntohs(FRouter[i].Fpadding),ntohs(FRouter[i].next_hop),ntohs(FRouter[i].Fcost));
		    }
printf("end forwarding table .....\n");
//end forwarding table


}




void distance_vector()
{
  //distance vector run
  for(int m =0; m<number_of_routers;m++)
  for(int n =0;n<number_of_routers;n++)
  if(system_table[n].isneighbor == 1){
   if(vector_table[source_node][m] >vector_table[source_node][n]+vector_table[n][m] )
       {
         vector_table[source_node][m] = vector_table[source_node][n]+vector_table[n][m];
         next[m] = ntohs(system_table[n].router_ID);
         //printf("m = %d, n = %d\n",m,n);
         //printf("next hop = %d",next[n]);
       }

  //  else if(vector_table[source_node][m] <vector_table[source_node][n]+vector_table[n][m])
    // {
      
    //  next[m] = ntohs(system_table[m].router_ID);

     // }

    }
 


 /*   for(int i=0;i<number_of_routers;i++)
      {printf("\n");
     for(int j =0;j<number_of_routers;j++)
       printf("%u        ",vector_table[i][j]);     
     }

  */

 }


void routing_table_update()
{
 
 //copy to routing table
 printf("start routing table......\n");
for(int k =0;k<number_of_routers;k++){
  system_table[k].link_cost = htons(vector_table[source_node][k] );
  system_table[k].next_hop = htons(next[k]);
}

for(int i=0;i<number_of_routers;i++)
                printf("%d %u %u %u %u %u %u %u %u %u %u %u\n",system_table[i].ID, ntohs(system_table[i].source_router_id ),ntohs(system_table[i].source_router_ip_address),ntohs(system_table[i].source_router_port),ntohs(system_table[i].router_ID),ntohs(system_table[i].router_port),ntohs(system_table[i].data_port),ntohs(system_table[i].link_cost),ntohs(system_table[i].router_ip_add),ntohs(system_table[i].padding),system_table[i].isneighbor,ntohs(system_table[i].next_hop));

printf("end routing table......\n");
//copy to routing table


}

void update_packet_table()

{
 printf("start updated update table......\n");
 for (int k =0;k<number_of_routers;k++){
 
  update_pkt.router_u[k].cost_u = system_table[k].link_cost ;
 printf("%u %u %u %u\n",ntohs(update_pkt.router_u[k].router_u_port),ntohs(update_pkt.router_u[k].padding_u),ntohs(update_pkt.router_u[k].router_u_ID),ntohs(update_pkt.router_u[k].cost_u));
}
printf("end updated update table......\n");
}

void update_forwarding_table()
{
  printf("start updated forwarding table......\n");
  for(int i =0;i<number_of_routers;i++){
  FRouter[i].Fcost = system_table[i].link_cost ;
  FRouter[i].next_hop = system_table[i].next_hop;
  printf("%u %u %u %u\n",ntohs(FRouter[i].Frouter_ID),ntohs(FRouter[i].Fpadding),ntohs(FRouter[i].next_hop),ntohs(FRouter[i].Fcost));
}

  printf("end updated forwarding table......\n");
}

