#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "../include/BplusTree.h"


using namespace std;


void Btree::treat_underflow(BTreeNode* node)
{
    if(node == root)
    {
        if(node->numKeysPerNode<1)
        {
            if(node->child[0]==nullptr)
            root = node->child[1];
            else
            root = node->child[0];
        }
    }
}
// Helper function to insert a key with a pointer to a vector of RecordAddresses in a leaf node
void  Btree::insert_in_leaf_node(BTreeNode* leafNode, int key, vector<RecordAddress>* addressVector) {
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

// bool Btree::tryBorrowing(BTreeNode* node1, BTreeNode* node2)
// {
//     bool rebalanced = false;
//     if(node1->numKeysPerNode < (this->deg+1)/2)
//     {
//         if(node2->numKeysPerNode <= (this->deg+1)/2)
//         rebalanced = false;
//         else
//         {
//             if(node1->isleaf)
//             {
               
//                 insert_in_leaf_node(node1, node2->keys[0].key_value, &(node2->keys[0].storage_array));
//                 remove_key_in_leaf_node(node2, node2->keys[0].key_value);
//             }
//             else
//             {
//                 insert_ChildNode_in_ParentNode(node1, node2->child[0], node2->keys[0].key_value);
//                 remove_key_in_internal_node(node2, node2->keys[0].key_value);
//             }
//             rebalanced = true;
//         }

//     }
//     else 
//     {
//         if(node1->numKeysPerNode <= (this->deg+1)/2)
//         rebalanced = false;
//         else
//         {
            
//             if(node1->isleaf)
//             {
//                 insert_in_leaf_node(node2,  node1->keys[node1->numKeysPerNode-1].key_value, &(node1->keys[node1->numKeysPerNode-1].storage_array));
//                 remove_key_in_leaf_node(node1, node1->keys[node1->numKeysPerNode-1].key_value);
//             }
//             else
//             {
//                 insert_ChildNode_in_ParentNode(node2,  node1->child[node1->numKeysPerNode-1], node1->keys[node1->numKeysPerNode-1].key_value);
//                 remove_key_in_internal_node(node1,  node1->keys[node1->numKeysPerNode-1].key_value);
//             }
//             rebalanced = true;
//         }
//     }
//     return rebalanced;
// }


// Helper function to update the key in the parent node when a child node borrows
void Btree::updateParentKey2(BTreeNode* parent, int oldKey, int newKey) {
    for (int i = 0; i < parent->numKeysPerNode; ++i) {
        if (parent->keys[i].key_value == oldKey) {
            parent->keys[i].key_value = newKey;
            break;
        }
    }
}


bool Btree::tryBorrowing(BTreeNode* node1, BTreeNode* node2) {
    bool rebalanced = false;

    if (node1->numKeysPerNode < (this->deg + 1) / 2) {
        if (node2->numKeysPerNode <= (this->deg + 1) / 2)
            rebalanced = false;
        else {
            if (node1->isleaf) {
                insert_in_leaf_node(node1, node2->keys[0].key_value, &(node2->keys[0].storage_array));
                int oldKey = node2->keys[0].key_value;
                remove_key_in_leaf_node(node2, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node2, parent, index);
                updateParentKey2(parent, oldKey, node2->keys[0].key_value);
            } else {
                insert_ChildNode_in_ParentNode(node1, node2->child[0], node2->keys[0].key_value);
                int oldKey = node2->keys[0].key_value;
                remove_key_in_internal_node(node2, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node2, parent, index);
                updateParentKey2(parent, oldKey, node2->keys[0].key_value);
            }
            rebalanced = true;
        }
    } else {
        if (node1->numKeysPerNode <= (this->deg + 1) / 2)
            rebalanced = false;
        else {
            if (node1->isleaf) {
                insert_in_leaf_node(node2, node1->keys[node1->numKeysPerNode - 1].key_value,
                                    &(node1->keys[node1->numKeysPerNode - 1].storage_array));
                int oldKey = node1->keys[node1->numKeysPerNode - 1].key_value;
                remove_key_in_leaf_node(node1, oldKey);
                BTreeNode* parent = nullptr;
                int index = -1;
                findParentAndIndex(root, root, nullptr, node1, parent, index);
                updateParentKey2(parent, oldKey, node1->keys[node1->numKeysPerNode - 1].key_value);
            } else {
                insert_ChildNode_in_ParentNode(node2, node1->child[node1->numKeysPerNode - 1],
                                               node1->keys[node1->numKeysPerNode - 1].key_value);
                int oldKey = node1->keys[node1->numKeysPerNode - 1].key_value;
                remove_key_in_internal_node(node1, oldKey);
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


// Helper function to remove a key and its associated vector of RecordAddresses from a leaf node
void  Btree::remove_key_in_leaf_node(BTreeNode* leafNode, int key) {
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



// Helper function to remove a key and its associated child pointer from an internal node
void  Btree::remove_key_in_internal_node(BTreeNode* internalNode, int key) {
    int keyIndex = -1; // Index of the key to be removed , -1 to act as a flag 

    // Find the index of the key to be removed
    for (int i = 0; i < internalNode->numKeysPerNode; ++i) {
        if (internalNode->keys[i].key_value == key) {
            keyIndex = i;
            break;
        }
    }

    // If the key was not found, return (key not present in the node)
    if (keyIndex == -1) {
        return;
    }

    // Shift keys and child pointers to the left to fill the gaps 
    for (int i = keyIndex; i < internalNode->numKeysPerNode - 1; ++i) {
        internalNode->keys[i] = internalNode->keys[i + 1];
        internalNode->child[i + 1] = internalNode->child[i + 2];
    }

    // Decrease the number of keys and child pointers in the node
    internalNode->numKeysPerNode--;
}



//Helper function to insert a child node (can be non-leaf/leaf) to parent node (non-leaf node). Function will be given
//address of childnode, parentnode , key.
void  Btree::insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, int key) {
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
    insertIndex= i;
    // Shift existing child nodes and keys to the right to make space for the new child
    for ( i = insertIndex; i < parent->numKeysPerNode; i++) {
        parent->child[i+1] = parent->child[i];
        parent->keys[i+1] = parent->keys[i];
    }
    parent->child[i+1] = parent->child[i];
    

    // Insert the new child node and respective key
    parent->child[insertIndex] = child;
    parent->keys[insertIndex].key_value = key;
    parent->numKeysPerNode++;   

    
}





//Helper function to get back 2 node pointers adjacent to the non-leaf node given (divided into 2 functions )
// Function A) to find the parent and index of the node
void  Btree::findParentAndIndex(BTreeNode* root, BTreeNode* current, BTreeNode* prev, BTreeNode* nodeToFind, BTreeNode*& parent, int& index) {
    if (current == nullptr) {
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

//Function B) that uses FunctionA)
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





void Btree::removeRecord(int key, BTreeNode* node){

    int keyToFind =  key;
    
    if(node->isleaf)
    remove_key_in_leaf_node(node,keyToFind);  //remove_key_in_leaf_node(BTreeNode* leafNode, int key)
    else
    {
        int i = 0;
        for(i = 0; i <node->numKeysPerNode; i++)
        {
    
            if(node->keys[i].key_value >= keyToFind)
            {   
                
                removeRecord(key, node->child[i]);
                
                // Condition of unbalanced child
                if((node->child[i]->numKeysPerNode < (this->deg+1)/2    && node->child[i]->isleaf)
                || (node->child[i]->numKeysPerNode < (this->deg)/2      && !(node->child[i]->isleaf)))
                    {
                        if(!((i>0 && tryBorrowing(node->child[i-1], node->child[i])) || (i<node->numKeysPerNode && tryBorrowing(node->child[i],node->child[i+1]))))
                        
                        {
                            if(i > 0)
                            remove_key_in_internal_node(node, mergeTwoNodes(node->child[i-1], node->child[i]));
                            else if(i < node->numKeysPerNode) // Case for 
                            remove_key_in_internal_node(node, mergeTwoNodes( node->child[i], node->child[i+1]));
                            treat_underflow(node);
                        }
                    }
               

            }
        }
        removeRecord(key, node->child[i]);
        if(node->child[i]->numKeysPerNode < (this->deg+1)/2)
        {
            if(!((i>0 && tryBorrowing(node->child[i], node->child[i+1])) || (i<node->numKeysPerNode && tryBorrowing(node->child[i+1], node->child[i]))))
            {
                if(i > 0)
                remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i-1], node->child[i]));
                else if(i < node->numKeysPerNode)
                remove_key_in_internal_node(node->child[i], mergeTwoNodes(node->child[i+1], node->child[i]));

            }              
        }

    }
    
}


void Btree::removeRecord(int key){

    removeRecord(key, root);
}


void Btree::insert(int keyValue, RecordAddress recordAddress){


    Key_Records key = {keyValue};
    // if there is a root node
    if(root == nullptr){
        cout << "B+ Tree Creation In Progress.....\n" << "\n";
        root = new BTreeNode(deg);
        root->keys[0] = key ; //inserting 
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
            for(i = 0; i < current->numKeysPerNode; i++){
                    current->keys[i] = virtualNode[i];
                    cout << current->keys[i].key_value<<"\n";
            }

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
            }
            else{ //insert new parent 
                insertParent(newLeafNode->keys[0],parent,newLeafNode);
            }
        } 
        
    }
}

void Btree::insertParent(Key_Records key, BTreeNode *current, BTreeNode *child){

    if(current->numKeysPerNode < deg){ //If the internal node is not full
        int i = 0;
        while(key.key_value > current->keys[i].key_value && i < current->numKeysPerNode){ //find the last key that is smaller than the one being inserted
            i++;
        }
        for(int j = current->numKeysPerNode; j>i; j--){
            current->keys[j] = current->keys[j-1];
        }

        current->keys[i] = key; //insert the key
        current->numKeysPerNode++; //increase current number of keys in the node
        current->child[i+1] = child; //point the i+1th pointer to the newLeafNode (child)
    }

    else { //if the parent node is full, split the node and create a new internal node
        BTreeNode *newParent = new BTreeNode(deg);
        Key_Records virtualKeys[deg+1];
        BTreeNode *virtualChildren[deg+2];
        for (int keyIndex = 0; keyIndex < deg; keyIndex++) {
            virtualKeys[keyIndex] = current->keys[keyIndex];     //copy the keys 
        }
        for (int childIndex = 0; childIndex < deg + 1; childIndex++) {
            virtualChildren[childIndex] = current->child[childIndex];
        }
        int i=0, j;
        while((key.key_value > virtualKeys[i].key_value) && i<deg){
                i++;
        }
        for (int j = deg+ 1; j>i;j--){
            virtualKeys[j] = virtualKeys[j-1];
        }
        virtualKeys[i] = key; //insert key in the correct position of the virtual array of keys 
        for (int j = deg+ 2; j>i+1;j--){
            virtualChildren[j] = virtualChildren[j-1];
        }
        virtualChildren[i+1] = child;
        newParent->isleaf = false;
        current ->numKeysPerNode = (deg +1)/2; //number of keys in the current node 
        newParent->numKeysPerNode = deg - (deg +1)/2; // number of keys in the new internal node 
        for (i = 0, j = current->numKeysPerNode + 1; i < newParent->numKeysPerNode; i++, j++) {
                newParent->keys[i] = virtualKeys[j]; //copy half of the keys into the new internal node 
        }
        for (i = 0, j = current->numKeysPerNode + 1; i < newParent->numKeysPerNode + 1; i++, j++) {
                newParent->child[i] = virtualChildren[j];
        }
        if (current == root) { // if we reach the root 
                BTreeNode *newRoot = new BTreeNode(deg);
                newRoot->keys[0] = current->keys[current->numKeysPerNode]; // add the smallest value in the right subtree to the root 
                newRoot->child[0] = current;
                newRoot->child[1] = newParent;
                newRoot->isleaf = false;
                newRoot->numKeysPerNode = 1;
                root = newRoot;
        }
        else{
            insertParent(current -> keys[current -> numKeysPerNode], findParent(root, current), newParent); //recursively insert parent 
        }




    

    }
    return;

}
BTreeNode *Btree::findParent( BTreeNode *current, BTreeNode *child){
    BTreeNode *parent;
    if (current->isleaf || current->child[0]->isleaf){
        return NULL;
    }
    for (int i=0; i < current->numKeysPerNode+1 ; i++){
        if (current->child[i] == child) {
            parent = current;
            return parent;
        }       
        else {
            parent = findParent(current->child[i], child);
                if (parent != NULL)
                    return parent;
        }
    }
    return parent;

}

void Btree::printTree(BTreeNode *current)
{
  if (current != NULL) {
    for (int i = 0; i < current->numKeysPerNode; i++) {
        int value = current->keys[i].key_value;
        cout << value <<" ";
    }
    cout << "\n";
    if (current->isleaf != true) {  
      for (int i = 0; i < current->numKeysPerNode + 1; i++) {
        printTree(current->child[i]);
      }
    }
  }
}
BTreeNode *Btree::fetchRoot(){
    return root;
}


int Btree::mergeTwoNodes(BTreeNode* node1, BTreeNode* node2)
{
    if(node1->numKeysPerNode + node2->numKeysPerNode > this->deg)
        return 0;
    else
        for(int i = 0; i<node2->numKeysPerNode && !node2->isleaf; i++)
            insert_ChildNode_in_ParentNode(node1,node2->child[i], node2->keys[i].key_value); //void insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, int key);
        for(int i = 0; i<node2->numKeysPerNode && node2->isleaf; i++)
            insert_in_leaf_node(node1, node2->keys[i].key_value, &(node2->keys[i].storage_array));

    return max(node1->keys[0].key_value,node2->keys[0].key_value);
}



int main(){
    int key1 = 1;
    RecordAddress address1 = {1, 20};
    int key2 = 2;
    RecordAddress address2 = {2, 40};
    int key3 = 10;
    RecordAddress address3 = {4, 20};
    int key4 = 4;
    RecordAddress address4 = {3, 20};  //duplicate 
    int key5 = 5;
    RecordAddress address5 = {8, 20};
    // int key6 = 6;
    // RecordAddress address6 = {6, 20};
    // int key7 = 9;
    // RecordAddress address7 = {7, 20};
    // int key8 = 3;
    // RecordAddress address8 = {8, 40};
    // int key9 = 7;
    // RecordAddress address9 = {9, 20};
    // int key10 = 8;
    // RecordAddress address10 = {10, 20};
    // int key11 = 8;
    // RecordAddress address11 = {11, 40};


    Btree treeNode(60) ;
    treeNode.insert(key1, address1);
    treeNode.insert(key2, address2);
    treeNode.insert(key3, address3);
    treeNode.insert(key4, address4);
    treeNode.insert(key5, address5);
    // treeNode.insert(key6, address6);
    // treeNode.insert(key7, address7);
    // treeNode.insert(key8, address8);
    // treeNode.insert(key9, address9);
    // treeNode.insert(key10, address10);
    // treeNode.insert(key11, address11);

    treeNode.printTree(treeNode.fetchRoot());

    treeNode.removeRecord(2);
    treeNode.printTree(treeNode.fetchRoot());
    treeNode.removeRecord(1);

    treeNode.printTree(treeNode.fetchRoot());
    




}










        // void Btree::printTree(BTreeNode* node, int level /* = 0 */) {
        //         if (node == nullptr) {
        //             return;
        //         }

        //         // Recursively print the right child
        //         printTree(node->child[node->numKeysPerNode]);

        //         // Print the current node's keys at this level
        //         for (int i = 0; i < node->numKeysPerNode; ++i) {
        //             for (int j = 0; j < level; ++j) {
        //                 cout << "  "; // Indentation based on the level
        //             }
        //             cout << node->keys[i].key_value << endl;
        //         }

        //         // Recursively print the left children
        //         for (int i = node->numKeysPerNode - 1; i >= 0; --i) {
        //             printTree(node->child[i], level + 1);
        //         }
        //     }

        // void Btree::printTree(BTreeNode* node, int level /* = 0 */) {
        //     if (node == nullptr) {
        //         return;
        //     }

        //     // Print the current node's keys at this level
        //     for (int i = 0; i < node->numKeysPerNode; ++i) {
        //         for (int j = 0; j < level; ++j) {
        //             cout << "  "; // Indentation based on the level
        //         }
        //         cout << node->keys[i].key_value << endl;
        //     }

        //     // Recursively print the child nodes
        //     if (!node->isleaf) {
        //         for (int i = 0; i <= node->numKeysPerNode; ++i) {
        //             printTree(node->child[i], level + 1);
        //         }
        //     }
        // }



        // void Btree::print(BTreeNode* node) {
        //         cout << "B+ Tree:" << endl;
        //         printTree(node,0);
        //     }









// int main() {
//     // Create a B+ tree instance with a block size of 512
//     Btree btreeInstance(512);

//     // Create nodes and set their key values
//     Key_Records node1;
//     node1.key_value = 1;

//     Key_Records node2;
//     node2.key_value = 2;

//     Key_Records node3;
//     node3.key_value = 3;

//     // Manually construct the tree structure
//     BTreeNode* root = new BTreeNode(btreeInstance.getDegree());
//     root->isleaf = false;
//     root->keys[0] = node2; // Set the root key
//     root->numKeysPerNode = 1;
//     root->child[0] = new BTreeNode(btreeInstance.getDegree());
//     root->child[1] = new BTreeNode(btreeInstance.getDegree());
//     root->child[0]->keys[0] = node1; // Set the left child key
//     root->child[0]->numKeysPerNode = 1;
//     root->child[1]->keys[0] = node3; // Set the right child key
//     root->child[1]->numKeysPerNode = 1;

//     // Set the root node in the B+ tree instance (if needed)
//      btreeInstance.setRoot(root); // Comment this line

//     // Now, you have manually constructed a B+ tree with nodes 1, 2, and 3
//     // You can test your functions on this instance

//     // Call the print function on the B+ tree instance
//     btreeInstance.print(root);

//     return 0;
// }


