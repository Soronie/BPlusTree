#include <iostream>
#include "InternalNode.h"

using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
  InternalNode *p, BTreeNode *left, BTreeNode *right) :
  BTreeNode(LSize, p, left, right), internalSize(ISize)
{
  keys = new int[internalSize]; // keys[i] is the minimum of children[i]
  children = new BTreeNode* [ISize];
} // InternalNode::InternalNode()

int InternalNode::getMinimum()const
{
  if(count > 0)   // should always be the case
    return children[0]->getMinimum();
  else
    return 0;
} // InternalNode::getMinimum()

int InternalNode::getMaximum()const
{
  if(count != 0)
    return keys[count-1];/*this is enough to determine
                           which child to insert a value in; the child's (specifically, LeafNode's)
                           maximum doesn't tell us anything about which child in an InternalNode
                           should undergo an insertion; it's mainly the keys (therefore minimums) that do*/
  else
    return 0;
}

InternalNode* InternalNode::insert(int value)
{/*since this function deals with  integers, but the children of InternalNodes are nodes,
   I let LeafNode's insert() function do the work here. placement is determined by accessing the internal
   nodes' keys.  InternalNode's insert function for NODES is only accessed when a leaf is split, meaning it
   will gain a child from the split, which is why I use the variables leafCheck and InternalCheck.
   If the leafCheck isn't NULL, meaning a leaf has split, the internal node gains that child and checks if it ITSELF has split*/

  BTreeNode* leafCheck = NULL;//checks if a leaf has split
  InternalNode* internalCheck = NULL;//checks if an internal node has split

  if(value < getMinimum())
    leafCheck = children[0]->insert(value);//insert value within the first child
  else if(value > getMaximum())
    leafCheck = children[count-1]->insert(value);//insert value within the last child
                                       //count isn't incremented since no new child is born here
  else//insert value in the middle, which would also involve rolling the elements after it to the right
    leafCheck = insertValueInMiddle(value);

  updateKeys();

  if(leafCheck!=NULL)//if a leaf has split, insert the new leaf into *this* internal node
    internalCheck = insert(leafCheck);//if internal node splits, let its parent know

  return internalCheck;
} // InternalNode::insert()

void InternalNode::insert(BTreeNode *oldRoot, BTreeNode *node2)
{ // Node must be the root, and node1
  insert(oldRoot);
  children[count-1]->setParent(this);
  insert(node2);
  children[count-1]->setParent(this);
} // InternalNode::insert()

InternalNode* InternalNode::insert(BTreeNode *newNode) // from a sibling
{//essentially the exact same as LeafNode's insert()
  InternalNode* ptr = NULL;
  if(count < internalSize)
  {
    if(count == 0)
    {
      children[count] = newNode;
      count++;
    }
    else if(newNode->getMinimum() < getMinimum())
      rollRightAndInsert(newNode,  0);
    else if(newNode->getMinimum() > getMaximum())
    {
      children[count] = newNode;
      ++count;
    }
    else
      insertNodeInMiddle(newNode);
  } else if(count == internalSize)
    {
     if(leftSibling && ((InternalNode *)leftSibling)->count < internalSize)
     {//check leftSibling
       ((InternalNode *)leftSibling)->insert(children[0]);
       rollLeft();
       --count; //removed a value from *this* to put into leftSibling

       if(newNode->getMinimum() > ((InternalNode *)leftSibling)-> getMinimum())
          insert(newNode);
       else
        ((InternalNode *)leftSibling)->insert(newNode);

    }
    else if(rightSibling && ((InternalNode *)rightSibling)->count < internalSize)
    {//check rightSibling
      ((InternalNode *)rightSibling)->insert(children[count-1]);
      --count; //removed a value from *this* to put into rightSibling
      if(newNode->getMinimum() > ((InternalNode *)rightSibling)->getMinimum())
        ((InternalNode *)rightSibling)->insert(newNode);
      else
        insert(newNode);
    }
    else //split if both siblings are full
        ptr = split(newNode); //returns InternalNode*
  }

  updateKeys();
  return ptr;

} // InternalNode::insert()

void InternalNode::print(Queue <BTreeNode*> &queue)
{
  int i;

  cout << "Internal: ";
  for (i = 0; i < count; i++)
    cout << keys[i] << ' ';
  cout << endl;

  for(i = 0; i < count; i++)
    queue.enqueue(children[i]);

} // InternalNode::print()

void InternalNode::rollRightAndInsert(BTreeNode* value, int pos)
{//pos is the position at which the value is to be inserted
  int i;
  for(i = count; i >= pos; i--)
    children[i+1] = children[i];
  children[pos] = value;
  ++count;
}

void InternalNode::rollLeft()
{
  int i;
  for(i = 0; i < count; i++)
    children[i] = children[i+1];
}

InternalNode* InternalNode::split(BTreeNode* value)
{
  int i;
  InternalNode* newInternal = new InternalNode(internalSize, leafSize, parent, this, rightSibling);
  if(rightSibling != NULL)
    ((InternalNode *)rightSibling)->setLeftSibling(newInternal);
  rightSibling = newInternal;
  for(i = count-1; i >= (internalSize+1)/2; i--)
  {
    newInternal->insert(children[i]);
    --count;
  }//inserts ceiling(leafSize/2) values from current internal node into new internal node

  if(newInternal->count == 0 || value->getMinimum() > newInternal->getMinimum())
    newInternal->insert(value);
  else
    insert(value);

  while(newInternal->count <= (internalSize+1)/2 && count > (internalSize+1)/2)
  {//guarantees that newInternal and *this* don't fall below ceiling(internalSize/2) number of children
    newInternal->insert(children[count-1]);
    count--;
  }

  return newInternal;

}

BTreeNode* InternalNode::insertValueInMiddle(int value)
{
  BTreeNode* ptr = NULL;
  int i = count-1;
  int pos = 0;//find position at which to insert
  for( ; i >= 0; i--)
    if(keys[i] < value)
    {
      pos = i;/*not pos = i+1, because it risks inserting a value into
                a child whose minimum is greater than the value to be inserted*/
      break;
    }

  ptr = children[pos]->insert(value);
  return ptr;
}

void InternalNode::insertNodeInMiddle(BTreeNode* value)
{//exact algorithm in LeafNode.cpp
  int i = count-1;
  int pos = 0;
  for( ; i >= 0; i--)
    if(keys[i] < value->getMinimum())
    {
      pos = i+1;
      break;
    }

  rollRightAndInsert(value, pos);
}

void InternalNode::updateKeys()
{
  int i;
  for(i = 0; i < count; i++)
    keys[i] = children[i]->getMinimum();
}
