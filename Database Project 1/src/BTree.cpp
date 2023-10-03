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


















//Delete -> Consists of helper functions which are defined , then the main deletetion function given below at the end



//HELPER FUNCTIONS START FROM HERE

// Helper function to remove a key from a node
void removeFromNode(BTreeNode* node, int keyIndex) {
    for (int i = keyIndex; i < node->numKeys - 1; ++i) {
        
        if (node->isleaf) {
            // If it's a leaf node, update the associated values accordingly
            // For example: node->values[i] = node->values[i + 1];
            

        } else {
            // If it's an internal node, update the child pointers accordingly
            // For example: node->children[i] = node->children[i + 1];


        }
    }
    node->numKeys--;
}

// Helper function to redistribute keys between two leaf nodes
void redistributeLeafNodes(BTreeNode* leftNode, BTreeNode* rightNode) {
    // Check if the leftNode and rightNode are leaf nodes
    if (!leftNode->isleaf || !rightNode->isleaf) {
        // Handle error: Both nodes should be leaf nodes for redistribution
        return;
    }

    // Calculate the total number of keys in both nodes
    int totalKeys = leftNode->numKeys + rightNode->numKeys;

    // Calculate the number of keys to move from the rightNode to the leftNode
    int keysToMove = (totalKeys / 2) - leftNode->numKeys;

    // Move keys and values from rightNode to leftNode
    for (int i = 0; i < keysToMove; ++i) {
        leftNode->keys[leftNode->numKeys] = rightNode->keys[i];
        leftNode->values[leftNode->numKeys] = rightNode->values[i];
        leftNode->numKeys++;

        // Update parent if necessary (parentKeyIndex should be provided)
        // ...
    }

    // Remove the moved keys and values from the rightNode
    for (int i = keysToMove; i < rightNode->numKeys; ++i) {
        rightNode->keys[i - keysToMove] = rightNode->keys[i];
        rightNode->values[i - keysToMove] = rightNode->values[i];
    }

    rightNode->numKeys -= keysToMove;

    // Update parent if necessary
    // ...
}

// Helper function to merge two leaf nodes
void mergeLeafNodes(BTreeNode* leftNode, BTreeNode* rightNode, BTreeNode* parent, int parentKeyIndex) {
    // Check if the leftNode and rightNode are leaf nodes
    if (!leftNode->isleaf || !rightNode->isleaf) {
        // Handle error: Both nodes should be leaf nodes for merging
        return;
    }

    // Move keys and values from the rightNode to the leftNode
    for (int i = 0; i < rightNode->numKeys; ++i) {
        leftNode->keys[leftNode->numKeys] = rightNode->keys[i];
        leftNode->values[leftNode->numKeys] = rightNode->values[i];
        leftNode->numKeys++;

        // Update parent if necessary (parentKeyIndex should be provided)
        // ...
    }

    // Update parent and remove the rightNode
    // ...
}

// Helper function to redistribute keys between two internal nodes
void redistributeInternalNodes(BTreeNode* leftNode, BTreeNode* rightNode) {
    // Check if the leftNode and rightNode are internal nodes
    if (leftNode->isleaf || rightNode->isleaf) {
        // Handle error: Both nodes should be internal nodes for redistribution
        return;
    }

    // Calculate the total number of keys in both nodes
    int totalKeys = leftNode->numKeys + rightNode->numKeys;

    // Calculate the number of keys to move from the rightNode to the leftNode
    int keysToMove = (totalKeys / 2) - leftNode->numKeys;

    // Move keys and children pointers from rightNode to leftNode
    for (int i = 0; i < keysToMove; ++i) {
        leftNode->keys[leftNode->numKeys] = rightNode->keys[i];
        leftNode->children[leftNode->numKeys + 1] = rightNode->children[i];
        leftNode->numKeys++;

        // Update parent if necessary
        // ...
    }

    // Remove the moved keys and children pointers from the rightNode
    for (int i = keysToMove; i < rightNode->numKeys; ++i) {
        rightNode->keys[i - keysToMove] = rightNode->keys[i];
        rightNode->children[i - keysToMove] = rightNode->children[i];
    }

    rightNode->children[rightNode->numKeys - keysToMove] = rightNode->children[rightNode->numKeys];

    rightNode->numKeys -= keysToMove;

    // Update parent if necessary
    // ...
}

// Helper function to merge two internal nodes
void mergeInternalNodes(BTreeNode* leftNode, BTreeNode* rightNode, BTreeNode* parent, int parentKeyIndex) {
    // Check if the leftNode and rightNode are internal nodes
    if (leftNode->isleaf || rightNode->isleaf) {
        // Handle error: Both nodes should be internal nodes for merging
        return;
    }

    // Move keys and children pointers from the rightNode to the leftNode
    for (int i = 0; i < rightNode->numKeys; ++i) {
        leftNode->keys[leftNode->numKeys] = rightNode->keys[i];
        leftNode->children[leftNode->numKeys + 1] = rightNode->children[i];
        leftNode->numKeys++;

        // Update parent if necessary
        // ...
    }

    // Update parent and remove the rightNode
    // ...
}


