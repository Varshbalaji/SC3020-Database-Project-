/**
 * @file BTree_insert.cpp
 * @brief Implementation of the Btree class.
 */


#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include<queue>
#include<tuple>
#include "../include/BplusTree.h"

using namespace std;

int recordCount=0;



/**
 * @brief Removes a record with the specified key from the B+ tree.
 * @param key The key of the record to be removed.
 */
void Btree::removeRecord(float key){
    removeRecord(key, root);
    int xyz = fixKeys(root);
}

/**
 * @brief Fixes the keys in the B+ tree after a record removal.
 * @param node The node to fix the keys for.
 * @return The new key value for the node.
 */

float Btree::fixKeys(BTreeNode *node)// Convert to float for actual experiment
{
    if(node->isleaf){
    return node->keys[0].key_value;
    }
    for(int i=0; i<=node->numKeysPerNode; i++)
    {
        node->keys[i-1].key_value = fixKeys(node->child[i]);
    }
    return fixKeys(node->child[0]);
}

/**
 * @brief Handles underflow in a B+ tree node.
 * @param node The node to treat for underflow.
 */

void Btree::treat_underflow(BTreeNode* node)
{
    BTreeNode* parent;
    int x;
    if(node == root)
    {
        if(node->numKeysPerNode<1)
        {
            if(node->child[0]==nullptr)
            root = node->child[1];
            else
            root = node->child[0];
            nodeCount--;

        }
    }
    else
    if(!node->isleaf && node->numKeysPerNode<(this->deg)/2)
    {
        pair<BTreeNode*, BTreeNode*> siblings = findAdjacentSiblings(root,node);
        if(!( (tryBorrowing(siblings.first, node)) 
        ||      (tryBorrowing(node,siblings.second)) ))
        {
            {   
                findParentAndIndex(root, root, nullptr, node, parent, x);
                if(siblings.first != nullptr)
                {
                    int oldKey = mergeInternalNodes(siblings.first, node);

                    insertKey_Useless(node, parent->keys[x].key_value);
                    updateParentKey2(parent, parent->keys[x].key_value, oldKey);
                    remove_key_in_internal_node(parent,oldKey);
                }
                else if(siblings.second != nullptr)
                {
                    int oldKey = mergeInternalNodes(node, siblings.second);

                    insertKey_Useless(node, parent->keys[x].key_value);
                    updateParentKey2(parent, parent->keys[x].key_value, oldKey);
                    remove_key_in_internal_node(parent, oldKey);
                }

                treat_underflow(parent);
            }
        }

    }
}

/**
 * @brief Inserts a key with a pointer to a vector of RecordAddresses in a leaf node.
 * @param leafNode The leaf node to insert the key into.
 * @param key The key to insert.
 * @param addressVector Pointer to a vector of RecordAddresses associated with the key.
 */

void  Btree::insert_in_leaf_node(BTreeNode* leafNode, float key, vector<RecordAddress>* addressVector) {
    int insertIndex = -1; // Index to insert the new key

    // Find the correct index to insert the new key
    for (int i = 0; i <= leafNode->numKeysPerNode; ++i) {

        if (key < leafNode->keys[i].key_value || leafNode->numKeysPerNode<=i) {

            insertIndex = i;
            break;
        }
    }

    // Shift existing keys and address vectors to the right to make space for the new key
    for (int i = leafNode->numKeysPerNode - 1; i >= insertIndex; --i) {
        leafNode->keys[i + 1] = leafNode->keys[i];
    }

    // Insert the new key and associated pointer to the vector
    leafNode->keys[insertIndex].key_value = key;
    leafNode->keys[insertIndex].storage_array = *addressVector;
    leafNode->numKeysPerNode++;
}


/**
 * @brief Helper function to update the key in the parent node when a child node borrows
 * 
 * @param parent The parent node containing the key to be updated
 * @param oldKey The old key value to be replaced
 * @param newKey The new key value to replace the old key
 */

void Btree::updateParentKey2(BTreeNode* parent, float oldKey, float newKey) {
    for (int i = 0; i < parent->numKeysPerNode; ++i) {
        if (parent->keys[i].key_value == oldKey) {
            parent->keys[i].key_value = newKey;
            break;
        }
    }
}

