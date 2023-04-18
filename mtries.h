#ifndef MTRIES_H
#define MTRIES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <arpa/inet.h>

#define BIT_TYPE_MAX 3

typedef enum{
    BIT_0,
    BIT_1,
    BIT_X
}BIT_TYPE;

//IPV4 route
typedef struct route_node{
    uint32_t address;           //only leaf nodes carry the address
    //uint32_t wildcard;
    uint32_t prefix;
    uint16_t prefix_len;
    //struct route_node *parent; 
    struct route_node *child[BIT_TYPE_MAX]; 
    //void *data;
}RouteNode;


RouteNode *RNode_create(void);

void RNode_printrec(RouteNode *node);

void RNode_print(RouteNode *root);

void RNode_split(RouteNode *node, uint32_t newaddr, uint8_t mask_len, int i_diff_bit);

void RNode_insert(RouteNode **root, uint32_t addr, uint8_t mask_len);

void RNode_delete(RouteNode **root, uint32_t addr);

RouteNode *RNode_retrieve(RouteNode **root, uint32_t addr, uint8_t mask_len);

RouteNode *RNode_search(RouteNode **root, uint32_t addr);

#endif