// Helper function to handle underflow in a node
void handleUnderflow(BTreeNode* node, BTreeNode* parent, int keyIndex) {
    // Determine if redistribution or merging is needed based on node type (leaf or internal)
    if (node->isleaf) {
        BTreeNode* leftNeighbor = ...; // Get the left neighbor leaf node
        BTreeNode* rightNeighbor = ...; // Get the right neighbor leaf node

        // Check if redistribution is possible
        if (leftNeighbor != nullptr && leftNeighbor->numKeys > degree / 2) {
            redistributeLeafNodes(leftNeighbor, node);
        } else if (rightNeighbor != nullptr && rightNeighbor->numKeys > degree / 2) {
            redistributeLeafNodes(node, rightNeighbor);
        } else {
            // Merge with a neighbor
            if (leftNeighbor != nullptr) {
                mergeLeafNodes(leftNeighbor, node, parent, keyIndex - 1);
            } else if (rightNeighbor != nullptr) {
                mergeLeafNodes(node, rightNeighbor, parent, keyIndex);
            }
        }
    } else {
        // Handle underflow in an internal node
        BTreeNode* leftNeighbor = ...; // Get the left neighbor internal node
        BTreeNode* rightNeighbor = ...; // Get the right neighbor internal node

        // Check if redistribution is possible
        if (leftNeighbor != nullptr && leftNeighbor->numKeys > degree / 2) {
            redistributeInternalNodes(leftNeighbor, node);
        } else if (rightNeighbor != nullptr && rightNeighbor->numKeys > degree / 2) {
            redistributeInternalNodes(node, rightNeighbor);
        } else {
            // Merge with a neighbor
            if (leftNeighbor != nullptr) {
                mergeInternalNodes(leftNeighbor, node, parent, keyIndex - 1);
            } else if (rightNeighbor != nullptr) {
                mergeInternalNodes(node, rightNeighbor, parent, keyIndex);
            }
        }
    }

    // Update parent if necessary
    if (parent != nullptr && parent->numKeys < degree / 2) {
        int parentKeyIndex = findKeyIndex(parent, key);
        handleUnderflow(parent, parent->parent, parentKeyIndex);
    }
}

// Helper function to find the index of a key within a node
int findKeyIndex(BTreeNode* node, int key) {
    for (int i = 0; i < node->numKeys; ++i) {
        if (node->keys[i] == key) {
            return i;
        }
    }
    return -1; // Key not found in the node
}















// MAIN DELETETION FUNCTION STARTS FROM HERE WHICH ENCAPSULATES THE HELPER FUNCTIONS 

void Btree::remove(int key) {
    // Search for the key to be deleted and locate the leaf node.
    BTreeNode* parent = nullptr;
    int keyIndex = -1; //initally set to -1 to indicate that the key is not found
    BTreeNode* currentNode = root;







    //SEARCH PART TO BE CHANGED ACCORDING TO WHAT VARSHA AND ANUSHREE COMPLETE 
    while (!currentNode->isleaf)  {
        // Internal node, navigate to the appropriate child node
        for (int i = 0; i < currentNode->numKeys; ++i) {
            if (key < currentNode->keys[i]) {
                currentNode = currentNode->children[i];
                break;
            }
            if (i == currentNode->numKeys - 1 || key < currentNode->keys[i + 1]) {
                currentNode = currentNode->children[i + 1];
                break;
            }
        }
    }

    // Locate the key within the leaf node (if present)
    for (int i = 0; i < currentNode->numKeys; ++i) {
        if (currentNode->keys[i] == key) {
            keyIndex = i;
            break;
        }
    }








    // If the key is not found, the delete operation is complete.
    if (keyIndex == -1) {
        return; // Key not found, deletion complete
    }

    // Step 3: If key is found , handle deletion in the leaf node using helper function which is define above.
    removeFromNode(currentNode, keyIndex);

 
    //After removing the key, the code checks if the leaf node is underfilled. 
    //An underflow occurs when the number of keys in a node is less than half the degree of the tree (degree / 2).
    // If there's an underflow, the code proceeds to handle it using the handleUnderflow function (not shown in the provided code).
    if (currentNode->numKeys < degree / 2) {
        // Underflow in leaf node, consider redistribution or merging
        handleUnderflow(currentNode, parent, keyIndex);
    }

    // Last the code enters a loop that propagates changes upward through the tree if necessary. 
    //It checks if the parent node of the current node (which was saved in the parent variable) is underfilled. 
    //If the parent node has an underflow, it calls handleUnderflow on the parent node and continues this process recursively up the tree.
    while (parent != nullptr) {
        // Check if the parent node is underfilled
        if (parent->numKeys < degree / 2) {
            // Underflow in internal node, propagate changes to parent
            keyIndex = findKeyIndex(parent, key);
            handleUnderflow(parent, parent->parent, keyIndex);
        }
        currentNode = parent;
        parent = currentNode->parent;
    }
}
