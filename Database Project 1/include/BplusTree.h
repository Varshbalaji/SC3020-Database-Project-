#ifndef BplusTree_H
#define BplusTree_H

#include "../include/Storage.h"
#include <math.h>
#include <vector>


struct Key_Records{
    
    int key_value; 
    vector<RecordAddress> storage_array;

};

struct BTreeNode{

    bool isleaf;  //flag to indicate if leaf node or internal node
    Key_Records * keys; // array of keys that stores keys associated with node 
    int numKeysPerNode; // current number of  keys per node 
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

class Btree
{

    public:
    // private:
        int deg; //degree
        BTreeNode* root;
        int nodeCount;

        // Helper functions 
        void treat_underflow(BTreeNode* node);

        void insertParent(Key_Records key, BTreeNode *current, BTreeNode *child);
        BTreeNode *findParent( BTreeNode *current, BTreeNode *child);

        void removeRecord(int key, BTreeNode* node);

        void remove_key_in_leaf_node(BTreeNode* leafNode, int key);
        void insert_in_leaf_node(BTreeNode* leafNode, int key, vector<RecordAddress>* addressVector);


        bool tryBorrowing(BTreeNode* node1, BTreeNode* node2);

        int mergeTwoNodes(BTreeNode* node1, BTreeNode* node2);



        void remove_key_in_internal_node(BTreeNode* internalNode, int key);

        void insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, int key);
        
        void findParentAndIndex(BTreeNode* root, BTreeNode* current, BTreeNode* prev, BTreeNode* nodeToFind, BTreeNode*& parent, int& index);

        std::pair<BTreeNode*, BTreeNode*> findAdjacentSiblings(BTreeNode* root, BTreeNode* node);


        
    // public:
        Btree(unsigned int BlockSize){
            root = nullptr;
            nodeCount =0;
            deg = 3;
            unsigned int spaceForKeys = BlockSize - sizeof(int) - sizeof(bool) - sizeof(BTreeNode *);
            // deg = floor(spaceForKeys / sizeof(Key_Records) + sizeof(BTreeNode *));

        } // constructor
        void updateParentKey2(BTreeNode* parent, int oldKey, int newKey);


        void insert(int keyValue, RecordAddress recordAddress);

        std::vector<Key_Records> search(BTreeNode* node, int key, bool rangeflag, int key2); // Search for a key in the B+ tree and return associated values

        void removeRecord(int key);

        int getDegree(){
            return deg;
            };

        void setRoot(BTreeNode* newRoot) {
            root = newRoot;
            };

        BTreeNode* getRoot() const {
            return root;
            };


        void printTree(BTreeNode *current);
        BTreeNode *fetchRoot();
        //void printTree(BTreeNode* node, int level = 0);
        //void print(BTreeNode* node);

};


#endif
