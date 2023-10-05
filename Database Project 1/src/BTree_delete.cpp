//Search keys in the Bplus Tree 

#include <iostream>

#include "BplusTree.h"

using namespace std; 

void Btree::removeRecord(Key_Records key){

    int keyToFind =  key.key_value;
    removeRecord(key, root);
};

void Btree::removeRecord(Key_Records key, BTreeNode* node){

    int keyToFind =  key.key_value;
    
    if(node->isleaf)

    {
        remove_key_in_leaf_node(node,keyToFind);  //remove_key_in_leaf_node(BTreeNode* leafNode, int key)
    }

    else
    {
        for(int i = 0; i < node->numKeysPerNode; i++)
        {
            if(node->keys[i].key_value >= keyToFind)
            removeRecord(key, node->child[i+1]);
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
    }
    
};



bool Btree::tryBorrowing(BTreeNode* node1, BTreeNode* node2)
{
    bool rebalanced = false;
    if(node1->numKeysPerNode < (this->deg+1)/2)
    {
        if(node2->numKeysPerNode <= (this->deg+1)/2)
        rebalanced = false;
        else
        {
            if(node1->isleaf)
            {
                insert_in_leaf_node(node1, node2->keys[0].key_value, &(node2->keys[0].storage_array));
                remove_key_in_leaf_node(node2, node2->keys[0].key_value);
            }
            else
            {
                insert_ChildNode_in_ParentNode(node1, node2->child[0], node2->keys[0].key_value);
                remove_key_in_internal_node(node2, node2->keys[0].key_value);
            }
            rebalanced = true;
        }

    }
    else 
    {
        if(node1->numKeysPerNode <= (this->deg+1)/2)
        rebalanced = false;
        else
        {
            if(node1->isleaf)
            {
                insert_in_leaf_node(node2,  node1->keys[node1->numKeysPerNode-1].key_value, &(node1->keys[node1->numKeysPerNode-1].storage_array));
                remove_key_in_leaf_node(node1, node1->keys[node1->numKeysPerNode-1].key_value);
            }
            else
            {
                insert_ChildNode_in_ParentNode(node2,  node1->child[node1->numKeysPerNode-1], node1->keys[node1->numKeysPerNode-1].key_value);
                remove_key_in_internal_node(node1,  node1->keys[node1->numKeysPerNode-1].key_value);
            }
            rebalanced = true;
        }
    }
    return rebalanced;
};

int Btree::mergeTwoNodes(BTreeNode* node1, BTreeNode* node2)
{
    if(node1->numKeysPerNode + node2->numKeysPerNode <= this->deg)
        return 0;
    else
        for(int i = 0; i<node2->numKeysPerNode; i++)
        insert_ChildNode_in_ParentNode(node1,node2->child[i], node2->keys[i].key_value); //void insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, int key);
    
    return max(node1->keys[0].key_value,node2->keys[0].key_value);
};