/**
 * @brief Attempt to borrow a key from one node to another
 * 
 * @param node1 The first node (either leaf or internal)
 * @param node2 The second node (either leaf or internal)
 * @return true if a key was successfully borrowed, false otherwise
 */

bool Btree::tryBorrowing(BTreeNode* node1, BTreeNode* node2) {
    if(!node1 || !node2)
    return false;
    if(!node1->isleaf)
    return tryBorrowingInternal(node1, node2);
    bool rebalanced = false;
    if (node1->numKeysPerNode < (this->deg + 1) / 2) {
        if (node2->numKeysPerNode <= (this->deg + 1) / 2){
            rebalanced = false;}
        else {
            if (node1->isleaf) {
                insert_in_leaf_node(node1, node2->keys[0].key_value, &(node2->keys[0].storage_array));
                float oldKey = node2->keys[0].key_value;
                remove_key_in_leaf_node(node2, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node2, parent, index);
                updateParentKey2(parent, oldKey, parent->keys[index].key_value);
            } else {
                insert_ChildNode_in_ParentNode(node1, node2->child[0], node2->keys[0].key_value);
                float oldKey = node2->keys[0].key_value;
                remove_key_in_internal_node(node2, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node2, parent, index);
                updateParentKey2(parent, oldKey, parent->keys[index].key_value);
            }
            rebalanced = true;
        }
    } else {
        if (node1->numKeysPerNode <= (this->deg + 1) / 2)
            rebalanced = false;
        else {
           {
                insert_in_leaf_node(node2, node1->keys[node1->numKeysPerNode - 1].key_value,
                                    &(node1->keys[node1->numKeysPerNode - 1].storage_array));
                int oldKey = node1->keys[node1->numKeysPerNode - 1].key_value;
                remove_key_in_leaf_node(node1, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node1, parent, index);
                updateParentKey2(parent, oldKey, node1->keys[node1->numKeysPerNode - 1].key_value);
            } 
            rebalanced = true;
        }
    }
    return rebalanced;
}


/**
 * @brief Attempt to borrow a key from one internal node to another internal node
 * 
 * @param node1 The first internal node
 * @param node2 The second internal node
 * @return true if a key was successfully borrowed, false otherwise
 */

bool Btree::tryBorrowingInternal(BTreeNode* node1, BTreeNode* node2) {
    bool rebalanced = false;
    if(node1==0 || node2==0)
    return false;
    if ((node1->numKeysPerNode < (this->deg) / 2 && node2->numKeysPerNode <= (this->deg ) / 2) 
    || (node2->numKeysPerNode < (this->deg) / 2 && node1->numKeysPerNode <= (this->deg ) / 2))
            rebalanced = false;
        else 
        {
            if(node2->numKeysPerNode>this->deg/2)
            {
                insert_ChildNode_in_ParentNode(node1, node2->child[0], node2->keys[0].key_value);
                int oldKey = node2->keys[0].key_value;
                remove_key_in_internal_node(node2, oldKey);
                
                BTreeNode* parent = nullptr;
                int index = -1;

                findParentAndIndex(root, root, nullptr, node2, parent, index);
                int siblingKey = parent->keys[index].key_value;
                updateParentKey2(parent, parent->keys[0].key_value, oldKey);
                node2->keys[0].key_value = siblingKey;
                rebalanced = true;
            }
            if(node1->numKeysPerNode>this->deg/2)
            {
         
                insert_ChildNode_in_ParentNode(node2, node1->child[node1->numKeysPerNode], node1->keys[node1->numKeysPerNode-1].key_value);
                int oldKey = node1->keys[node1->numKeysPerNode-1].key_value;
                remove_key_in_internal_node(node1, oldKey);
                BTreeNode* parent = nullptr;
                int index;

                findParentAndIndex(root, root, nullptr, node1, parent, index);
                int siblingKey = parent->keys[index].key_value;
                updateParentKey2(parent, parent->keys[index].key_value, oldKey);
                node2->keys[0].key_value = siblingKey;
                rebalanced = true;
            
        
            }
        
    
    }
    return rebalanced;
}


/**
 * @brief Helper function to remove a key and its associated vector of RecordAddresses from a leaf node
 * 
 * @param leafNode The leaf node from which the key and associated data should be removed
 * @param key The key value to be removed
 */

