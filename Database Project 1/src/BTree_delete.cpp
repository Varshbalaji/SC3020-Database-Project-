//Search keys in the Bplus Tree 

#include <iostream>
// #include "/Users/lakshya/Desktop/NTU CUrriculum/Year 3/Semester 1/SC3020/Project_1/SC3020-Database-Project-/Database Project 1/include/BplusTree.h"
#include "../include/BplusTree.h"

using namespace std; 

void Btree::removeRecord(int key){

    removeRecord(key, root);
};

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
    if(!node->isleaf && node->numKeysPerNode<(this->deg)/2)
    {
        pair<BTreeNode*, BTreeNode*> siblings = findAdjacentSiblings(root,node);
        if(!( (tryBorrowing(siblings.first, node)) || (tryBorrowing(node,siblings.second)) ))
        {
            {
                            if(siblings.first != nullptr)
                            remove_key_in_internal_node(node, mergeTwoNodes(siblings.first, node));
                            else if(siblings.second != nullptr) // Case for 
                            remove_key_in_internal_node(node, mergeTwoNodes( node, siblings.second));
                            treat_underflow(node);
            }
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
    if(node1==0 || node2==0)
    return false;
    if (node1->numKeysPerNode < (this->deg + 1) / 2) {
        if (node2->numKeysPerNode <= (this->deg + 1) / 2){
            rebalanced = false;}
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
            if (true) {
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
               
                return;
            }
        }
        i = node->numKeysPerNode;
        removeRecord(key, node->child[i]);
         if((node->child[i]->numKeysPerNode < (this->deg+1)/2    && node->child[i]->isleaf)
        || (node->child[i]->numKeysPerNode < (this->deg)/2      && !(node->child[i]->isleaf)))
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




//Find parent 
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


