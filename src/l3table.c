#include "l3table.h"
#include <time.h>

static bool BIT(uint8_t bit_n, uint32_t addr){ 
    return (addr & (1 << bit_n));
}

void RNode_printrec(RouteNode *node, uint32_t *prefix, int length){

    uint32_t newprefix = *prefix;
    if(node->subnet_end){
        char addr_str[INET_ADDRSTRLEN] = {0};
        char gateway_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;

        //convert address and gateway to presentation format
        iaddr.s_addr = htonl(*prefix);
        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        iaddr.s_addr = htonl(node->gateway);
        inet_ntop(AF_INET, &iaddr, gateway_str, INET_ADDRSTRLEN);

        printf("%s/%d  |%10s/%d\n", addr_str, length,
                gateway_str,
                node->gw_mask_len);
    }
    for(int i = 0; i < BIT_TYPES; i++){   
        if(node->child[i] != NULL){
            newprefix |= (i << (31-length));
            RNode_printrec(node->child[i], &newprefix, length +1);
        }
    }
}

void RNode_print(RouteNode *root){
    printf("%-20s%-20s\n", "SUBNET", "GATEWAY");
    if(root == NULL){
        printf("Empty trie\n");
        return;
    }
    uint32_t prefix = 0;
    RNode_printrec(root, &prefix, 0);
}

RouteNode *RNode_create(void){
    RouteNode *node = (RouteNode*) calloc(sizeof(RouteNode), 1); 
    return node;
}


void RNode_insert(RouteNode **root, uint32_t addr, int mask_len, uint32_t gateway, uint8_t gw_mask_len){
    //TODO: parse address to 4 bytes
    if(*root == NULL){
        *root = RNode_create();
    }

    RouteNode *p = *root;
    for(int i = 31; i >= (32-mask_len); i--){           //for each bit of address
        if(p->child[BIT(i,addr)] == NULL){              //no node for the char?
            p->child[BIT(i,addr)] = RNode_create();      //create node 
        }
        p = p->child[BIT(i,addr)];                   //go to new node
    }
    p->subnet_end = true;                              //mark end of string
    p->gateway = gateway;
    p->gw_mask_len = gw_mask_len;
}

void RNode_retrieve(RouteNode *root, uint32_t addr, int mask_len, struct RN_addr_in *addr_in){
    //will be filled during the traversal
    struct RN_addr_in tmp_addr_in;
    memset(&tmp_addr_in, 0, sizeof(struct RN_addr_in));

    ////will be filled only when a subned_end flag is found
    struct RN_addr_in ret_addr_in;
    memset(&ret_addr_in, 0, sizeof(struct RN_addr_in));

    RouteNode *p = root;
    for(int i = 31; i >= (32-mask_len); i--){   //for each char in str, traverse
        if(p->subnet_end){
            memcpy(&ret_addr_in, &tmp_addr_in, sizeof(struct RN_addr_in));
        }
        if(p->child[BIT(i,addr)] == NULL){   //and check if its present
            memcpy(addr_in, &ret_addr_in, sizeof(struct RN_addr_in));
            return;
        }

        tmp_addr_in.mask_len ++;
        tmp_addr_in.addr |= (BIT(i,addr) << i);
        p = p->child[BIT(i,addr)];
        tmp_addr_in.gateway = p->gateway;
        tmp_addr_in.gw_mask_len = p->gw_mask_len;
    }
    //if traversal is complete and there is an end flag
    if(p->subnet_end){
        tmp_addr_in.gateway = p->gateway;
        tmp_addr_in.gw_mask_len = p->gw_mask_len;
        memcpy(&ret_addr_in, &tmp_addr_in, sizeof(struct RN_addr_in));
    }
    memcpy(addr_in, &ret_addr_in, sizeof(struct RN_addr_in));
}

static bool RNode_hasChidren(RouteNode *node, BIT_TYPE exclude){
    //check if node has other children besides 'exclude'
    for(BIT_TYPE i = 0; i < BIT_TYPES; i++){
        if(i == exclude){
            continue;
        }
        if(node->child[i]){
            return true;
        }
    }

    return false;
}

void RNode_delete(RouteNode **root, uint32_t addr, uint8_t mask_len){
                        
    //Traverse and find lowest last node that cant be deleted
    RouteNode *p = *root;
    RouteNode *last = NULL;             //delete every node after this
    int tmp_i = 0;
    for(int i = 31; i >= (32-mask_len); i--){
        bool isEnd = p->subnet_end;                        
        bool hasChildren = RNode_hasChidren(p, BIT(i,addr));   
        bool isCurrent = i == (32-mask_len);            //end of (soon to be) deleted string
        if(hasChildren || (isEnd && !isCurrent)){       //is terminal or has children
            if(isCurrent && hasChildren){   
                //special case where nothing is deleted, just the end flag cleared
                p->subnet_end = 0;
                return;
            }
            last = p;
            tmp_i = i;
        }
        p = p->child[BIT(i,addr)];                   //go to new node
    }


    //clear pointer to deleted nodes in 'last'
    p = last? last : *root;                             //if no previous node with children
                                                        //or terminal is found, delete from root
    RouteNode *tmp = p->child[BIT(tmp_i, addr)];
    p->child[BIT(tmp_i, addr)] = NULL;
    p = tmp;
    tmp_i --;


    //delete every node after last
    for(int i = tmp_i; i >= (32-mask_len); i--){    
        tmp = p->child[BIT(i, addr)];
        free(p);
        p = tmp;
    }
}


bool RNode_search(RouteNode *root, uint32_t addr, uint8_t mask_len){

    RouteNode *p = root;
    for(int i = 31; i >= (32-mask_len); i--){   //for each char in str, traverse
        if(p->child[BIT(i,addr)] == NULL){   //and check if its present
            return false;
        }
        p = p->child[BIT(i,addr)];
    }

    //if traversal is complete and there is an end flag, return true
    return p->subnet_end;      
}
