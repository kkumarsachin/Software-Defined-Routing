#ifndef ROUTER_CHANGES_H_
#define ROUTER_CHANGES_H_

    
     struct __attribute__((__packed__)) router_details
      {
         
	uint16_t router_ID;
	uint16_t router_port;
	uint16_t data_port;
	uint16_t link_cost;
        uint32_t router_ip_add;
        

        };

     //struct router_details router[5];
    struct __attribute__((__packed__)) CONTROL_PAYLOAD_INIT
    {
        uint16_t num_of_routers;
 	uint16_t periodic_interval;
	
	struct router_details router[5];

	//router_details router[1];
	//router_details router[2];
	//router_details router[3];
	//router_details router[4];

	//struct router_details router1;
	//struct router_details router2;
	//struct router_details router3;
	//struct router_details router4;
	//struct router_details router5;
	//uint16_t router_port[5];
	//uint16_t data_port[5];
	//uint16_t link_cost[5];
        //uint32_t router_ip_addr[5];
        
    };

uint16_t number_of_routers;
uint16_t vector_table[5][5];
uint16_t next[5];
int source_node;
int period;
struct Forwarding_Table *FRouter;
    struct ROUTING_UPDATE_PKT update_pkt; 
struct router_table *system_table;

void dump_routing_table( struct CONTROL_PAYLOAD_INIT *payload);
void initialize_distance_matrix();

int initialize_update_pkt(struct CONTROL_PAYLOAD_INIT *payload);
void initialize_forwarding_table (struct CONTROL_PAYLOAD_INIT *payload);
void distance_vector();
void routing_table_update();
void update_packet_table();
void update_forwarding_table();

#endif

