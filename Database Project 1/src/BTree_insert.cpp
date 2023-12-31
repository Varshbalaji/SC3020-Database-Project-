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

void Btree::insert(int keyValue, RecordAddress recordAddress){


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
            cout <<"Test" <<current->keys[i].key_value <<"\n"; 
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
            cout<<"split 1"<<"\n";
            for(i = 0; i < current->numKeysPerNode; i++){
                    current->keys[i] = virtualNode[i];
                    cout << current->keys[i].key_value<<"\n";
            }

            cout<<"split 2"<<"\n";
            for(i = 0, j = current->numKeysPerNode; i < newLeafNode->numKeysPerNode; i++, j++){
                    newLeafNode->keys[i] = virtualNode[j];
                    cout << newLeafNode->keys[i].key_value << "\n";

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

//Helper Functions

//Updates the parent node with the newly created child nodes and adds a parent if needed 
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
        cout<<"Adding to same parent"<<"\n";
        current->keys[i] = key; //insert the key
        current->numKeysPerNode++; //increase current number of keys in the node
        cout << "Testing Internal" << current->keys[i].key_value <<"\n";
        current->child[i+1] = child; //point the i+1th pointer to the newLeafNode (child)
        cout<< "child: "<<  current->child[i+1];
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

        cout<<"Parent split 1"<<"\n";
        for (i=0;i<current->numKeysPerNode;i++){
            current->keys[i] = virtualKeys[i];
            current->child[i] = virtualChildren[i];
        }
        current->child[current->numKeysPerNode] = virtualChildren[i];

        cout<<"Parent split 2"<<"\n";
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

//Find parent 
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

Key_Records Btree::fetchSSKey(BTreeNode *current){
    
    // Find the smallest on the right subtree
    while (!current->isleaf){
        current = current->child[0];
    }
    return current->keys[0];

}

// Print the tree
void Btree::printTree(BTreeNode *current)
//{

//     if (current == nullptr)
//         return;

//     queue<BTreeNode *> q;

//     q.push(current);

//     while (!q.empty())
//     {
//         int queueSize = q.size();
//         for (int i = 0; i < queueSize; i++)
//         {
//           BTreeNode *node = q.front();
//           for (int i = 0; i < node->numKeysPerNode; i++)
//           {
//             cout << node->keys[i].key_value<< "|";
//             if (!node->isleaf)
//             {
//               q.push(node->child[i]);
//             }
//           }
//           cout << "-----";
//           if (!node->isleaf)
//           {
//             q.push(node->child[node->numKeysPerNode]);
//           }
//           q.pop();
//           if(nodeCount-- == 0)
//             return;
//         }
//         cout << endl;
//     }
// }
{
//     Record *dataRecord; 
//     int numberOfDataBlocksAccessed = 0;

//   if (current != NULL) {
//     for (int i = 0; i < current->numKeysPerNode; i++) {
//         float value = current->keys[i].key_value;
//         if (current->isleaf) {
//             cout << value <<" [ ";
//             for(int j=0; j < current->keys[i].storage_array.size(); j++){
//                 tie(dataRecord,numberOfDataBlocksAccessed) = diskStorage->getRecord( current->keys[i].storage_array[j],44);
//                 if (dataRecord != nullptr) {
//                     cout << "(" << dataRecord->gameDate <<"," <<dataRecord->teamID<<") ; ";
//                 }
//                 //cout << "(" << current->keys[i].storage_array[j].blockNumber <<"," <<current->keys[i].storage_array[j].offset<<") ; ";
//             }
//         cout << " ] \n";            
//         }

//     }
    
//     if (current->isleaf != true) {  
//       for (int i = 0; i < current->numKeysPerNode + 1; i++) {
//         printTree(current->child[i]);
//       }
//     }
//   }

  if (current != NULL) {
    for (int i = 0; i < current->numKeysPerNode; i++) {
        float value = current->keys[i].key_value;
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
//Fetch root 
BTreeNode *Btree::fetchRoot(){
    return root;
}

std::vector<Key_Records> Btree::search(float key, bool rangeflag, float key2){

    std::vector<Key_Records> search_result;
    nodesAccessed = 0;

    //Tree is empty
    if(root == nullptr) {
        cout<<"B+ Tree is empty\n";
        return {};
    }

    else{
        BTreeNode* current = root;

        while (current ->isleaf == false){
            nodesAccessed++;
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
            nodesAccessed++;
            for(int i = 0; i<current->numKeysPerNode; i++){
                if (current->keys[i].key_value == key) {
                    cout<<"Key Found!\n";
                    search_result.push_back(current->keys[i]);
                    cout <<nodesAccessed<<"\n";
                    return search_result;
                }
            }
        }

        else{ //Range query
            nodesAccessed++;
            for (int i = 0; i < current->numKeysPerNode+1; i++) {
                
                if(i==current->numKeysPerNode){
                        current = current->child[i];
                        nodesAccessed++;
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
}

int Btree::getLevels(BTreeNode* current){
    if(current->isleaf == false)
        return getLevels(current->child[0])+1;
    
    else if(current->isleaf == true)
        return 1;
    
    else
        return 0;

}

// int main(){
//     int key1 = 1;
//     RecordAddress address1 = {1, 20};
//     int key2 = 2;
//     RecordAddress address2 = {2, 40};
//     int key3 = 10;
//     RecordAddress address3 = {4, 20};
//     int key4 = 4;
//     RecordAddress address4 = {3, 20};  //duplicate 
//     int key5 = 5;
//     RecordAddress address5 = {8, 20};
//     int key6 = 6;
//     RecordAddress address6 = {6, 20};
//     int key7 = 9;
//     RecordAddress address7 = {7, 20};
//     int key8 = 3;
//     RecordAddress address8 = {8, 40};
//     int key9 = 7;
//     RecordAddress address9 = {9, 20};
//     int key10 = 0;
//     RecordAddress address10 = {10, 20};
//     int key11 = 0;
//     RecordAddress address11 = {11, 40};


//     Btree treeNode(60) ;
//     treeNode.insert(key1, address1);
//     treeNode.insert(key2, address2);
//     treeNode.insert(key3, address3);
//     treeNode.insert(key4, address4);
//     treeNode.insert(key5, address5);
//     treeNode.insert(key6, address6);
//     treeNode.insert(key7, address7);
//     treeNode.insert(key8, address8);
//     treeNode.insert(key9, address9);
//     treeNode.insert(key10, address10);
//     treeNode.insert(key11, address11);
//     cout << "TREE START..."<<endl;

//     treeNode.printTree(treeNode.fetchRoot());

//     // vector<Key_Records> searchResult = treeNode.search(8,false,10);
//     // for(int i=0; i<searchResult.size(); i++){
//     //     cout<<searchResult[i].key_value<<"\n";

//     // }
// }

BTreeNode* Btree::fetchSmallestLeaf(BTreeNode *current){
    
    // Find the smallest on the right subtree
    while (!current->isleaf){
        current = current->child[0];
    }
    return current;

}

float Btree::avgFG3_PCT_home( Storage *diskStorage, vector<Key_Records> search_result){
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
            }
        }
    }
    cout<<recordCount<<"\n";
    avg = sum/recordCount;
    return avg;
}