void  Btree::remove_key_in_leaf_node(BTreeNode* leafNode, float key) {
    int keyIndex = -1; // Index of the key to be removed

    // Find the index of the key to be removed
    for (int i = 0; i < leafNode->numKeysPerNode; ++i) {
        if (leafNode->keys[i].key_value == key) {
            keyIndex = i;
            break;
        }
    }

    // If the key was not found, return (key not present in the node)
    if (keyIndex == -1) {
        return;
    }

    // Clear the associated vector of RecordAddresses
    leafNode->keys[keyIndex].storage_array.clear();

    // Shift keys to the left to fill the gap
    for (int i = keyIndex; i < leafNode->numKeysPerNode - 1; ++i) {
        leafNode->keys[i] = leafNode->keys[i + 1];
    }

    // Decrease the number of keys in the node
    leafNode->numKeysPerNode--;
}


/**
 * @brief Helper function to remove a key and its associated child pointer from an internal node
 * 
 * @param internalNode The internal node from which the key and child pointer should be removed
 * @param key The key value to be removed
 */

void  Btree::remove_key_in_internal_node(BTreeNode* internalNode, float key) {
    int keyIndex = 0; // Index of the key to be removed , -1 to act as a flag 

    // Find the index of the key to be removed
    while(keyIndex<internalNode->numKeysPerNode && internalNode->keys[keyIndex].key_value < key)
    keyIndex++;

    // If the key was not found, return (key not present in the node)
    if (keyIndex == -1) {
        return;
    }

    // Shift keys and child pointers to the left to fill the gaps
    internalNode->child[keyIndex] = internalNode->child[keyIndex+1];

    for (int i = keyIndex; i < internalNode->numKeysPerNode - 1; ++i) {
        internalNode->keys[i] = internalNode->keys[i + 1];
        internalNode->child[i + 1] = internalNode->child[i + 2];
    }

    // Decrease the number of keys and child pointers in the node
    internalNode->numKeysPerNode--;
}


/**
 * @brief Helper function to insert a child node (can be non-leaf/leaf) to a parent node (non-leaf node)
 * 
 * @param parent The parent node to which the child node should be inserted
 * @param child The child node to be inserted
 * @param key The key associated with the child node
 */

void  Btree::insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, float key) {
    if (parent == nullptr || child == nullptr || parent->isleaf) {
        // Invalid input or parent is a leaf node
        return;
    }

    int insertIndex = -1; // Index to insert the new child node
    int i;
    // Find the correct index to insert the new child node based on the key
    for ( i = 0; i < parent->numKeysPerNode; ++i) {
        if (key < parent->keys[i].key_value ) {
            insertIndex = i;
            break;
        }
    }
    insertIndex = i;
    // Shift existing child nodes and keys to the right to make space for the new child
    for ( i = insertIndex; i < parent->numKeysPerNode-1; i++) {
        parent->child[i+1] = parent->child[i];
        parent->keys[i+1] = parent->keys[i];
    }
    parent->child[i+1] = parent->child[i];
    

    // Insert the new child node and respective key
    parent->child[insertIndex + 1] = child;
    parent->keys[insertIndex].key_value = key;
    parent->numKeysPerNode++;   

    
}


/**
 * @brief Helper function to find the parent and index of a given node within a B-tree
 * 
 * @param root The root node of the B-tree
 * @param current The current node being examined during the recursive search
 * @param prev The parent node of the current node (initially nullptr)
 * @param nodeToFind The node for which the parent and index need to be found
 * @param parent A reference to a pointer to the parent node
 * @param index A reference to an integer to store the index of the node within its parent's children
 */

void  Btree::findParentAndIndex(BTreeNode* root, BTreeNode* current, BTreeNode* prev, BTreeNode* nodeToFind, BTreeNode*& parent, int& index) {
    if (!current) {
        return;
    }

    if (current == nodeToFind) {
        // 'current' is the node we're looking for
        // 'prev' is its parent
        parent = prev; // 'prev' is the parent
        for (int i = 0; i <= parent->numKeysPerNode; ++i) {
            if (parent->child[i] == nodeToFind) {
                index = i;
                break;
            }
        }
        return;
    }

    // Recursively search in child nodes
    for (int i = 0; i <= current->numKeysPerNode; ++i) {
        findParentAndIndex(root, current->child[i], current, nodeToFind, parent, index);
    }
}

