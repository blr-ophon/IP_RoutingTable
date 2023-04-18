#include "mtries.h"

//TODO: delete function

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
    RNode_insert(&root, 0x20000000, 32);
    RNode_insert(&root, 0x30000000, 32);
    //RNode_insert(&root, 0x30000000, 32);
    //RNode_insert(&root, 0x40000000, 32);
    //RNode_insert(&root, 0xe0000000, 32);
    //RNode_insert(&root, 0xe1000000, 7);
    //RNode_insert(&root, 0xe0000000, 32);
    RNode_print(root);


    RouteNode *retrieved = RNode_retrieve(&root, 0x2f000000, 32);
    printf("%x/%d retrieved\n", retrieved->prefix, retrieved->prefix_len);
    //TODO: free trie
    return 0;
}

RouteNode *RNode_create(void){
    RouteNode *node = (RouteNode*) calloc(sizeof(RouteNode), 1); 
    return node;
}

void RNode_printrec(RouteNode *node){
    //print address
    if(node->child[BIT_0] == NULL && node->child[BIT_1] == NULL){  //leaf node
        char addr_str[INET_ADDRSTRLEN] = {0};
        struct in_addr iaddr;
        iaddr.s_addr = htonl(node->address);

        inet_ntop(AF_INET, &iaddr, addr_str, INET_ADDRSTRLEN);
        //printf("ADDRESS: %s/%d\n", addr_str, node->prefix_len);
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
void RNode_split(RouteNode *node, uint32_t newaddr, uint8_t mask_len, int i_diff_bit){
    printf("LEAF NODE: %x/%d\n", node->address, node->prefix_len);
    printf("INSERTING: %x/%d\n", newaddr, mask_len);
    bool diff_bit = BIT(newaddr, i_diff_bit);


    //child node keeping previous info
    RouteNode *keepingNode = RNode_create();
    memcpy(keepingNode, node, sizeof(RouteNode));
    node->child[!diff_bit] = keepingNode;

    //Update node info
    node->prefix_len = 32 - (i_diff_bit+1);
    node->prefix = mask_addr(node->address, node->prefix_len);
    node->address = 0;

    //child node with new info
    RouteNode *newNode = RNode_create();
    node->child[diff_bit] = newNode; 
    newNode->address = newaddr;
    newNode->prefix_len = mask_len;            //leafs have full prefix len
    newNode->prefix = mask_addr(newaddr, mask_len);           //TODO: mask prefix

    printf("BECOMES:\n%x/%d: (0)%x/%d | (1)%x/%d\n\n",
            node->prefix,
            node->prefix_len,
            node->child[BIT_0]->address,
            node->child[BIT_0]->prefix_len,
            node->child[BIT_1]->address,
            node->child[BIT_1]->prefix_len
            );
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
        (*root)->child[BIT_0]->prefix_len = 1;

        (*root)->child[BIT_1] = RNode_create(); 
        (*root)->child[BIT_1]->address = 0xffffffff;    
        (*root)->child[BIT_1]->prefix = 0xffffffff;
        (*root)->child[BIT_1]->prefix_len = 1;
    }

    RouteNode *p = *root;
    for(int i = 31; i >= 0;){
        //number of bits to be checked before advancing to next node.
        //n = (current_bit) - (last bit to check) + 1(including last bit to check)
        int prefix_bits = i-(32 - p->prefix_len) + 1;       
                                                           
        for(int j = 0; j < prefix_bits; j++){
            //look all bits in the prefix for a node
            if(BIT(addr,i) != BIT(p->prefix,i)){
                //split node and make parent point to it
                RNode_split(p, addr, mask_len, i);
                return;
            }
            i--;
        }
        //go to next node
        if(p->child[BIT(addr,i)] == NULL){
            //subnet has higher mask than deepest leaf. 
            RNode_split(p, addr, mask_len, i);
            return;
        }
        p = p->child[BIT(addr,i)];
    }
}

//TODO: i is screwing everything up probably. Either that or check the tree structure to see if everything was inserted
//correctly
RouteNode *RNode_retrieve(RouteNode **root, uint32_t addr, uint8_t mask_len){
    printf("\nRETRIEVING FOR: %.8x/%d\n", addr, mask_len);
    addr = mask_addr(addr, mask_len);


    RouteNode *p = *root;
    if(p == NULL) return NULL;

    for(int i = 31; i >= 0;){
        RouteNode *node = p->child[BIT(addr,i)];
        if(node == NULL) return p;  //return if leaf node
        if(node->prefix == mask_addr(addr, node->prefix_len)){
            i -= node->prefix_len;
            p = node;
        }else return p; //return when a mismatch occurs
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
