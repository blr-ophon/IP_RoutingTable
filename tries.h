#ifndef TRIES_H
#define TRIES_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define ASCII_TOTAL 128


typedef struct trieNode{
    struct trieNode *children[ASCII_TOTAL];
    bool str_end;
}TrieNode;


TrieNode *TN_create(void);

bool TN_search(TrieNode *root, char *str);

void TN_insert(TrieNode **root, char *str);

void TN_delete(TrieNode **root, char *str);

void TN_printrec(TrieNode *node, unsigned char *prefix, int length);

void TN_print(TrieNode *root);

#endif