/**
 * @brief Helper function to find adjacent siblings of a given node within a B-tree
 * 
 * @param root The root node of the B-tree
 * @param node The node for which adjacent siblings need to be found
 * @return A pair of pointers to the left and right siblings, respectively
 */

std::pair<BTreeNode*, BTreeNode*>  Btree::findAdjacentSiblings(BTreeNode* root, BTreeNode* node) {
    BTreeNode* leftSibling = nullptr;
    BTreeNode* rightSibling = nullptr;

    if (root == nullptr || node == nullptr || node->isleaf) {
        // Invalid input or the node is a leaf node
        return std::make_pair(leftSibling, rightSibling);
    }

    BTreeNode* parent = nullptr;
    int index = -1;

    // Start the search from the root
    findParentAndIndex(root, root, nullptr, node, parent, index);

    if (parent != nullptr) {
        if (index > 0) {
            // 'node' has a left sibling
            leftSibling = parent->child[index - 1];
        }

        if (index < parent->numKeysPerNode) {
            // 'node' has a right sibling
            rightSibling = parent->child[index + 1];
        }
    }

    return std::make_pair(leftSibling, rightSibling);
}




/**
 * @brief Function to remove a record with a given key from the B-tree
 * 
 * @param key The key value associated with the record to be removed
 * @param node The current node being examined (initially the root)
 */

void Btree::removeRecord(float key, BTreeNode* node){

    float keyToFind =  key;
    
    if(node->isleaf)
    remove_key_in_leaf_node(node,keyToFind);  //remove_key_in_leaf_node(BTreeNode* leafNode, int key)
    else
    {
        int i = 0;
        for(i = 0; i <node->numKeysPerNode; i++)
        {
    
            
            if(node->keys[i].key_value > keyToFind)
            {   
                removeRecord(key, node->child[i]);
                
                // Condition of unbalanced child
                if((node->child[i]->numKeysPerNode < (this->deg+1)/2    && node->child[i]->isleaf)
                || (node->child[i]->numKeysPerNode < (this->deg)/2      && !(node->child[i]->isleaf)))
                    {
                        if(!((i>0 && tryBorrowing(node->child[i-1], node->child[i])) || 
                        (i<node->numKeysPerNode && tryBorrowing(node->child[i],node->child[i+1]))))
                        
                        {
                            if(i > 0)
                            {
                                
                                remove_key_in_internal_node(node, mergeTwoNodes(node->child[i-1], node->child[i]));
                            }
                            else if(i < node->numKeysPerNode) // Case for 
                            {
                                ;
                                remove_key_in_internal_node(node, mergeTwoNodes( node->child[i], node->child[i+1]));

                            }
                            treat_underflow(node);
                        }
                    }
                return;
               
            }
        }
        i = node->numKeysPerNode;
        removeRecord(key, node->child[i]);
         if((node->child[i]->numKeysPerNode < (this->deg+1)/2    && node->child[i]->isleaf)
        || (node->child[i]->numKeysPerNode < (this->deg)/2      && !(node->child[i]->isleaf)))
        {
            if(!((i>0 && tryBorrowing(node->child[i], node->child[i+1])) 
            || (i<node->numKeysPerNode && tryBorrowing(node->child[i+1], node->child[i]))))
            {
                if(i > 0)
                remove_key_in_internal_node(node, mergeTwoNodes(node->child[i-1], node->child[i]));
                else if(i < node->numKeysPerNode)
                remove_key_in_internal_node(node, mergeTwoNodes(node->child[i], node->child[i+1]));
                treat_underflow(node);
            }              
        }

    }
    
}



/**
 * @brief Function to merge two nodes into one node
 * 
 * @param node1 The first node to merge
 * @param node2 The second node to merge
 * @return The key value that triggered the merge
 */

