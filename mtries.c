#include "mtries.h"

//TODO: insert prefixes etc
//free memory

static bool BIT(uint32_t addr, uint8_t bit_n){ 
    return (addr & (1 << bit_n));
}

int main(void){
    RouteNode *root = NULL;
    RNode_insert(&root, 0x01020304);
    RNode_insert(&root, 0xf10f10f1);
    RNode_insert(&root, 0xfa0fa0fa);
    RNode_insert(&root, 0xff0ff0ff);
    RNode_insert(&root, 0x01000002);
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
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(node->address);

        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        printf("ADDRESS: %s\n", addr_str);
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

void RNode_split(RouteNode *node, uint32_t newaddr, bool diff_bit){
    //previous node
    node->child[!diff_bit] = RNode_create();
    node->child[!diff_bit]->address = node->address;   //pass address to child
    node->address = 0;                              //clear own address

    //new node
    node->child[diff_bit] = RNode_create();
    node->child[diff_bit]->address = newaddr;
}

void RNode_insert(RouteNode **root, uint32_t addr){
    //TODO: receive addr as str and convert to network order uint32_t
    //the only difference between this and a normal trie is that is doesnt
    //keep inserting after finding an empty child node, just creates and 
    //breaks loop

    if(*root == NULL){
        *root = RNode_create();
    }

    RouteNode *p = *root;
    for(int i = 31; i >= 0; i--){
        if(!p->child[BIT(addr, i)]){    //if empty child node
            //create node
            if(p->address == addr){   
                //Already inserted
                break;
            }
            RNode_split(p, addr, BIT(addr,i));
            break;
        }

        //go to next node
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
