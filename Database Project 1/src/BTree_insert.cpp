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
        parent = new BTreeNode(deg);

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

    }

        //Call search function to check for duplicates 

        //Else find the target node

            //Check if there is space in the node 

            //If there is no space - split the node and adjust the parents

    }

};