float Btree::mergeTwoNodes(BTreeNode* node1, BTreeNode* node2)
{
    if(node1->numKeysPerNode + node2->numKeysPerNode > this->deg)
        return 0;
    else
        for(int i = 0; i<node2->numKeysPerNode && !node2->isleaf; i++)
            {
                insert_ChildNode_in_ParentNode(node1, node2->child[i], node2->keys[i].key_value);
                if(i == node2->numKeysPerNode-1)
                {
                    node1->child[node1->numKeysPerNode + node2->numKeysPerNode + 1] = node2->child[node2->numKeysPerNode];
                    node1->numKeysPerNode++;
                    
                }
                node2->numKeysPerNode--;
            } 
        for(int i = 0; i<node2->numKeysPerNode && node2->isleaf; i++)
            {
                insert_in_leaf_node(node1, node2->keys[i].key_value, &(node2->keys[i].storage_array));
                // node2->numKeysPerNode--;
            }
            int deleting_key = max(node1->keys[0].key_value,node2->keys[0].key_value);
        // free(node2);
    nodeCount--;
    return deleting_key;
}


/**
 * @brief Helper function to insert a key into a node (used when node is already full)
 * 
 * @param node The node to which the key should be inserted
 * @param key The key to insert
 */

void Btree::insertKey_Useless(BTreeNode* node, float key)
{
    int i = 0;
    while(key > node->keys[i].key_value)
    i++;
    for(int j = i+1; j <= node->numKeysPerNode; j++)
        node->keys[j].key_value = node->keys[j-1].key_value;
    node->keys[i].key_value = key;
    node->numKeysPerNode++;
}


/**
 * @brief Merge two internal nodes into one internal node
 * 
 * @param node1 The first internal node to merge
 * @param node2 The second internal node to merge
 * @return The key value that triggered the merge
 */


float Btree::mergeInternalNodes(BTreeNode* node1, BTreeNode* node2)
{
        int i = 0;
        int j;
        for(i = node1->numKeysPerNode, j = 0; j < node2->numKeysPerNode+1; i++, j++)
                node1->child[i+1] = node2->child[j];  
        for(i = node1->numKeysPerNode, j = 0; j < node2->numKeysPerNode; i++, j++, node1->numKeysPerNode++)
                node1->keys[i] = node2->keys[j];   
    nodeCount--;
    return max(node1->keys[0].key_value,node2->keys[0].key_value);
}




/**
 * @brief Insert a new key with an associated record address into the B-tree
 * 
 * @param keyValue The key value to insert
 * @param recordAddress The record address associated with the key
 */

