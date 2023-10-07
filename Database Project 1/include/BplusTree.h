#ifndef BplusTree_H
#define BplusTree_H
#include "../include/Storage.h"
#include <math.h>
#include <vector>

/**
 * \struct Key_Records
 * \brief Structure to hold key-value pairs associated with nodes.
 */
struct Key_Records{
    
    float key_value; 
    vector<RecordAddress> storage_array;

};

/**
 * \struct BTreeNode
 * \brief Represents a B+ tree node.
 */

struct BTreeNode {
    bool isleaf;        ///< Flag to indicate if leaf node or internal node.
    Key_Records *keys; ///< Array of keys that stores keys associated with the node.
    int numKeysPerNode; ///< Current number of keys per node.
    BTreeNode **child;  ///< Array of child nodes.

    /**
     * \brief Constructor for the BTreeNode class.
     * \param n Maximum number of keys in a node.
     */
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


/**
 * \class Btree
 * \brief Represents a B+ tree data structure.
 */

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

            /**
             * \brief Constructor for the Btree class.
             *
             * Initializes a new B+ tree with the specified degree and disk storage.
             * \param diskStorage The storage system for the B+ tree.
             * \param degree The degree of the B+ tree.
             */

        Btree(Storage *diskStorage, int degree){
            root = nullptr;
            nodeCount =0;
            this->diskStorage = diskStorage;
            deg = degree;
            indexNodesAccessed = 0;

            // unsigned int spaceForKeys = diskStorage->getBlockSize() - sizeof(int) - sizeof(bool) - sizeof(BTreeNode *);
            // deg = floor(spaceForKeys / sizeof(Key_Records) + sizeof(BTreeNode *));

        } // constructor


            /**
             * \brief Inserts a key-value pair into the B+ tree.
             *
             * Inserts the given key and value into the B+ tree.
             * \param keyValue The key to insert.
             * \param recordAddress The corresponding record address to insert.
             */

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
            int deg;                 ///< Degree of the B+ tree.
            BTreeNode* root;         ///< Root node of the B+ tree.
            int nodeCount;          ///< Number of nodes in the B+ tree.
        
            Storage *diskStorage;  ///< Storage for the B+ tree.
            int recordCount;       ///< Number of records in the B+ tree.
            int indexNodesAccessed;///< Counter for index nodes accessed.
            int dataBocksAccessed; ///< Counter for data blocks accessed.


};


#endif
