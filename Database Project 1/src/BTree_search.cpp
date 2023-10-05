//Search keys in the Bplus Tree 

#include <iostream>

#include "BplusTree.h"

using namespace std; 

 std::vector<Key_Records> Btree::search(BTreeNode* node, int key, bool rangeflag, int key2){

    std::vector<Key_Records> search_result = {};

    //Tree is empty
    if(node == nullptr) {
        cout<<"B+ Tree is empty\n";
        return;
    }

    //Traverse internal nodes 
    if(!node->isleaf){
        for(int i=0 ; i<node->numKeysPerNode;++i){
            if (key < node->keys->key_value) {
                return search(node->child[i], key, rangeflag, key2);
            }
            
            //reach the end of the keys in the node 
            if (key == node->numKeysPerNode - 1) {
                return search(node->child[i+1], key, rangeflag, key2);
            }
        }        
    }

    else { //leaf node 

        //If not a range query
        if(!rangeflag){
            for (int i = 0; i < node->numKeysPerNode; ++i) {
                if (node->keys->key_value == key) {
                cout<<"Key Found!\n";
                search_result.push_back(node->keys[i]);
                return search_result; 
                }
            }
        }
        
        //Range query
        else{
            for (int i = 0; i < node->numKeysPerNode; ++i) {
                if(node->keys->key_value >= key && node->keys->key_value <= key2){
                    search_result.push_back(node->keys[i]);

                    if(i==node->numKeysPerNode-1){
                        node = node->child[i];
                        i = 0;
                    }
                }
            }
            return search_result;
        }
    }
    return search_result;
};

 