void Btree::insert(float keyValue, RecordAddress recordAddress){


    Key_Records key = {keyValue};
    // if there is a root node
    if(root == nullptr){
        cout << "B+ Tree Creation In Progress.....\n" << "\n";
        root = new BTreeNode(deg);
        root->keys[0] = key ; //inserting 
        key.storage_array.push_back(recordAddress);
        root->numKeysPerNode = 1;
        root->isleaf = true;
        nodeCount++; 
        return;
    }

    else{
        BTreeNode *current = root;
        BTreeNode *parent;
        parent = new BTreeNode(deg); //why is this here??

        // where to store Key in leaf node 
        while (current ->isleaf == false){
            parent = current;
            // the key to be inserted is larger than all keys in the internal node  
            if (key.key_value >= current->keys[current->numKeysPerNode-1].key_value){
                current = current->child[current->numKeysPerNode]; // go to the next node
            }
            
            else{
                for(int i=0;i<current->numKeysPerNode;i++){
                    if(key.key_value < current->keys[i].key_value){
                        current = current->child[i];
                        break;
                    }
                }
            }
        }
        //Once the leaf node is reached first check if the key is a duplicate. If yes, update the storage array of the existing key and return
        for (int i=0 ; i<current->numKeysPerNode;i++){
            if(current->keys[i].key_value == key.key_value){
                current->keys[i].storage_array.push_back(recordAddress);
                return;
            }
        }
        key.storage_array.push_back(recordAddress);  //Update the storage array for the correct leaf node 
        //If there is a space in the leaf node, insert key accordingly
        if(current->numKeysPerNode < deg){
            int i = 0;
            while(key.key_value > current->keys[i].key_value && i<current->numKeysPerNode) //find the last key that is smaller than the one being inserted
                i++; 
            for(int j = current->numKeysPerNode; j>i ; j--){ // shift the keys after i to the right by one to create space for insertion
                current->keys[j] = current->keys[j-1];
            }
            current->keys[i] = key; //insert the record  
            // cout <<"Test" <<current->keys[i].key_value <<"\n"; 
            current->numKeysPerNode++; //update the number of keys in current node
            current->child[current->numKeysPerNode]=current->child[current->numKeysPerNode-1]; //change pointer of old last position to new last position
            current->child[current->numKeysPerNode-1]=nullptr;
            return;
        }

        else{ //If there is no space in the leaf node, split the node
            BTreeNode* newLeafNode = new BTreeNode(deg);
            nodeCount++;
            Key_Records virtualNode[deg+1]; //the virtualNode has space for n+1 keys to find the right insertion place 

            //copy keys to virtual node
            for(int i = 0; i<deg ; i++){
                virtualNode[i] = current->keys[i];
            }

            int i = 0, j;
            while(key.key_value > virtualNode[i].key_value && i<deg) //find the last key that is smaller than the one being inserted
                i++;
            for(j = deg ; j>i ; j--){   // shift the keys after i to the right by one to create space for insertion
                virtualNode[j] = virtualNode[j-1];
            }

            virtualNode[i] = key; //insert the record
            newLeafNode->isleaf = true;

            //split the current node into the two leaf nodes
            current->numKeysPerNode = (deg+1)/2;
            newLeafNode->numKeysPerNode = (deg +1) - (deg+1)/2;

            //Update pointers
            current->child[current->numKeysPerNode] = newLeafNode; // Make the last key of the current node point to the new leaf node
            newLeafNode->child[newLeafNode->numKeysPerNode] = current->child[deg]; 
            current->child[deg] = NULL;

            //Distribute records from the virtualNode between the 2 leaf nodes
            // cout<<"split 1"<<"\n";
            for(i = 0; i < current->numKeysPerNode; i++){
                    current->keys[i] = virtualNode[i];
                    // cout << current->keys[i].key_value<<"\n";
            }

            // cout<<"split 2"<<"\n";
            for(i = 0, j = current->numKeysPerNode; i < newLeafNode->numKeysPerNode; i++, j++){
                    newLeafNode->keys[i] = virtualNode[j];
                    // cout << newLeafNode->keys[i].key_value << "\n";

            }

            //create a new root if the current node is the root
            if(current == root){
                BTreeNode* newRoot = new BTreeNode(deg);
                nodeCount++;
                newRoot->keys[0] = newLeafNode->keys[0]; //check if it should be the whole key struct or just key value
                newRoot->child[0] = current;
                newRoot->child[1] = newLeafNode;
                newRoot->isleaf = false;
                newRoot->numKeysPerNode = 1;
                root = newRoot;
                cout <<"New root created"<<"\n";
            }
            else{ //insert new parent 
                insertParent(newLeafNode->keys[0],parent,newLeafNode);
            }
        } 
    }
}




/**
 * @brief Helper function to update the parent node with newly created child nodes and add a parent if needed
 * 
 * @param key The key value to insert into the parent node
 * @param current The current node that needs to be updated
 * @param child The new child node to be added
 */

