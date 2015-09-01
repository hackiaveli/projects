#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;


class Node{

public:
  Node * next;
  Node(int arg_value);
  int getVal();
  void printAllValues();
  Node * deleteNode(Node *head);


private:
  int m_value;

};

Node::Node(int arg_value)
{
  m_value=arg_value;
};

Node* Node::deleteNode(Node *head)
{

  Node *cur =head;
  Node *previous=NULL;
  Node *returner=head;

  
  //our head is what we are deleting
  if(cur==this)
  {

    Node *new_head;
    new_head=head->next;
     delete head;
    return new_head;
  //return returner;
  }
  else
  {
    previous=cur;
    cur=cur->next;
    while(cur!=NULL)
    {
      if(cur==this)
      {
        previous->next=cur->next;
        delete cur;
        return head;
        //return returner;
      }

      previous = cur;
      cur=cur->next;
    }

  }
 

  return head; 

};


int Node::getVal()
{
return m_value;
}
void Node::printAllValues()
{
  Node *cur = this;
  while(cur!=NULL)
  {
    cout << cur->m_value << endl;
    cur= cur->next;
  }
}


void eraseTheseNodes(Node *head,int maxNum, int array[])
{

  Node *cur=head;
  cur=head;

  for(int i=0;i<maxNum;i++)
  {
    array[i]=0;
  }

  while(cur!=NULL)
  {
    int val = cur->getVal();
    array[val]++;
    cur=cur->next;
  }
  //return array;

};

Node * removeDups(Node *head)
{
  Node *cur=head;
  Node *new_head=head;
  int maxNum=0;
 
  while(cur!=NULL)
  {
    if(cur->getVal()>maxNum)
    {
      maxNum=cur->getVal();
    }
    cur=cur->next;
  }

  maxNum++;

  int array[maxNum];

  eraseTheseNodes(head,maxNum,array);



  cur=head;
  cur=cur->next;
  if(array[head->getVal()]>1)//if head is a dup
  {
    new_head = head->next;
    array[head->getVal()]--;
    delete head;
  }
  Node *temp;

  while(cur!=NULL)
  {
    int val = cur->getVal();
    if(array[val]>1)//need a duplicate
    {
      temp=cur;
      cur=cur->next;
      array[val]--;
      new_head=temp->deleteNode(new_head);
      
    }
    else
    {
      cur=cur->next;
    }

  }

  
  return new_head;

}

int main()
{



  Node *head= new Node(4);
  Node *second = new Node(3);
  head->next=second;
  Node *third = new Node(4);
  second->next = third;
  Node *fourth = new Node(1);
  third->next = fourth;
  Node *fifth = new Node(3);
  fourth->next=fifth;
  Node *sixth = new Node(2);
  fifth->next = sixth;
  sixth->next = NULL;
  
  head->printAllValues();

 // head = head->deleteNode(head);
  head = removeDups(head);
  cout << "new array" <<endl;
  head->printAllValues();



}

