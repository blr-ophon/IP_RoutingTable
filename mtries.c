#include "mtries.h"

//TODO: insert prefixes etc
//free memory

static bool BIT(uint32_t addr, uint8_t bit_n){ 
    return (addr & (1 << bit_n));
}

int main(void){
    RouteNode *root = NULL;
    RNode_insert(&root, 0x210f10f1);
    RNode_insert(&root, 0x31020304);
    RNode_insert(&root, 0xea0fa0fa);
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

void RNode_split(RouteNode *node, uint32_t newaddr, int i_diff_bit){
    bool diff_bit = BIT(newaddr, i_diff_bit);

    //previous node 
    node->child[!diff_bit] = RNode_create();
    node->child[!diff_bit]->address = node->address;    //pass address to child
    node->child[!diff_bit]->prefix_len = node->prefix_len;            //leafs have full prefix len
    node->child[!diff_bit]->prefix = node->address;     //TODO: mask prefix
                                                        
    //parent node 
    node->prefix_len = 32 - (i_diff_bit+1);
    node->prefix = node->address;                       //TODO: mask prefix
    node->address = 0;                                  //clear parent address

    //new node
    node->child[diff_bit] = RNode_create();
    node->child[diff_bit]->address = newaddr;
    node->child[!diff_bit]->prefix_len = 32;            //leafs have full prefix len
    node->child[!diff_bit]->prefix = newaddr;           //TODO: mask prefix
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

    RouteNode *p = *root;
    for(int i = 31; i >= 0; i--){
        for(int j = 0; j < i -(31-p->prefix_len); j++){
            //look all bits in the prefix for a node
            if(BIT(addr,i) != BIT(p->prefix,i)){
                RNode_split(p, addr, i);
                return;
            }
            i--;
        }
        //go to next node
        p = p->child[BIT(addr,i)];

        /*
        if(!p->child[BIT(addr, i)]){    //if empty child node
            //create node
            if(p->address == addr){   
                //Already inserted
                break;
            }
            RNode_split(p, addr, BIT(addr,i));
            break;
        }
        */
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