void Btree::insertParent(Key_Records key, BTreeNode *current, BTreeNode *child){

    if(current->numKeysPerNode < deg){ //If the internal node is not full
        int i = 0;
        while(key.key_value > current->keys[i].key_value && i < current->numKeysPerNode){ //find the last key that is smaller than the one being inserted
            i++;
        }
        for(int j = current->numKeysPerNode; j>i; j--){
            current->keys[j] = current->keys[j-1];
            current->child[j+1] = current->child[j];
        }
        // cout<<"Adding to same parent"<<"\n";
        current->keys[i] = key; //insert the key
        current->numKeysPerNode++; //increase current number of keys in the node
        // cout << "Testing Internal" << current->keys[i].key_value <<"\n";
        current->child[i+1] = child; //point the i+1th pointer to the newLeafNode (child)
        // cout<< "child: "<<  current->child[i+1];
    }

    else { //if the parent node is full, split the node and create a new internal node
        BTreeNode *newParent = new BTreeNode(deg);
        nodeCount++;
        Key_Records virtualKeys[deg+1];
        BTreeNode *virtualChildren[deg+2];
        
        for (int keyIndex = 0; keyIndex < deg; keyIndex++) {
            virtualKeys[keyIndex] = current->keys[keyIndex];     //copy the keys 
            virtualChildren[keyIndex] = current->child[keyIndex];
        }
        virtualChildren[deg] = current->child[deg];

        
        int i=0, j;
        while((key.key_value > virtualKeys[i].key_value) && i<deg){
                i++;
        }
        for (int j = deg; j>i;j--){
            virtualKeys[j] = virtualKeys[j-1];
            virtualChildren[j+1] = virtualChildren[j];

        }
        virtualKeys[i] = key; //insert key in the correct position of the virtual array of keys 
        virtualChildren[i+1] = child;
        newParent->isleaf = false;

        current ->numKeysPerNode = (deg +1)/2; //number of keys in the current node 
        newParent->numKeysPerNode = deg - (deg +1)/2; // number of keys in the new internal node 

        // cout<<"Parent split 1"<<"\n";
        for (i=0;i<current->numKeysPerNode;i++){
            current->keys[i] = virtualKeys[i];
            current->child[i] = virtualChildren[i];
        }
        current->child[current->numKeysPerNode] = virtualChildren[i];

        // cout<<"Parent split 2"<<"\n";
        j = current->numKeysPerNode+1;
        for(i=0;i<newParent->numKeysPerNode;i++){
            newParent->keys[i] = virtualKeys[j];
            newParent->child[i] = virtualChildren[j];
            j++;
        }
        newParent->child[newParent->numKeysPerNode] = virtualChildren[j];

        // for(i=0;i<current->numKeysPerNode+1;i++){
        //     current->keys[i] = NULL;
        //     current->child[i+1] = nullptr;
        // }
       
        if (current == root) { // if we reach the root 
                BTreeNode *newRoot = new BTreeNode(deg);
                nodeCount++;
                // smallest right subtree value (ss) 
                Key_Records ss_value = fetchSSKey(newParent);
                newRoot->keys[0] = ss_value; // add the smallest value in the right subtree to the root 
                newRoot->child[0] = current;
                newRoot->child[1] = newParent;
                newRoot->isleaf = false;
                newRoot->numKeysPerNode = 1;
                root = newRoot;
                cout<<"New Parent Root"<<"\n";
        }
        else{
            Key_Records ss_value = fetchSSKey(newParent);
            insertParent(ss_value, findParent(root, current), newParent); //recursively insert parent 
        }

    }
    return;

}



/**
 * @brief Helper function to find the parent of a child node within the B-tree
 * 
 * @param current The current node being examined (initially the root)
 * @param child The child node for which to find the parent
 * @return A pointer to the parent node
 */

BTreeNode *Btree::findParent( BTreeNode *current, BTreeNode *child){
    cout<<"Entered findParent"<<"\n";
    BTreeNode *parent;
    if (current->isleaf || current->child[0]->isleaf){
        return nullptr;
    }
    for (int i=0; i < current->numKeysPerNode+1 ; i++){
        if (current->child[i] == child) {
            parent = current;
            return parent;
        }       
        else {
            parent = findParent(current->child[i], child);
                if (parent != nullptr)
                    return parent;
        }
    }
    cout<<"Exiting findParent"<<"\n";
    return parent;

}


/**
 * @brief Find the smallest key value in the right subtree starting from the given node.
 * 
 * @param current The current node to start the search from
 * @return The smallest key value found in the right subtree
 */

Key_Records Btree::fetchSSKey(BTreeNode *current){
    
    // Find the smallest on the right subtree
    while (!current->isleaf){
        current = current->child[0];
    }
    return current->keys[0];

}



/**
 * @brief Print the keys in the B-tree in ascending order.
 * 
 * @param current The current node to start printing from
 */

void Btree::printTree(BTreeNode *current)
{

  if (current != NULL) {
    for (int i = 0; i < current->numKeysPerNode; i++) {
        float value  = current->keys[i].key_value;
        cout << value <<" ";
    }
    cout << "\n";
    cout << "\n";
    if (current->isleaf != true) {  
      for (int i = 0; i < current->numKeysPerNode + 1; i++) {
        printTree(current->child[i]);
      }
    }
  }

}


