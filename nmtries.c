#include "nmtries.h"

static bool BIT(uint8_t bit_n, uint32_t addr){ 
    return (addr & (1 << bit_n));
}

int main(void){
    RouteNode *root = NULL;
    RNode_insert(&root, 0x2f000000, 6);
    RNode_insert(&root, 0x2f000000, 3);
    RNode_insert(&root, 0x2f000000, 7);
    RNode_insert(&root, 0x20000000, 32);
    RNode_insert(&root, 0x30000000, 32);
    RNode_print(root);

    struct RN_addr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    RNode_retrieve(root, 0x2f000000, 7, &addr_in);
    printf("Retrieved: %x/%d\n", addr_in.addr, addr_in.mask_len);
    return 0;
}

void RNode_printrec(RouteNode *node, uint32_t *prefix, int length){
    //uint8_t newprefix[length+2];            //length + next char + \0
    //memcpy(newprefix, prefix, length);
    //newprefix[length+1] = 0;                //\0

    if(node->subnet_end){
        printf("Subnet: %x/%d\n", *prefix, length);
    }
    for(int i = 0; i < BIT_TYPES; i++){   
        if(node->child[i] != NULL){
            //newprefix[length] = i + '0';          //next char
            //RNode_printrec(node->child[i], newprefix, length +1);
            (*prefix) |= (i << (31-length));
            RNode_printrec(node->child[i], prefix, length +1);
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


//void RNode_insert(RouteNode **root, char *subnet, int mask_len){
void RNode_insert(RouteNode **root, uint32_t addr, int mask_len){
    //TODO: parse address to 4 bytes
    //TODO: insert gateway
    //uint32_t addr = atoi(subnet);
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

//static bool RNode_hasChidren(RouteNode *node, uint8_t exclude){
//    //check if node has other children besides 'exclude'
//    for(int i = 0; i < ASCII_TOTAL; i++){
//        if(i == exclude){
//            continue;
//        }
//        if(node->children[i]){
//            return true;
//        }
//    }
//
//    return false;
//}
//
//void RNode_delete(RouteNode **root, char *str){
//    uint8_t *cstr = (uint8_t*) str;    //casted str
//    RouteNode *last = NULL;             //delete every node after this
//                        
//    //find 'last' node to keep
//    RouteNode *p = *root;
//    int tmp_i = 0;
//    for(int i = 0; i <= strlen(str); i++){
//        bool isEnd = p->str_end;                        
//        bool hasChildren = RNode_hasChidren(p, cstr[i]);   
//        bool isCurrent = i == strlen(str);          //end of deleted string
//        if(hasChildren || (isEnd && !isCurrent)){  //is terminal os has children
//            if(isEnd && hasChildren){   //special case where nothing is deleted
//                p->str_end = 0;
//                return;
//            }
//            last = p;
//            tmp_i = i;
//        }
//        p = p->children[cstr[i]];                   //go to new node
//    }
//
//    //clear pointer to deleted nodes in 'last'
//    p = last? last : *root;
//    RouteNode *tmp = p->children[cstr[tmp_i]];
//    p->children[cstr[tmp_i]] = NULL;
//    p = tmp;
//    tmp_i ++;
//
//    //delete every node after last
//    for(int i = tmp_i; i <= strlen(str); i++){    
//        tmp = p->children[cstr[i]];
//        free(p);
//        p = tmp;
//    }
//}


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
