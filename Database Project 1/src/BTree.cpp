#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "BplusTree.h"


using namespace std;

struct RecordArrayDuplicateKey{
    
    int key_value; 
    vector<RecordAddress> storage_array;

};

Btree::Btree(int n){
    this->root = (BTreeNode*)malloc(sizeof(BTreeNode*));
    this->degree = n;
};

//search
//Insert





// Helper function to insert a key with a pointer to a vector of RecordAddresses in a leaf node
void insert_in_leaf_node(BTreeNode* leafNode, int key, vector<RecordAddress>* addressVector) {
    int insertIndex = -1; // Index to insert the new key

    // Find the correct index to insert the new key
    for (int i = 0; i < leafNode->numKeysPerNode; ++i) {
        if (key < leafNode->keys[i].key_value) {
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



// Helper function to remove a key and its associated vector of RecordAddresses from a leaf node
void remove_key_in_leaf_node(BTreeNode* leafNode, int key) {
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
void remove_key_in_internal_node(BTreeNode* internalNode, int key) {
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


//Helper function to get back 2 node pointers adjacent to the non-leaf node given (divided into 2 functions )
// Function A) to find the parent and index of the node
void findParentAndIndex(BTreeNode* root, BTreeNode* current, BTreeNode* prev, BTreeNode* nodeToFind, BTreeNode*& parent, int& index) {
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
std::pair<BTreeNode*, BTreeNode*> findAdjacentSiblings(BTreeNode* root, BTreeNode* node) {
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