/**
 * @brief Fetch the root node of the B-tree.
 * 
 * @return A pointer to the root node of the B-tree
 */

BTreeNode *Btree::fetchRoot(){
    return root;
}




/**
 * @brief Search for key(s) in the B-tree.
 * 
 * @param key The key value to search for
 * @param rangeflag Set to true for range query, false for single key query
 * @param key2 The second key value (used in range query)
 * @return A vector of Key_Records that match the search criteria
 */


std::vector<Key_Records> Btree::search(float key, bool rangeflag, float key2){

    std::vector<Key_Records> search_result;

    //Tree is empty
    if(root == nullptr) {
        cout<<"B+ Tree is empty\n";
        return {};
    }

    else{
        BTreeNode* current = root;

        while (current ->isleaf == false){
            indexNodesAccessed++;
            // the key to be inserted is larger than all keys in the internal node  
            if (key >= current->keys[current->numKeysPerNode-1].key_value){
                current = current->child[current->numKeysPerNode]; // go to the next node
            }
            
            else{
                for(int i=0;i<current->numKeysPerNode;i++){
                    if(key < current->keys[i].key_value){
                        current = current->child[i];
                        break;
                    }
                }
            }
        }

        //leaf node 
        if(!rangeflag){ //Single key query
            indexNodesAccessed++;
            for(int i = 0; i<current->numKeysPerNode; i++){
                if (current->keys[i].key_value == key) {
                    // cout<<"Key Found!\n";
                    search_result.push_back(current->keys[i]);
                    // cout <<"Nodes Accessed:"<<nodesAccessed<<"\n";
                    return search_result;
                }
            }
            // cout <<"Key Not Found :("<<"\n";
            //     return search_result;
        }

        else{ //Range query
            indexNodesAccessed++;
            for (int i = 0; i < current->numKeysPerNode+1; i++) {
                
                if(i==current->numKeysPerNode){
                        current = current->child[i];
                        indexNodesAccessed++;
                        i = 0;
                }

                if(current->keys[i].key_value >= key && current->keys[i].key_value <= key2){
                    search_result.push_back(current->keys[i]);
                
                }

                if(current->child[current->numKeysPerNode]==nullptr && i==current->numKeysPerNode-1)
                    break;

                if(current->keys[i].key_value>=key2 )
                    break;

            }
            return search_result;
        }
    }
    return {};
}


/**
 * @brief Calculate the number of levels in the B-tree.
 * 
 * @param current The current node to start counting levels from
 * @return The number of levels in the B-tree
 */

int Btree::getLevels(BTreeNode* current){
    if(current->isleaf == false)
        return getLevels(current->child[0])+1;
    
    else if(current->isleaf == true)
        return 1;
    
    else
        return 0;

}



/**
 * @brief Find the smallest leaf node in the right subtree starting from the given node.
 * 
 * @param current The current node to start the search from
 * @return A pointer to the smallest leaf node in the right subtree
 */

BTreeNode* Btree::fetchSmallestLeaf(BTreeNode *current){
    
    // Find the smallest on the right subtree
    while (!current->isleaf){
        current = current->child[0];
    }
    return current;

}



/**
 * @brief Calculate the average FG3_PCT_home value for records associated with search results.
 * 
 * @param diskStorage A pointer to the disk storage
 * @param search_result The search results containing keys and associated record addresses
 * @return A tuple containing the average FG3_PCT_home value and the number of records used in the calculation
 */

tuple<float, int> Btree::avgFG3_PCT_home( Storage *diskStorage, vector<Key_Records> search_result){
    int recordCount = 0;
    float sum = 0;
    float avg = 0;
    Record *dataRecord;
    int numberOfDataBlocksAccessed;
    for(int i=0;i<search_result.size();i++){
        Key_Records key = search_result[i];
        for(int j=0;j<key.storage_array.size();j++){
            RecordAddress recordAddress= {key.storage_array[j].blockNumber, key.storage_array[j].offset};
            tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord(recordAddress, sizeof(Record));
            
            if (dataRecord != nullptr){
                sum += dataRecord->FG3_PCT_home;
                recordCount++;
                dataBocksAccessed += numberOfDataBlocksAccessed;
            }
        }
    }
    avg = sum/recordCount;
    return {avg, recordCount};
}

