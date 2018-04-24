#ifndef CONTROL_HANDLER_LIB_H_
#define CONTROL_HANDLER_LIB_H_

#define CNTRL_HEADER_SIZE 8
#define CNTRL_RESP_HEADER_SIZE 8

#define PACKET_USING_STRUCT // Comment this out to use alternate packet crafting technique

#ifdef PACKET_USING_STRUCT
    struct __attribute__((__packed__)) CONTROL_HEADER
    {
        uint32_t dest_ip_addr;
        uint8_t control_code;
        uint8_t response_time;
        uint16_t payload_len;
    };

    struct __attribute__((__packed__)) CONTROL_RESPONSE_HEADER
    {
        uint32_t controller_ip_addr;
        uint8_t control_code;
        uint8_t response_code;
        uint16_t payload_len;
    };

   /*   struct __attribute__((__packed__)) router_details
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

  */

      struct __attribute__((__packed__)) router_update_details
      {
         
	uint32_t router_u_ip_add;
	uint16_t router_u_port;
	uint16_t padding_u;
	uint16_t router_u_ID;
        uint16_t cost_u;

        };


  struct __attribute__((__packed__)) ROUTING_UPDATE_PKT
  {
	uint16_t num_of_update_fields;
	uint16_t source_router_port;
	uint32_t source_router_ip_add;
     
	struct router_update_details router_u[5];

   };


  struct __attribute__((__packed__)) Forwarding_Table

   {
      uint16_t Frouter_ID;
      uint16_t Fpadding;
      uint16_t next_hop;
      uint16_t Fcost;

    };


 struct __attribute__((__packed__)) router_table 
  {
    int ID;
    uint16_t source_router_id;
    uint32_t source_router_ip_address;
    uint16_t source_router_port; 
    
    uint16_t router_ID;
    uint16_t router_port;
    uint16_t data_port; 
    uint16_t link_cost;
    uint32_t router_ip_add;
    uint16_t next_hop;
    uint16_t padding;

    bool isneighbor;
        
   };

#endif


char* create_response_header(int sock_index, uint8_t control_code, uint8_t response_code, uint16_t payload_len);

#endif
