#include "../include/Storage.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "../include/BplusTree.h"

using namespace std;


// Searching on a B+ tree in C++




// // Search operation
// void Btree::searchTree(int key) {
//   if (root == NULL) {
//     cout << "Tree is empty\n";
//   } 
//   else {
//     BTreeNode *cursor = root;
//     while (cursor->IS_LEAF == false) {
//       for (int i = 0; i < cursor->size; i++) {
//         if (x < cursor->key[i]) {
//           cursor = cursor->ptr[i];
//           break;
//         }
//         if (i == cursor->size - 1) {
//           cursor = cursor->ptr[i + 1];
//           break;
//         }
//       }
//     }
//     for (int i = 0; i < cursor->size; i++) {
//       if (cursor->key[i] == x) {
//         cout << "Found\n";
//         return;
//       }
//     }
//     cout << "Not found\n";
//   }
// }



//Helper Functions

//Updates the parent node with the newly created child nodes and adds a parent if needed 

//Find parent 


// Print the tree

//Fetch root 


