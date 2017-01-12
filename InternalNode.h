  #ifndef InternalNodeH
  #define InternalNodeH

#include "BTreeNode.h"

class InternalNode:public BTreeNode
{
  int internalSize;
  BTreeNode **children;
  int *keys;
public:
  InternalNode(int ISize, int LSize, InternalNode *p,
    BTreeNode *left, BTreeNode *right);
  int getMinimum()const;
  int getMaximum()const;
  InternalNode* insert(int value); // returns pointer to new InternalNode
    // if it splits else NULL
  void insert(BTreeNode *oldRoot, BTreeNode *node2); // if root splits use this
  InternalNode* insert(BTreeNode *newNode); // from a sibling
  void print(Queue <BTreeNode*> &queue);
  void printnewLeaf();
  void rollRightAndInsert(BTreeNode* value, int pos);
  void rollLeft();
  InternalNode* split(BTreeNode* value);
  BTreeNode* insertValueInMiddle(int value);
  void insertNodeInMiddle(BTreeNode* value);
  void updateKeys();
}; // InternalNode class

#endif

