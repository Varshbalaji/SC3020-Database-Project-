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

// int main() {
//     // Create a B+ tree instance with a block size of 512 and degree 3
//     Btree btreeInstance(3);

//     // Create nodes and set their key values
//     Key_Records node1, node2, node3, node4, node5, node6, node7, node8, node9, node10;
//     node1.key_value = 1;
//     node2.key_value = 2;
//     node3.key_value = 3;
//     node4.key_value = 4;
//     node5.key_value = 5;
//     node6.key_value = 6;
//     node7.key_value = 7;
//     node8.key_value = 8;
//     node9.key_value = 9;
//     node10.key_value = 10;

//     // Manually construct the tree structure
//     BTreeNode* root = new BTreeNode(btreeInstance.getDegree());
//     root->isleaf = false;
//     root->keys[0] = node3; // Set the root key
//     root->keys[1] = node7; // Set the second key in the root
//     root->numKeysPerNode = 2;

//     // Create the left internal node (3)
//     BTreeNode* leftInternal = new BTreeNode(btreeInstance.getDegree());
//     leftInternal->isleaf = false;
//     leftInternal->keys[0] = node3;
//     leftInternal->numKeysPerNode = 1;

//     // Create the right internal node (5)
//     BTreeNode* rightInternal = new BTreeNode(btreeInstance.getDegree());
//     rightInternal->isleaf = false;
//     rightInternal->keys[0] = node5;
//     rightInternal->numKeysPerNode = 1;

//     // Create the left leaf node with keys (1, 2)
//     BTreeNode* leftLeaf = new BTreeNode(btreeInstance.getDegree());
//     leftLeaf->isleaf = true;
//     leftLeaf->keys[0] = node1;
//     leftLeaf->keys[1] = node2;
//     leftLeaf->numKeysPerNode = 2;

//     // Create the right leaf node with keys (3, 4)
//     BTreeNode* rightLeaf = new BTreeNode(btreeInstance.getDegree());
//     rightLeaf->isleaf = true;
//     rightLeaf->keys[0] = node3;
//     rightLeaf->keys[1] = node4;
//     rightLeaf->numKeysPerNode = 2;

//     // Create the second left leaf node with keys (5, 6)
//     BTreeNode* leftLeaf2 = new BTreeNode(btreeInstance.getDegree());
//     leftLeaf2->isleaf = true;
//     leftLeaf2->keys[0] = node5;
//     leftLeaf2->keys[1] = node6;
//     leftLeaf2->numKeysPerNode = 2;

//     // Create the second right leaf node with keys (7, 8)
//     BTreeNode* rightLeaf2 = new BTreeNode(btreeInstance.getDegree());
//     rightLeaf2->isleaf = true;
//     rightLeaf2->keys[0] = node7;
//     rightLeaf2->keys[1] = node8;
//     rightLeaf2->numKeysPerNode = 2;

//     // Create the third internal node (9)
//     BTreeNode* rightInternal2 = new BTreeNode(btreeInstance.getDegree());
//     rightInternal2->isleaf = false;
//     rightInternal2->keys[0] = node9;
//     rightInternal2->numKeysPerNode = 1;

//     // Create the third right leaf node with keys (9, 10)
//     BTreeNode* rightLeaf3 = new BTreeNode(btreeInstance.getDegree());
//     rightLeaf3->isleaf = true;
//     rightLeaf3->keys[0] = node9;
//     rightLeaf3->keys[1] = node10;
//     rightLeaf3->numKeysPerNode = 2;

//     // Set child nodes in the root
//     root->child[0] = leftLeaf;
//     root->child[1] = leftInternal;
//     root->child[2] = rightInternal;

//     // Set child nodes in the internal nodes
//     leftInternal->child[0] = leftLeaf;
//     leftInternal->child[1] = rightLeaf;
//     rightInternal->child[0] = leftLeaf2;
//     rightInternal->child[1] = rightLeaf2;

//     rightInternal2->child[0] = rightLeaf3;
//     rightInternal2->child[1] = rightInternal;

//     // Set the root node in the B+ tree instance
//     btreeInstance.setRoot(root);

//     // Call the print function on the B+ tree instance
//     btreeInstance.print(root);

//     return 0;
// }


// int main() {
//     // Create a B+ tree instance with a degree of 3
//     Btree btreeInstance(3);

//     // Create keys from 1 to 12
//     Key_Records keys[12];
//     for (int i = 0; i < 12; ++i) {
//         keys[i].key_value = i + 1;
//     }

//     // Manually construct the B+ tree structure
//     BTreeNode* root = new BTreeNode(btreeInstance.getDegree());
//     root->isleaf = true;
//     for (int i = 0; i < 12; ++i) {
//         root->keys[i] = keys[i];
//     }
//     root->numKeysPerNode = 12;

//     // Set the root node in the B+ tree instance
//     btreeInstance.setRoot(root);

//     // Call the print function on the B+ tree instance
//     btreeInstance.print(root);

//     return 0;
// }
