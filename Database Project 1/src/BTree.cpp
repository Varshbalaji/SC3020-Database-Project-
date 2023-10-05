#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "../include/BplusTree.h"


using namespace std;

// struct RecordArrayDuplicateKey{
    
//     int key_value; 
//     vector<RecordAddress> storage_array;

// };

// Btree::Btree(int n){
//     this->root = (BTreeNode*)malloc(sizeof(BTreeNode*));
//     this->degree = n;
// };



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



//Helper function to insert a child node (can be non-leaf/leaf) to parent node (non-leaf node). Function will be given
//address of childnode, parentnode , key.
void insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, int key) {
    if (parent == nullptr || child == nullptr || parent->isleaf) {
        // Invalid input or parent is a leaf node
        return;
    }

    int insertIndex = -1; // Index to insert the new child node

    // Find the correct index to insert the new child node based on the key
    for (int i = 0; i < parent->numKeysPerNode; ++i) {
        if (key < parent->keys[i].key_value) {
            insertIndex = i;
            break;
        }
    }

    // Shift existing child nodes and keys to the right to make space for the new child
    for (int i = parent->numKeysPerNode; i > insertIndex; --i) {
        parent->child[i + 1] = parent->child[i];
        parent->keys[i] = parent->keys[i - 1];
    }

    // Insert the new child node and respective key
    parent->child[insertIndex + 1] = child;
    parent->keys[insertIndex].key_value = key;
    parent->numKeysPerNode++;   
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












int main()
{

    // Create a B+ tree instance
    Btree btreeInstance(512); // Assuming a block size of 512

    // Create nodes and set their key values
    Key_Records node1;
    node1.key_value = 1;

    Key_Records node2;
    node2.key_value = 2;

    Key_Records node3;
    node3.key_value = 3;

    // Manually construct the tree structure
    BTreeNode* root = new BTreeNode(btreeInstance.getDegree());
    root->isleaf = false;
    root->keys[0] = node2; // Set the root key
    root->numKeysPerNode = 1;
    root->child[0] = new BTreeNode(btreeInstance.getDegree());
    root->child[1] = new BTreeNode(btreeInstance.getDegree());
    root->child[0]->keys[0] = node1; // Set the left child key
    root->child[0]->numKeysPerNode = 1;
    root->child[1]->keys[0] = node3; // Set the right child key
    root->child[1]->numKeysPerNode = 1;

    // Set the root node in the B+ tree instance
    btreeInstance.setRoot(root);

    // Now, you have manually constructed a B+ tree with nodes 1, 2, and 3
    // You can test your functions on this instance

    btreeInstance.print();

    return 0;
}



