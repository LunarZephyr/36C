#include <iostream>
#include <climits>
#include "InternalNode.h"

using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
  InternalNode *p, BTreeNode *left, BTreeNode *right) :
  BTreeNode(LSize, p, left, right), internalSize(ISize)
{
  keys = new int[internalSize]; // keys[i] is the minimum of children[i]
  children = new BTreeNode* [ISize];
} // InternalNode::InternalNode()


BTreeNode* InternalNode::addPtr(BTreeNode *ptr, int pos)
{ // called when original was full, pos is where the node belongs.
  if(pos == internalSize)
    return ptr;

  BTreeNode *last = children[count - 1];

  for(int i = count - 2; i >= pos; i--)
  {
    children[i + 1] = children[i];
    keys[i + 1] = keys[i];
  } // shift things to right to make room for ptr, i can be -1!

  children[pos] = ptr;  // i will end up being the position that it is inserted
  keys[pos] = ptr->getMinimum();
  ptr->setParent(this);
  return last;
} // InternalNode:: addPtr()


void InternalNode::addToLeft(BTreeNode *last)
{
  ((InternalNode*)leftSibling)->insert(children[0]);

  for(int i = 0; i < count - 1; i++)
  {
    children[i] = children[i + 1];
    keys[i] = keys[i + 1];
  }

  children[count - 1] = last;
  keys[count - 1] = last->getMinimum();
  last->setParent(this);
  if(parent)
    parent->resetMinimum(this);
} // InternalNode::ToLeft()


void InternalNode::addToRight(BTreeNode *ptr, BTreeNode *last)
{
  ((InternalNode*) rightSibling)->insert(last);
  if(ptr == children[0] && parent)
    parent->resetMinimum(this);
} // InternalNode::addToRight()

void InternalNode::addToThis(BTreeNode *ptr, int pos)
{  // pos is where the ptr should go, guaranteed count < internalSize at start
  int i;

  for(i = count - 1; i >= pos; i--)
  {
      children[i + 1] = children[i];
      keys[i + 1] = keys[i];
  } // shift to the right to make room at pos

  children[pos] = ptr;
  keys[pos] = ptr->getMinimum();
  count++;
  ptr->setParent(this);

  if(pos == 0 && parent)
    parent->resetMinimum(this);
} // InternalNode::addToThis()

int InternalNode::getMaximum() const
{
  if(count > 0) // should always be the case
    return children[count - 1]->getMaximum();
  else
    return INT_MAX;
}  // getMaximum();


int InternalNode::getMinimum()const
{
  if(count > 0)   // should always be the case
    return children[0]->getMinimum();
  else
    return 0;
} // InternalNode::getMinimum()


