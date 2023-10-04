//Search keys in the Bplus Tree 

#include <iostream>

#include "BplusTree.h"

using namespace std; 

void BTree::removeRecord(Key_Records key){

    int keyToFind =  key.key_value;
    removeRecord(key, root);
};

void BTree::removeRecord(Key_Records key, BTreeNode* node){

    int keyToFind =  key.key_value;
    
    if(node->isleaf)
    {
        remove(keyToFind, node);
    }
    else
    {
        for(int i = 0; i<node->numKeysPerNode; i++)
        {
            if(node->keys[i] >= keyToFind)
            removeRecord(key, node->child[i+1]);
            // Condition of unbalanced child
            if(node->child[i]->numKeysPerNode < (this.degree+1)/2)
            {
                if!((i>0 && tryBorrowing(node->child[i-1], node->child[i])) || (i<node->numKeysPerNode && tryBorrowing(node->child[i+1], node->child[i])))
                {
                    if(i>0)
                    {
                        removeFromInternal(mergeTwoNodes(node->child[i-1], node->child[i]), node);
                        treat_underflow(node, this->root);
                        break;
                    }
                    else if (i < node->numKeysPerNode)
                    {
                        removeFromInternal(mergeTwoNodes(node->child[i+1], node->child[i]), node);
                        treat_underflow(node, this->root);
                    }
                
                }              
            }
        }
    }
    
};

void BTree::treat_underflow(BTreeNode* node, BTreeNode* root)
{
    if(node!=root)
    {
        if(node->numKeysPerNode<this->degree/2)
        {
            BTreeNode* node1, node2 = findSiblings(node, root);
            if(tryBorrowing(node, node1) || tryBorrowing(node, node2))
                break;
            else 
                BTreeNode* parentNode = mergeInternalNodes(node1, node, root);
                treat_underflow(parentNode);
        }
    }
    else
    {
        if(node->numKeysPerNode<=1)
        this->root = node;
        free(root);
    }
    
};

BTreeNode* mergeInternalNodes(BTreeNode* node1, BTreeNode* node2, BTreeNode* parent)
{
    if(node1->numKeysPerNode + node2->numKeysPerNode <= this.degree)
    {
        return 0;
    }
    else
    {
        for(int i = 0; i<node2->numKeysPerNode; i++)
        {
            insert(node2->child[i], node2->child[i], node1);
        }
    }
    return max(node1[key[0]],node2[key[0]]);
}

BTreeNode** findSiblings(BTreeNode* node, BTreeNode* root)
{
    BTreeNode* curr;
    BTreeNode* siblings[2];
    curr = root;
    int keyToFind = node->keys[0].key_value;
    bool parent = false;
    while(!parent)
    {
        int i = 0;
        for(i = 0; i < curr->numKeysPerNode; i++)
        {
            
            if(curr->keys[i].key_value >= keyToFind)
                break;
        }
        if(node == curr->child[i])
        {
            parent = true;
            if(i<=0)
            siblings[0] = nullptr;
            else siblings[0] = curr->child[i-1];
            if(i==curr->numKeysPerNode)
            siblings[1] = nullptr;
            else siblings[1] = curr->child[i+1];
        }
        else
        {
            curr = curr->child[i];
        }
    }
    return siblings;
}


bool BTree::tryBorrowing(BTreeNode* node1, BTreeNode* node2)
{
    bool rebalanced = false;
    if(node1->numKeysPerNode < (this.degree+1)/2)
    {
        if(node2->numKeysPerNode <= (this.degree+1)/2)
        rebalanced = false;
        else
        {
            insert(node2->child[0], node2->keys[0], node1);
            remove(node2->keys[0], node2);
            rebalanced = true;
        }

    }
    else 
    {
        if(node1->numKeysPerNode <= (this.degree+1)/2)
        rebalanced = false;
        else
        {
            insert(node1->child[node1->numKeysPerNode-1], node1->keys[node1->numKeysPerNode-1], node2);
            remove(node1->keys[node1->numKeysPerNode-1], node1);
            rebalanced = true;
        }
    }
    return rebalanced;
};

int BTree::mergeTwoNodes(BTreeNode* node1, BTreeNode* node2)
{
    if(node1->numKeysPerNode + node2->numKeysPerNode <= this.degree)
    {
        return 0;
    }
    else
    {
        for(int i = 0; i<node2->numKeysPerNode; i++)
        {
            insert(node2->child[i], node2->child[i], node1);
        }
    }
    return max(node1[key[0]],node2[key[0]]);
}

