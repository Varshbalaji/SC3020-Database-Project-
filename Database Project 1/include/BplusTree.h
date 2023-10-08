#ifndef BplusTree_H
#define BplusTree_H
#include "../include/Storage.h"
#include <math.h>
#include <vector>


struct Key_Records{
    
    float key_value; 
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


        // Deletion Helper functions 
        void treat_underflow(BTreeNode *node);
        void removeRecord(float key, BTreeNode* node);

        bool tryBorrowing(BTreeNode* node1, BTreeNode* node2);

        float mergeTwoNodes(BTreeNode* node1, BTreeNode* node2);

        void insert_in_leaf_node(BTreeNode* leafNode, float key, vector<RecordAddress>* addressVector);

        void remove_key_in_leaf_node(BTreeNode* leafNode, float key);

        void remove_key_in_internal_node(BTreeNode* internalNode, float key);

        void insert_ChildNode_in_ParentNode(BTreeNode* parent, BTreeNode* child, float key);

        float mergeInternalNodes(BTreeNode* node1, BTreeNode* node2); 
        void insertKey_Useless(BTreeNode* node, float key); 
        bool tryBorrowingInternal(BTreeNode* node1, BTreeNode* node2);

        float fixKeys(BTreeNode *node);//Convert to float for actual experiment
        void deletePointer(BTreeNode* parent, BTreeNode* node);

        Btree(Storage *diskStorage, int degree){
            root = nullptr;
            nodeCount =0;
            this->diskStorage = diskStorage;
            deg = degree;
            indexNodesAccessed = 0;

            // unsigned int spaceForKeys = diskStorage->getBlockSize() - sizeof(int) - sizeof(bool) - sizeof(BTreeNode *);
            // deg = floor(spaceForKeys / sizeof(Key_Records) + sizeof(BTreeNode *));

        } // constructor

        void insert(float keyValue, RecordAddress recordAddres);

        //Insert Helper Functions
        void insertParent(Key_Records key, BTreeNode *current, BTreeNode *child);
        BTreeNode *findParent( BTreeNode *current, BTreeNode *child);
        Key_Records fetchSSKey(BTreeNode *current);
        void printTree(BTreeNode *current);
        BTreeNode *fetchRoot();
        void updateParentKey2(BTreeNode* parent, float oldKey, float newKey);
        void findParentAndIndex(BTreeNode* root, BTreeNode* current, BTreeNode* prev, BTreeNode* nodeToFind, BTreeNode*& parent, int& index);
        void removeRecord(float key);
        std::pair<BTreeNode*, BTreeNode*>  findAdjacentSiblings(BTreeNode* root, BTreeNode* node);

        BTreeNode  *fetchSmallestLeaf(BTreeNode *current);

        //search
        std::vector<Key_Records> search(float key, bool rangeflag, float key2); // Search for a key in the B+ tree and return associated values
        tuple<float, int> avgFG3_PCT_home( Storage *diskStorage, vector<Key_Records> search_result);

        int getDegree(){
            return deg;
        };

        int getNodeCount(){
            return nodeCount;
        };            
        int getRecordCount(){
            return recordCount;
        }; 

        int getLevels(BTreeNode* current);

        void setRoot(BTreeNode* newRoot) {
            root = newRoot;
        };

        BTreeNode* getRoot() const {
            return root;
        };
        void resetIndexNodeAndDataBlockAccessCounter(){
            indexNodesAccessed = 0;
            dataBocksAccessed = 0;
        }

        int getIndexNodeAccessCounter() {
            return indexNodesAccessed;
        }

        int getDataBocksAccessedCounter() {
            return dataBocksAccessed;
        }
        // void printTree(BTreeNode* node, int level = 0);
        void print(BTreeNode* node);
        private:
            int deg; //degree
            BTreeNode* root;
            int nodeCount;
        
            Storage *diskStorage;
            int recordCount;
            int indexNodesAccessed;
            int dataBocksAccessed;


};


#endif
