#ifndef BplusTree_H
#define BplusTree_H

#include <storage.h>


struct BTreeNode{

    bool isleaf;  //flag to indicate if leaf node or internal node
    int* keys; // array of keys that stores keys associated with node 
    union {    //allows you to use either children or values based on the type of node (leaf or internal)
        BTreeNode** children; //If isLeaf is false, this array of pointers stores pointers to child nodes/internal nodes.
        struct RecordsAddressVector* values; // If isLeaf is true, Pointer to an array of block addresses 
    };
    BTreeNode* nextLeaf; // Pointer to the next leaf node (nullptr if there is no next leaf node)

};

class Btree{

    private:
        int degree;
        BTreeNode* root;
        
    public:
        Btree(int degree); // constructor

        void insert(int key, int value);//// Insert a key-value pair into the B+ tree

        std::vector<int> search(int key); //// Search for a key in the B+ tree and return associated values

        void remove(int key);   // Delete a key from the B+ tree

        void print();       // Print the B+ tree for debugging 
};


#endif
