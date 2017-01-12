#include <iostream>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;


LeafNode::LeafNode(int LSize, InternalNode *p,
  BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
{
  values = new int[LSize];
}  // LeafNode()



int LeafNode::getMinimum()const
{
  if(count > 0)  // should always be the case
    return values[0];
  else
    return 0;

} // LeafNode::getMinimum()

int LeafNode::getMaximum() const
{
  if(count != 0)
    return values[count-1];
  else
    return 0;
}

LeafNode* LeafNode::insert(int value)
{
  LeafNode* ptr = NULL;
  if(count < leafSize)
  {//keeps leaf in sorted order
    if(value < getMinimum())
      rollRightAndInsert(value, 0);//insert value at the front
    else if(value > getMaximum())
      values[count++] = value;//insert value at the end then increment count
    else//insert value in the middle, which would also involve rolling the elements after it to the right
      insertInMiddle(value);
  }
  else if(count == leafSize)//the leaf is full
  {
    if(leftSibling && ((LeafNode *)leftSibling)->count < leafSize)
    {//check leftSibling
      ((LeafNode *)leftSibling)->insert(getMinimum());
      rollLeft();
      --count; //removed a value from *this* to put into leftSibling
      if(value > ((LeafNode *)leftSibling)->getMaximum())
        insert(value);
      else
      {
        ((LeafNode *)leftSibling)->insert(value);
      }
    }
    else if(rightSibling && ((LeafNode *)rightSibling)->count < leafSize)
    {//check rightSibling
      ((LeafNode *)rightSibling)->insert(getMaximum());
      --count; //removed a value from *this* to put into rightSibling
      if(value < ((LeafNode *)rightSibling)->getMinimum())
        insert(value);
      else
      {
        ((LeafNode *)rightSibling)->insert(value);
      }
    }
    else //split if both siblings are full
    {
        ptr = split(value); //returns LeafNode*
    }
  }
   return ptr;

}  // LeafNode::insert()

void LeafNode::print(Queue <BTreeNode*> &queue)
{
  cout << "Leaf: ";
  for (int i = 0; i < count; i++)
    cout << values[i] << ' ';
  cout << endl;
} // LeafNode::print()

void LeafNode::rollRightAndInsert(int value, int pos)
{//pos is the position at which the value is to be inserted
  int i;
  for(i = count-1; i >= pos; i--)
    values[i+1] = values[i];
  values[pos] = value;
  ++count;
}

void LeafNode::rollLeft()
{
  int i;
  for(i = 0; i < count; i++)
    values[i] = values[i+1];
}

LeafNode* LeafNode::split(int value)
{
  int i;
  LeafNode* newLeaf = new LeafNode(leafSize, parent, this, rightSibling);

  if(rightSibling)
    ((LeafNode *)rightSibling)->setLeftSibling(newLeaf);

  rightSibling = newLeaf;

  for(i = count-1; i >= (leafSize+1)/2; i--)
  {
    newLeaf->insert(values[i]);
    --count;
  }//inserts  ceiling(leafSize) values from current leaf into new leaf

  if(newLeaf->count == 0 || value > newLeaf->getMinimum())
    newLeaf->insert(value);
  else
    insert(value);

  while(newLeaf->count < (leafSize+1)/2 && count > (leafSize+1)/2)
  {//guarantees that new leaf and *this* don't fall below ceiling(leafSize/2) integers
    newLeaf->insert(getMaximum());
    count--;
  }

  return newLeaf;
}

void LeafNode::insertInMiddle(int value)
{
  int i = count-1;
  int pos = 0;//find position at which to insert
  for( ; i >= 0; i--)
    if(values[i] < value)
    {
      pos = i+1;
      break;
    }
  rollRightAndInsert(value, pos);
}

