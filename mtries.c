#include "mtries.h"

//TODO: insert prefixes etc
//free memory

static bool BIT(uint32_t addr, uint8_t bit_n){ 
    return (addr & (1 << bit_n));
}

int main(void){
    RouteNode *root = NULL;
    RNode_insert(&root, 0x20000000);
    RNode_insert(&root, 0x30000000);
    RNode_insert(&root, 0xe0000000);
    //RNode_insert(&root, 0x01000002);
    RNode_print(root);
    return 0;
}

RouteNode *RNode_create(void){
    RouteNode *node = (RouteNode*) calloc(sizeof(RouteNode), 1); 
    return node;
}

void RNode_printrec(RouteNode *node){
    //print address
    if(node->address){  //leaf node
        /*
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(node->address);

        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        printf("ADDRESS: %s\n", addr_str);
        */
        printf("ADDRESS: %x\n", node->address);
        return;
    }
    for(int i = 0; i < 2; i++){   //bit 0 and 1
        if(node->child[i] != NULL){
            RNode_printrec(node->child[i]);
        }
    }
}

void RNode_print(RouteNode *root){
    if(root == NULL){
        printf("Empty trie\n");
        return;
    }
    RNode_printrec(root);
}

//split node into 2 and returns address of new parent node
RouteNode *RNode_split(RouteNode *node, uint32_t newaddr, int i_diff_bit){
    //TODO: use RouteNode **node with void return
    bool diff_bit = BIT(newaddr, i_diff_bit);

    //create new parent with similar info from previous node
    RouteNode *new_parent = RNode_create();
    new_parent->address = 0;
    new_parent->prefix = node->prefix;
    new_parent->prefix_len = 32 - (i_diff_bit+1);
    new_parent->child[!diff_bit] = node;        //unchanged child points to previous node

    //new node
    new_parent->child[diff_bit] = RNode_create();
    new_parent->child[diff_bit]->address = newaddr;
    new_parent->child[diff_bit]->prefix_len = 32;            //leafs have full prefix len
    new_parent->child[diff_bit]->prefix = newaddr;           //TODO: mask prefix
                                                             
    return new_parent;
}

void RNode_insert(RouteNode **root, uint32_t addr){
    //TODO: receive addr as str and convert to network order uint32_t
    //the only difference between this and a normal trie is that is doesnt
    //keep inserting after finding an empty child node, just creates and 
    //breaks loop

    if(*root == NULL){
        //root start as leaf with prefix 0, address 0 and 32bit mask
        *root = RNode_create();
        (*root)->prefix_len = 32;   
    }

    RouteNode *parent = NULL;
    RouteNode *p = *root;
    for(int i = 31; i >= 0;){
        //number of bits to be checked before advancing to next node.
        //n = (current_bit) - (last bit to check) + 1(including last bit to check)
        int prefix_bits = i-(32 - p->prefix_len) + 1;       
                                                           
        for(int j = 0; j < prefix_bits; j++){
            //look all bits in the prefix for a node
            if(BIT(addr,i) != BIT(p->prefix,i)){
                //split node and make parent point to it
                if(parent){
                    //i + 1 for previous bit
                    parent->child[BIT(addr,i+1)] = RNode_split(p, addr, i);
                    return;
                }else{  //if parent is NULL, p is *root
                    *root = RNode_split(p, addr, i);
                }
            }
            i--;
        }
        //go to next node
        parent = p;
        p = p->child[BIT(addr,i)];
    }
}

//delete, search
//  delete: there are no single-leafed branches, just delete desired leaf.
//  search: traverse the tree according to address specified until a leaf is found.
//          if it breaks before a leaf, it's not in the trie

void RNode_delete(RouteNode **root, uint32_t addr){

    RouteNode *parent = NULL;
    RouteNode *p = *root;
    for(int i = 31; i >= 0; i--){
        if(!p->child[BIT(addr, i)]){    //if empty child node
            //create node
            if(p->address == addr && parent){   
                //keep other node in parent
                parent->address = p->address;

                //delete both leafs
                free(parent->child[BIT(addr,i)]);
                free(parent->child[!BIT(addr,i)]);
                parent->child[BIT(addr,i)] = NULL;
                parent->child[!BIT(addr,i)] = NULL;
            } //else, node not present
            break;
        }

        //go to next node
        parent = p;
        p = p->child[BIT(addr,i)];
    }

}
