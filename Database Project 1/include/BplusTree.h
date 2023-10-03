#ifndef BplusTree_H
#define BplusTree_H

#include <storage.h>
#include <math.h>
#include <vector>


struct Key_Records{
    
    int key_value; 
    vector<RecordAddress> storage_array;

};

struct BTreeNode{

    bool isleaf;  //flag to indicate if leaf node or internal node
    Key_Records * keys; // array of keys that stores keys associated with node 
    int numKeysPerNode; // number of  keys per node 
    BTreeNode **child;

    //Constructor 
    BTreeNode(int n){ //n : maximum number of keys in a node 
        keys = new Key_Records[n];
        isleaf = true;
        numKeysPerNode = 0;
        child = new BTreeNode*[n+1]; // Btree node can have maximum n+1 pointers
        for(int i=0;i<n;i++)
        {
            child[i]= nullptr;
        }

    }

};

class Btree{

    private:
        int deg; //degree
        BTreeNode* root;
        int nodeCount;
        
    public:
        Btree(unsigned int BlockSize){
            root = nullptr;
            nodeCount =0;
            deg = 0;
            unsigned int spaceForKeys = BlockSize - sizeof(int) - sizeof(bool) - sizeof(BTreeNode *);
            deg = floor(spaceForKeys / sizeof(Key_Records) + sizeof(BTreeNode *));


        } // constructor

        void insert(Key_Records key);//// Insert a key-value pair into the B+ tree

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
