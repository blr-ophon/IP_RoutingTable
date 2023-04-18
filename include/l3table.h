#ifndef L3TABLE_H
#define L3TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <arpa/inet.h>

#define BIT_TYPES 2

typedef enum{
    BIT_0,
    BIT_1,
}BIT_TYPE;

struct RN_addr_in{
    uint32_t addr;
    uint8_t mask_len;
};

typedef struct route_node{
    struct route_node *child[BIT_TYPES]; 
    bool subnet_end;    //marks end of known subnet
    uint32_t gateway;   //stores gateway for know subnet 
}RouteNode;


RouteNode *RNode_create(void);

void RNode_insert(RouteNode **root, uint32_t addr, int mask_len);

//void RNode_delete(RouteNode **root, char *subnet);
void RNode_delete(RouteNode **root, uint32_t addr, uint8_t mask_len);

void RNode_retrieve(RouteNode *root, uint32_t addr, int mask_len, struct RN_addr_in *addr_in);

//void RNode_printrec(RouteNode *node, unsigned char *prefix, int length);
void RNode_printrec(RouteNode *node, uint32_t *prefix, int length);

void RNode_print(RouteNode *root);

#endif
