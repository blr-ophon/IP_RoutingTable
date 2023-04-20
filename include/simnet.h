#ifndef SIMNET_H
#define SIMNET_H

//every node already knows the complete graph from the LSDB

#include <stdint.h>
#include <stdlib.h>
#include "l3table.h"

#define ROUTER_TOTAL 5

//Edges of an adjacency list
typedef struct route_link{
    struct route_link *next;    //next neighbor in list
    int node_id;                //position in the list array
    size_t weight;              //weight to reach neighbor
    uint32_t ipv4;              //In this mini simulation, every 
                                //router already know it's neighbors
                                //IPv4 and MAC address in it's ARP
                                //cache.
}RouteLink;

struct Router{
    //int id;                     //position in the list array                    
    RouteNode *Routing_table;   
    RouteLink *neighbors;       //adjacency list of neighbors
};


void Simnet_init(struct Router *routers);

void Simnet_createGraph(struct Router *routers);

void Router_insertDEdge(struct Router *routers, int id1, int id2);

void Router_insertNeighbor(RouteLink **neighborList, int id);

void Router_printNeighbors(RouteLink *sPtr);

//Router will be stored in an array, where id is it's position, to
//facilitate.
//
//The ideal would be a hash table, where id can be any number, like MAC
//address

void fill_all_tables(void);
//Each node knows all of the topology from LSDB.
//call fill_table for each node

void fill_table(struct Router *Router);
//calls dijkstra for all the nodes in the topology
//fiils routing table with the proper gateways

void shortest_path(void);
//dijkstra algorithm 

void bfs(void);
//breadth-first search for dijkstra

void graph_init(void);
//insert all neighbors in adjacency list
//call graph print in dotgen
























void lsp_exchange(void);
//once the graph is initialized, every node will have a basic routing
//table consisting solely of their neighbors (from assumed ARP cache).
//For each neighbor of node_id, send routing table to neighbor
//for(int i = 0; i < 8; i ++){
//  for(p = router.neighbors; p != NULL; p = p->next){
//      send_rtable(i, p->id); 
//  }
//}

void send_rtable(int src_id, int dst_id);
//assert dst_id is in src_id neighbor list
//get routers[src_id].Routing_table and update...
//...routers[dst_id].Routing_table based on that
//for(each node in src_id table){
//  gateway = src_id.address;
//  insert(dst_id table, address, mask_len, gateway);
//  TODO: what about multiple gateways for an address?
//}


#endif
