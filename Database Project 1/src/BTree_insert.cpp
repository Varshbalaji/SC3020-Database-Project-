//Search keys in the Bplus Tree 

#include <iostream>

#include "BplusTree.h"

using namespace std; 

void BTree::insert(Key_Records key){

    //Check if there is a root node
    if(root=null){
        root = new BTreeNode(deg);
        root->keys[0] = key;
        root->numKeysPerNode = 1
        nodeCount++
    }

    else{

        //Call search function to check for duplicates 

        //Else find the target node

            //Check if there is space in the node 

            //If there is no space - split the node and adjust the parents

    }

};