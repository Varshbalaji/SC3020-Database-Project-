// //Search keys in the Bplus Tree 

// #include <iostream>

// #include "BplusTree.h"

// using namespace std; 

//  std::vector<Key_Records> Btree::search(int key, bool rangeflag, int key2){

//     std::vector<Key_Records> search_result = {};

//     //Tree is empty
//     if(root == nullptr) {
//         cout<<"B+ Tree is empty\n";
//         return;
//     }

//     else{
//         BTreeNode* current = root;

//         //traverse through internal nodes 
//         while(current->isleaf == false){
//             for(int i=0; i<current->numKeysPerNode; i++){
//                 if(key < current->keys[i].key_value){
//                     current = current->child[i]; 
//                     break;
//                 }

//                 //if we reach the end of the keys in the node
//                 if(key == current->numKeysPerNode - 1){
//                     current = current->child[i+1];
//                     break;
//                 }
//             }
//         }

//         //leaf node 
//         if(!rangeflag){ //Single key query
//             for(int i = 0; i<current->numKeysPerNode; i++){
//                 if (current->keys[i].key_value == key) {
//                     cout<<"Key Found!\n";
//                     search_result.push_back(current->keys[i]);
//                     return search_result;
//                 }
//             }
//         }

//         else{ //Range query
//             for (int i = 0; i < current->numKeysPerNode; ++i) {
//                 if(current->keys[i].key_value >= key && current->keys[i].key_value <= key2){
//                     search_result.push_back(current->keys[i]);

//                     if(i==current->numKeysPerNode-1){
//                         current = current->child[i];
//                         i = 0;
//                     }
//                 }
//             }
//             return search_result;
//         }

//     }
//     return search_result;
// };

 

