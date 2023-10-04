#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "../include/BplusTree.h"


using namespace std;


void Btree::insert(Key_Records key){

    // if there is a root node
    if(root == nullptr){
        cout << "B+ Tree Creation In Progress.....\n" << "\n";
        root = new BTreeNode(deg);
        root->keys[0] = key; //inserting 
        root->numKeysPerNode = 1;
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
            if (key.key_value > current->keys[current->numKeysPerNode-1].key_value){
                current = current->child[current->numKeysPerNode]; // go to the next node
            }
            
            else{
                for(int i=0;i<current->numKeysPerNode;i++){
                    if(key.key_value < current->keys->key_value){
                        current = current->child[i];
                        break;
                    }
                }
            }
        }

        //Once the leaf node is reached first check if the key is a duplicate. If yes, update the storage array of the existing key and return
        for (int i=0 ; i<current->numKeysPerNode;i++){
            if(current->keys[i].key_value == key.key_value){
                current->keys[i].storage_array.insert(current->keys[i].storage_array.end(),key.storage_array.begin(),key.storage_array.end());
                return;
            }
        }

        //If there is a space in the leaf node, insert key accordingly
        if(current->numKeysPerNode < deg){
            int i = 0;
            while(key.key_value > current->keys[i].key_value && i<current->numKeysPerNode) //find the last key that is smaller than the one being inserted
                i++; 
            for(int j = current->numKeysPerNode; j>i ; j--){ // shift the keys after i to the right by one to create space for insertion
                current->keys[j] = current->keys[j-1];
            }

            current->keys[i] = key; //insert the record
            current->numKeysPerNode++; //update the number of keys in current node
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
            for(j = deg + 1; j>1 ; j--){   // shift the keys after i to the right by one to create space for insertion
                virtualNode[j] = virtualNode[j-1];
            }

            virtualNode[i] = key; //insert the record
            newLeafNode->isleaf = true;

            //split the current node into the two leaf nodes
            current->numKeysPerNode = (deg+1)/2;
            newLeafNode->numKeysPerNode = deg+1 - (deg+1)/2;

            //Update pointers
            current->child[current->numKeysPerNode] = newLeafNode; // Make the last key of the current node point to the new leaf node
            newLeafNode->child[newLeafNode->numKeysPerNode] = current->child[deg]; 
            current->child[deg] = NULL;

            //Distribute records from the virtualNode between the 2 leaf nodes
            for(i = 0; i < current->numKeysPerNode; i++){
                    current->keys[i] = virtualNode[i];
            }

            for(i = 0, j = current->numKeysPerNode; i < newLeafNode->numKeysPerNode; i++, j++){
                    newLeafNode->keys[i] = virtualNode[j];
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
            }
            else{ //insert new parent 
                insertParent(newLeafNode->keys[0],parent,newLeafNode);
            }
        }  
    }
};

//Helper Functions

//Updates the parent node with the newly created child nodes and adds a parent if needed 
void Btree::insertparent(Key_Records key, BTreeNode *cursor, BTreeNode *child){

}

//Find parent 
