#include "tries.h"

int main(void){
    TrieNode *root = NULL;
    TN_insert(&root, "zaio");
    TN_insert(&root, "caio");
    TN_insert(&root, "carro");
    TN_insert(&root, "carron");
    TN_insert(&root, "corno");
    TN_delete(&root, "carron");
    TN_insert(&root, "internet");
    TN_insert(&root, "bosta");
    TN_print(root);
    return 0;
}

TrieNode *TN_create(void){
    TrieNode *node = (TrieNode*) calloc(sizeof(TrieNode), 1); 
    return node;
}

bool TN_search(TrieNode *root, char *str){
    uint8_t *cstr = (uint8_t*) str;    //casted str

    TrieNode *p = root;
    for(int i = 0; i < strlen(str); i++){   //for each char in str, traverse
        if(p->children[cstr[i]] == NULL){   //and check if its present
            return false;
        }
        p = p->children[cstr[i]];
    }

    //if traversal is complete and there is an end flag, return true
    return p->str_end;      
}

void TN_insert(TrieNode **root, char *str){
    uint8_t *cstr = (uint8_t*) str;    //casted str
    if(*root == NULL){
        *root = TN_create();
    }

    TrieNode *p = *root;
    for(int i = 0; i < strlen(str); i++){           //for each char of string
        if(p->children[cstr[i]] == NULL){           //no node for the char?
            p->children[cstr[i]] = TN_create();     //create node 
        }
        p = p->children[cstr[i]];                   //go to new node
    }
    p->str_end = true;                              //mark end of string
}

static bool TN_hasChidren(TrieNode *node, uint8_t exclude){
    //check if node has other children besides 'exclude'
    for(int i = 0; i < ASCII_TOTAL; i++){
        if(i == exclude){
            continue;
        }
        if(node->children[i]){
            return true;
        }
    }

    return false;
}

void TN_delete(TrieNode **root, char *str){
    uint8_t *cstr = (uint8_t*) str;    //casted str
    TrieNode *last = NULL;             //delete every node after this
                        
    //find 'last' node to keep
    TrieNode *p = *root;
    int tmp_i = 0;
    for(int i = 0; i <= strlen(str); i++){
        bool isEnd = p->str_end;                        
        bool hasChildren = TN_hasChidren(p, cstr[i]);   
        bool isCurrent = i == strlen(str);          //end of deleted string
        if(hasChildren || (isEnd && !isCurrent)){  //is terminal os has children
            if(isEnd && hasChildren){   //special case where nothing is deleted
                p->str_end = 0;
                return;
            }
            last = p;
            tmp_i = i;
        }
        p = p->children[cstr[i]];                   //go to new node
    }

    //clear pointer to deleted nodes in 'last'
    p = last? last : *root;
    TrieNode *tmp = p->children[cstr[tmp_i]];
    p->children[cstr[tmp_i]] = NULL;
    p = tmp;
    tmp_i ++;

    //delete every node after last
    for(int i = tmp_i; i <= strlen(str); i++){    
        tmp = p->children[cstr[i]];
        free(p);
        p = tmp;
    }
}


void TN_printrec(TrieNode *node, unsigned char *prefix, int length){
    uint8_t newprefix[length+2];            //length + next char + \0
    memcpy(newprefix, prefix, length);
    newprefix[length+1] = 0;                //\0

    if(node->str_end){
        printf("WORD: %s\n", prefix);
    }
    for(int i = 0; i < ASCII_TOTAL; i++){   //all ASCII chars
        if(node->children[i] != NULL){
            newprefix[length] = i;          //next char
            TN_printrec(node->children[i], newprefix, length +1);
        }
    }
}

void TN_print(TrieNode *root){
    if(root == NULL){
        printf("Empty trie\n");
        return;
    }
    TN_printrec(root, NULL, 0);
}

