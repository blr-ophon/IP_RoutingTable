#include "l3table.h"
#include <time.h>

static bool BIT(uint8_t bit_n, uint32_t addr){ 
    return (addr & (1 << bit_n));
}

static void insert_random_addrs(RouteNode **root, size_t n){
    //used for testing
    srand(time(NULL));
    for(size_t i = 0; i < n; i++){
        RNode_insert(root, (uint32_t)rand(), rand()%32);
    }
}

int main(void){
    RouteNode *root = NULL;
    insert_random_addrs(&root, 128);
    RNode_insert(&root, 0xff000000, 4);
    RNode_delete(&root, 0xff000000, 4);
    RNode_print(root);

    struct RN_addr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    RNode_retrieve(root, 0x2f000000, 7, &addr_in);
    printf("Retrieved: %x/%d\n", addr_in.addr, addr_in.mask_len);
    return 0;
}


void RNode_printrec(RouteNode *node, uint32_t *prefix, int length){

    uint32_t newprefix = *prefix;
    if(node->subnet_end){
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(*prefix);

        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        printf("-- %s/%d (%#x)\n", addr_str, length, newprefix);
        //printf("Subnet: %x/%d\n", newprefix, length);
    }
    for(int i = 0; i < BIT_TYPES; i++){   
        if(node->child[i] != NULL){
            newprefix |= (i << (31-length));
            RNode_printrec(node->child[i], &newprefix, length +1);
        }
    }
}

void RNode_print(RouteNode *root){
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


void RNode_insert(RouteNode **root, uint32_t addr, int mask_len){
    //TODO: parse address to 4 bytes
    //TODO: insert gateway
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
}

void RNode_retrieve(RouteNode *root, uint32_t addr, int mask_len, struct RN_addr_in *addr_in){
    //will be filled during the traversal
    uint32_t tmp_addr = 0;
    int tmp_mask_len = 0;

    //will be filled only when a subned_end flag is found
    uint32_t ret_addr = 0;
    int ret_mask_len = 0;

    RouteNode *p = root;
    for(int i = 31; i >= (32-mask_len); i--){   //for each char in str, traverse
        if(p->subnet_end){
            ret_addr = tmp_addr;
            ret_mask_len = tmp_mask_len;
        }
        if(p->child[BIT(i,addr)] == NULL){   //and check if its present
            addr_in->addr = ret_addr;
            addr_in->mask_len= ret_mask_len;
            return;
        }
        tmp_mask_len++;
        tmp_addr |= (BIT(i,addr) << i);
        p = p->child[BIT(i,addr)];
    }

    //if traversal is complete and there is an end flag
    if(p->subnet_end){
        ret_addr = tmp_addr;
        ret_mask_len = tmp_mask_len;
    }
    addr_in->addr = ret_addr;
    addr_in->mask_len= ret_mask_len;
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


//bool RNode_search(RouteNode *root, char *str){
//    uint8_t *cstr = (uint8_t*) str;    //casted str
//
//    RouteNode *p = root;
//    for(int i = 0; i < strlen(str); i++){   //for each char in str, traverse
//        if(p->children[cstr[i]] == NULL){   //and check if its present
//            return false;
//        }
//        p = p->children[cstr[i]];
//    }
//
//    //if traversal is complete and there is an end flag, return true
//    return p->str_end;      
//}
