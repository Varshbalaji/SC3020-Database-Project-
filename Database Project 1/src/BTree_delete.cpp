//Search keys in the Bplus Tree 

#include <iostream>
#include <BplusTree.h>
// #include "..\include\BplusTree.h"

using namespace std; 

void Btree::removeRecord(int key){

    removeRecord(key, root);
};

void Btree::removeRecord(int key, BTreeNode* node){

    int keyToFind =  key;
    
    if(node->isleaf)

    {
        remove_key_in_leaf_node(node,keyToFind);  //remove_key_in_leaf_node(BTreeNode* leafNode, int key)
    }

    else
    {
        int i = 0;
        for( i = 0; i < node->numKeysPerNode; i++)
        {
            if(node->keys[i].key_value >= keyToFind)
            removeRecord(key, node->child[i]);
            // Condition of unbalanced child
            if(node->child[i]->numKeysPerNode < (this->deg+1)/2)
            {
                if(!((i>0 && tryBorrowing(node->child[i-1], node->child[i])) || (i<node->numKeysPerNode && tryBorrowing(node->child[i+1], node->child[i]))))
                {
                    if(i > 0)
                    remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i-1], node->child[i]));
                    else if(i < node->numKeysPerNode) // Case for 
                    remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i+1], node->child[i]));
  
                }              
            }
        }
            removeRecord(key, node->child[i+1]);
            if(node->child[i+1]->numKeysPerNode < (this->deg+1)/2)
            {
                if(!((i>0 && tryBorrowing(node->child[i], node->child[i+1])) || (i<node->numKeysPerNode && tryBorrowing(node->child[i+1], node->child[i]))))
                {
                    if(i > 0)
                    remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i-1], node->child[i]));
                    else if(i < node->numKeysPerNode) // Case for 
                    remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i+1], node->child[i]));
  
                }              
            }

    }
    
};