InternalNode* InternalNode::insert(int value)
{  // count must always be >= 2 for an internal node
  int pos; // will be where value belongs

  for(pos = count - 1; pos > 0 && keys[pos] > value; pos--);

  BTreeNode *last, *ptr = children[pos]->insert(value);
  if(!ptr)  // child had room.
    return NULL;

  if(count < internalSize)
  {
    addToThis(ptr, pos + 1);
    return NULL;
  } // if room for value

  last = addPtr(ptr, pos + 1);

  if(leftSibling && leftSibling->getCount() < internalSize)
  {
    addToLeft(last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < internalSize)
    {
      addToRight(ptr, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(last);
} // InternalNode::insert()


void InternalNode::insert(BTreeNode *oldRoot, BTreeNode *node2)
{ // Node must be the root, and node1
  children[0] = oldRoot;
  children[1] = node2;
  keys[0] = oldRoot->getMinimum();
  keys[1] = node2->getMinimum();
  count = 2;
  children[0]->setLeftSibling(NULL);
  children[0]->setRightSibling(children[1]);
  children[1]->setLeftSibling(children[0]);
  children[1]->setRightSibling(NULL);
  oldRoot->setParent(this);
  node2->setParent(this);
} // InternalNode::insert()


void InternalNode::insert(BTreeNode *newNode)
{ // called by sibling so either at beginning or end
  int pos;

  if(newNode->getMinimum() <= keys[0]) // from left sibling
    pos = 0;
  else // from right sibling
    pos = count;

  addToThis(newNode, pos);
} // InternalNode::insert(BTreeNode *newNode)


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


BTreeNode* InternalNode::remove(int value)
{  // to be written by students
  int borrowed;
  int check = 0;
  int internalLimit;
  int index = NULL;
  BTreeNode *ptr;
  InternalNode *plt;
  if(internalSize%2 != 0)
    internalLimit = (internalSize/2)+1;
  else
    internalLimit = internalSize/2;

  for(int i = 0; i < count; i++)
  {
    //removed the value from the tree
    if(value >= keys[i] && value < keys[i+1])
    {
      children[i]->remove(value);
      //children[i]->removeFromRight(value);

      break;
    }
    else if(value >= keys[count-1])
    {
      children[count-1]->remove(value);
      break;
    }
  }
  //reindexes keys and removed NULL key
  for (int i = 0; i < count; i++)
  {
    if(keys[i] == 0)
    {
      index = i;
    } 
  }
  if (keys[index] == 0) 
  {
    for(int k = index; k<count;k++)
    {
      keys[k] = keys[k+1];
      children[k] = children[k + 1];
    }
    index = NULL;
    count--;
  }

  if(this->getCount() < internalLimit)
  {
    check = borrow();
  }
  if(parent && parent->getCount() == 1)
    parent->resetMinimum(this);
  if (count == 0)
  {
    keys = NULL;
    children = NULL;
    this->setRightSibling(NULL);
  }

  if (count == 1) {
    return children[0];
  }    

  return NULL; // filler for stub
} // InternalNode::remove(int value)

void InternalNode::resetMinimum(const BTreeNode* child)
{
  for(int i = 0; i < count; i++)
    if(children[i] == child)
    {
      keys[i] = children[i]->getMinimum();
      if(i == 0 && parent)
        parent->resetMinimum(this);
      break;
    }
} // InternalNode::resetMinimum()


InternalNode* InternalNode::split(BTreeNode *last)
{
  InternalNode *newptr = new InternalNode(internalSize, leafSize,
    parent, this, rightSibling);

  if(rightSibling)
    rightSibling->setLeftSibling(newptr);

  rightSibling = newptr;

  for(int i = (internalSize + 1) / 2; i < internalSize; i++)
  {
    newptr->children[newptr->count] = children[i];
    newptr->keys[newptr->count++] = keys[i];
    children[i]->setParent(newptr);
  }

  newptr->children[newptr->count] = last;
  newptr->keys[newptr->count++] = last->getMinimum();
  last->setParent(newptr);
  count = (internalSize + 1) / 2;
  return newptr;
} // split()

int InternalNode::borrow()
{
  BTreeNode *ptr;
  int borrowed;
  int internalLimit;
  if(internalSize%2 != 0)
    internalLimit = (internalSize/2)+1;
  else
    internalLimit = internalSize/2;


  if(leftSibling && leftSibling->getCount() > internalLimit)
  {
    addToThis(leftSibling, 0);
    borrowed = leftSibling->getMaximum();
    this->addPtr(ptr, 0);
    if(parent)
      parent->resetMinimum(this);
    return (0);
  }
  else if(leftSibling && leftSibling->getCount() <= internalLimit)
  {
    merge(0);
    return (1);
    if (parent)
      parent->resetMinimum(this);
  }
  else if(rightSibling && rightSibling->getCount() > internalLimit)
  {
    //addToThis(rightSibling, count);
    borrowed = rightSibling->getMinimum();
    int number = rightSibling->getCount();
    InternalNode* sibling = ((InternalNode*)rightSibling);
    ptr = sibling->removeFromRight(borrowed, parent);
    addPtr(ptr, count);

    if(parent)
      parent->resetMinimum(this);
    return(0);
  }
  
  else if(rightSibling && rightSibling->getCount() <= internalLimit)
  {
    merge(1);
    return(1);
    if (parent)
      parent->resetMinimum(this);
  }
}

InternalNode* InternalNode::removeFromRight(int value, InternalNode* parent)
{
  BTreeNode *ptr;
  for(int i= 0; i < count; i++)\
  {
    if(keys[i] == value)
    {
      addToLeft(children[i]);    
      break;
    }
  }
  int index;
  for(int i = 0; i < count; i++)
  {
    if (keys[i] == value)   
      index = i;
  }
  ptr = children[index];

  for(int i = index; i < count; i++)
  {
    children[i] = children[i + 1];
    keys[i] = keys[i + 1];
  }
  --count;
  this->setParent(parent);
  if(parent)
    parent->resetMinimum(this);

  return ((InternalNode*)ptr);

  //ptr->setParent((InternalNode*)this->getLeftSibling());
} // InternalNode::ToLeft()

InternalNode* InternalNode::removeFromLeft(int value, InternalNode* parent)
{
  BTreeNode *ptr;
  for(int i= 0; i < count; i++)\
  {
    if(keys[i] == value)
    {
      addToRight(this->getLeftSibling(), children[i]);    
      break;
    }
  }

  int index;
  for(int i = 0; i < count; i++)
  {
    if (keys[i] == value)   
      index = i;
  }
  ptr = children[index];

  for(int i = index; i < count; i++)
  {
    children[i] = children[i + 1];
    keys[i] = keys[i + 1];
  }
  --count;
  this->setParent(parent);
  if(parent)
    parent->resetMinimum(this);

  return ((InternalNode*)ptr);

  //ptr->setParent((InternalNode*)this->getLeftSibling());
} // InternalNode::ToLeft()

void InternalNode::merge(int x)
{
  int index;
  int value;
  if(x==0)//merge left
  {
    for(int i = 0; i < count; i++ )
    {
      this->addToLeft(children[0]);
      value = this->getMinimum();
      for (int i = 0; i < count; i++)
      {
        if(keys[i] == value)
        {
          index = i;
          keys[i] = NULL;
        } 
      }
      for(int k = index; k<count;k++)
        keys[k] = keys[k+1];

      if(parent)
        parent->resetMinimum(this);
    }
    count = 0;
    if(leftSibling)
      leftSibling->setRightSibling(rightSibling);
  }
  else if(x==1)//merge right
  {
    for(int i = 0; i < count; i++ )
    {
      addToRight(rightSibling, children[count - i - 1]);
      value = this->getMaximum();
      for (int i = 0; i < count; i++)
      {
        if(keys[i] == value)
          keys[i] = 0;
      }

      if(parent)
        parent->resetMinimum(this);
    }
    keys = 0;
    parent->resetMinimum(this);
    count = NULL;
    if(rightSibling)
      rightSibling->setLeftSibling(leftSibling);
  }
}