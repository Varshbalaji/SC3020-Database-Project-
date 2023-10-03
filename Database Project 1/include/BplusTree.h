#ifndef BplusTree_H
#define BplusTree_H

#include <storage.h>


struct BTreeNode{

    bool isleaf;  //flag to indicate if leaf node or internal node
    int numKeys;  // Number of keys currently in the node
    int* keys; // array of keys that stores keys associated with node 
    union {    //allows you to use either children or values based on the type of node (leaf or internal)
        BTreeNode** children; //If isLeaf is false, this array of pointers stores pointers to child nodes/internal nodes.
        struct RecordsAddressVector* values; // If isLeaf is true, Pointer to an array of block addresses 
    };
    BTreeNode* nextLeaf; // Pointer to the next leaf node (nullptr if there is no next leaf node)

};

class Btree{

    private:
        BTreeNode* root;

    public:
        int degree;

        
    public:
        Btree(int degree); // constructor

        void insert(int key, int value);//// Insert a key-value pair into the B+ tree

        std::vector<int> search(int key); //// Search for a key in the B+ tree and return associated values

        void remove(int key);   // Delete a key from the B+ tree

        void print();       // Print the B+ tree for debugging 


    private:
        // Helper functions for deletion and balancing
        void removeFromNode(BTreeNode* node, int keyIndex);
        void redistributeLeafNodes(BTreeNode* leftNode, BTreeNode* rightNode);
        void mergeLeafNodes(BTreeNode* leftNode, BTreeNode* rightNode, BTreeNode* parent, int parentKeyIndex);
        void redistributeInternalNodes(BTreeNode* leftNode, BTreeNode* rightNode);
        void mergeInternalNodes(BTreeNode* leftNode, BTreeNode* rightNode, BTreeNode* parent, int parentKeyIndex);
        void handleUnderflow(BTreeNode* node, BTreeNode* parent, int keyIndex);
        int findKeyIndex(BTreeNode* node, int key);
    };

#endif
