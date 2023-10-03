//Search keys in the Bplus Tree 

#include <iostream>

#include "BplusTree.h"

using namespace std; 

BTreeNode* Btree::search(BTreeNode* node, int key, bool rangeflag, int key2){

    //Tree is empty
    if(node == nullptr) {
        cout<<"B+ Tree is empty\n";
        return nullptr;
    }

    //Traverse internal nodes 
    if(!node->isleaf){
        for(int i=0 ; i<node->size;++i){
            if (key < node->keys[i]) {
                return search(node->children[i], key, rangeflag, key2);
            }
            
            //reach the end of the keys in the node 
            if (key == node->size - 1) {
                return search(node->children[i+1], key, rangeflag, key2);
            }
        }        
    }

    else { //leaf node 
        for (int i = 0; i < node->size; ++i) {

            //Key found in leaf node and not a range query
            if (!rangeflag && node->keys[i] == key) {
                cout<<"Key Found!\n";
                return node; //Needs changing
            }

            //Range query
            if (rangeflag && node->keys[i] >= key && node->keys[i] <= key2){
                cout<<"Keys Found!\n";
                return node; //Needs changing
            }
        }
    }

    return nullptr;
};

 

