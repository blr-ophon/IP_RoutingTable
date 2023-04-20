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
}RouteLink;

struct Router{
    uint32_t ipv4;              //In this mini simulation, every 
                                //router already know it's neighbors
                                //IPv4 and MAC address in it's ARP
                                //cache.
    uint8_t mask_len;
    int id;                     //position in the list array                    
    RouteNode *Routing_table;   
    RouteLink *neighbors;       //adjacency list of neighbors
};



void Simnet_init(struct Router *routers);

void Simnet_createGraph(struct Router *routers);

void Simnet_fillTables(struct Router *routers);

void Router_insertDEdge(struct Router *routers, int id1, int id2, int weight);

void Router_insertNeighbor(RouteLink **neighborList, int id, int weight);

void Router_printNeighbors(RouteLink *sPtr);

//Router will be stored in an array, where id is it's position, to
//facilitate.
//
//The ideal would be a hash table, where id can be any number, like MAC
//address


#endif
