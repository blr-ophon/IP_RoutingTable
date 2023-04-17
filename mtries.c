#include "mtries.h"

//TODO: insert prefixes etc
//free memory

static bool BIT(uint32_t addr, uint8_t bit_n){ 
    return (addr & (1 << bit_n));
}

static uint32_t mask_addr(uint32_t addr, uint8_t mask_len){ 
    uint32_t mask = 0;
    for(int i = 31; i >= (32-mask_len); i--){
        mask |= (uint32_t)(1 << i);
    }
    return (addr & mask);
}

int main(void){
    RouteNode *root = NULL;
    RNode_insert(&root, 0x2f000000, 6);
    RNode_insert(&root, 0x2f000000, 3);
    RNode_insert(&root, 0x2f000000, 7);
    //RNode_insert(&root, 0x20000000, 32);
    //RNode_insert(&root, 0x30000000, 32);
    //RNode_insert(&root, 0x40000000, 32);
    //RNode_insert(&root, 0xe0000000, 32);
    //RNode_insert(&root, 0xe1000000, 7);
    //RNode_insert(&root, 0xe0000000, 32);
    //RNode_search(&root, 0x20000000);
    RNode_print(root);
    return 0;
}

RouteNode *RNode_create(void){
    RouteNode *node = (RouteNode*) calloc(sizeof(RouteNode), 1); 
    return node;
}

void RNode_printrec(RouteNode *node){
    //print address
    if(node->child[BIT_0] == NULL && node->child[BIT_1] == NULL){  //leaf node
        /*
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(node->address);

        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        printf("ADDRESS: %s\n", addr_str);
        */
        printf("ADDRESS: %x/%d\n", node->address, node->prefix_len);
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
RouteNode *RNode_split(RouteNode *node, uint32_t newaddr, uint8_t mask_len, int i_diff_bit){
    //TODO: use RouteNode **node with void return
    bool diff_bit = BIT(newaddr, i_diff_bit);

    //create new parent with similar info from previous node
    RouteNode *new_parent = RNode_create();
    new_parent->address = 0;
    new_parent->prefix_len = 32 - (i_diff_bit+1);
    new_parent->prefix = mask_addr(node->prefix, new_parent->prefix_len);
    new_parent->child[!diff_bit] = node;        //unchanged child points to previous node
    new_parent->parent = node->parent;          
    node->parent = new_parent;

    //new node
    new_parent->child[diff_bit] = RNode_create();
    new_parent->child[diff_bit]->parent = new_parent;
    new_parent->child[diff_bit]->address = newaddr;
    new_parent->child[diff_bit]->prefix_len = mask_len;            //leafs have full prefix len
    new_parent->child[diff_bit]->prefix = mask_addr(newaddr, mask_len);           //TODO: mask prefix
                                                             
    printf("NODE SPLIT:\n%x/%d: %x | %x\n\n",
            new_parent->prefix,
            new_parent->prefix_len,
            new_parent->child[BIT_0]->address,
            new_parent->child[BIT_1]->address
            );
    return new_parent;
}

void RNode_insert(RouteNode **root, uint32_t addr, uint8_t mask_len){
    //TODO: receive addr as str and convert to network order uint32_t
    //the only difference between this and a normal trie is that is doesnt
    //keep inserting after finding an empty child node, just creates and 
    //breaks loop

    addr = mask_addr(addr, mask_len);       //only the subnet matters for the trie
    if(*root == NULL){
        //root start as leaf with prefix 0, address 0 and 0bit mask
        *root = RNode_create();
        //Create 2 leaf nodes, one starting with 0 and other starting with 1, else the root would
        //split everytime a node starting with 0 would be inserted
        //Both 0.0.0.0 and ff.ff.ff.ff will exist on the routing table, but should be ignored
        (*root)->child[BIT_0] = RNode_create(); 
        (*root)->child[BIT_0]->address = 0x00000000;
        (*root)->child[BIT_0]->prefix = 0x00000000;
        (*root)->child[BIT_0]->prefix_len = 32;
        (*root)->child[BIT_0]->parent = *root;

        (*root)->child[BIT_1] = RNode_create(); 
        (*root)->child[BIT_1]->address = 0xffffffff;    
        (*root)->child[BIT_1]->prefix = 0xffffffff;
        (*root)->child[BIT_1]->prefix_len = 32;
        (*root)->child[BIT_1]->parent = *root;
    }

    //RouteNode *parent = NULL;
    RouteNode *p = *root;
    //for(int i = 31; i >= (32-mask_len);){
    for(int i = 31; i >= 0;){
        //number of bits to be checked before advancing to next node.
        //n = (current_bit) - (last bit to check) + 1(including last bit to check)
        int prefix_bits = i-(32 - p->prefix_len) + 1;       
                                                           
        for(int j = 0; j < prefix_bits; j++){
            //if(i < (32-mask_len)) break;    //break from outer loop
            //look all bits in the prefix for a node
            if(BIT(addr,i) != BIT(p->prefix,i)){
                //split node and make parent point to it
                //if(parent){
                if(p->parent){
                    //i + 2 for previous bit
                    RouteNode *tmp = p->parent;
                    p = RNode_split(p, addr, mask_len, i);
                    tmp->child[BIT(addr,i+2)] = p;
                    //parent->child[BIT(addr,i+1)] = RNode_split(p, addr, mask_len, i);
                    return;
                }else{  //if parent is NULL, p is *root
                    *root = RNode_split(p, addr, mask_len, i);
                }
            }
            i--;
        }
        //go to next node
        //parent = p;
        p = p->child[BIT(addr,i)];
    }
}

//traverse the tree according to address specified until a leaf is found.
//if it breaks before a leaf, it's not in the trie
RouteNode *RNode_search(RouteNode **root, uint32_t addr){

    RouteNode *p = *root;
    for(int i = 31; i >= 0;){
        //number of bits to be checked before advancing to next node.
        //n = (current_bit) - (last bit to check) + 1(including last bit to check)
                                                           
        if(p->prefix_len == 32){
            break;
        }
        int prefix_bits = i-(32 - p->prefix_len) + 1;       
        for(int j = 0; j < prefix_bits; j++){
            //look all bits in the prefix for a node
            if(BIT(addr,i) != BIT(p->prefix,i)){
                //NOT IN THE TRIE
                return NULL;
            }
            i--;
        }
        //go to next node
        p = p->child[BIT(addr,i)];
    }
    if(p->address == addr){ 
        return p;
    }else{
        return NULL;
    }
}

//delete, search
//  delete: there are no single-leafed branches, just delete desired leaf.

//Delete the leaf node and parent, make grandparent point to the other leaf
void RNode_delete(RouteNode **root, uint32_t addr){
}
