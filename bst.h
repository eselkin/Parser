#ifndef BST_H
#define BST_H
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <vector>
#include "node.h"
using namespace std;

enum PRINTORDER {IN_ORDER, PRE_ORDER, POST_ORDER};
enum ERRORS {NOT_FOUND, BAD_RANGE, BAD_ROTATION };

template<typename T>
class bst
{
public:
    bst(PRINTORDER order = IN_ORDER);
    bst(const T &data, int count = 1, PRINTORDER order = IN_ORDER);
    ~bst();
    bst(const bst<T> &other);
    bst<T>& operator=(const bst<T> &other);

    bst<T>& operator<<(const T* data);
    bst<T>& operator<<(const vector<T *> &data);

    bst<T>& operator<<(const bst<T> &other);//Merge a tree into another tree
    vector<T *> thebstroot() const;
    vector<T *>& thebstroot();
    void clear();
    bool empty();
    bool find(const T &data);
    int nodeCount(); // use the private, return the value
    int dataCount(); // use the private...

    int depth(); // since no access to vector bstroot
    void insert(const T *data, int c = 1);
    T remove(const T &data, int c = 1); // will use findRightMost! and findParent
    void setPrintOrder(PRINTORDER order);

    PRINTORDER getPrintOrder() const;

    template<typename U, typename V>
    friend
    V& operator<<(V& out, const bst<U> &tree);

    template<typename U, typename V>
    friend
    V& operator>>(V &in, bst<U> &tree);

    template <typename U>
    friend
    int max(int lhs, int rhs);

    QTextStream& print(QTextStream &out, int idx);


private:
    vector<T *> bstroot; // A vector of pointers of T type // NULL if no child, etc!
    int lastInserted;

    // A Parent has children that are 2n+1 and 2n+2 ahead of it in index and the parent of where you are (child) is myloc/2.. which is easy for bstroot's spot at 0
    PRINTORDER printOrder;

    void nukem();
    void copy(const bst<T> &other);
    void copy(const vector<T *> &v);
    int depth(int idx); // return the number of levels we've traversed
    int nodeCount(int from_idx); // count the number of nodes
    int dataCount(int from_idx); // count the items in the number of nodes // why not keep a tally? faster for multiple look ups of large trees!
    int find(int from_idx, const T &data); // find the node with the data we're looking for

    int findParent(int idx); // find the parent of the data we're looking for
    int findRightMost(int from_idx); // same thing as remove max and set it as the bstroot of whatever node you just deleted... in remove

    void rotateRight(int rotate_from);  // rotate right from the node
    void rotateLeft(int rotate_from);   // rotate left from node
    void doubleRotateRight(int idx); // rotate right child->left, rotate r right with new right child;
    void doubleRotateLeft(int idx);
    int height(int at_idx);

    void insert(const T& data, int count, int at_idx); // private helper
    void reinsertpre(int idx, int new_idx);
    void reinsertpostleft(int idx, int new_idx);
    void reinsertpostright(int idx, int new_idx);
    void addDepth(int idx);
    void reinsertpreright(int idx, int new_idx);
    void reinsertpreleft(int idx, int new_idx);
};

// Construct ordered, empty BST
template<typename T>
bst<T>::bst(PRINTORDER order)
{
    bstroot.push_back(NULL); // where we really start adding
    bstroot.push_back(NULL); // where we really start adding
    bstroot.push_back(NULL); // where we really start adding
    // the extra layer, the bstroot node does not exist... or it's a NULL pointer
    printOrder = order;
}

// Construct bstroot only, ordered BST
template<typename T>
bst<T>::bst(const T &data, int count, PRINTORDER order)
{
    bstroot.push_back(NULL);
    bstroot.push_back(new node<T>(data)); // at position 1
    bstroot[0]->count = count;
    bstroot.push_back(NULL);
    bstroot.push_back(NULL); // the extra layer, we will test this!
    printOrder = order;
}

// Kill everything
template<typename T>
bst<T>::~bst()
{
    nukem();
}

// Copy constructor
template<typename T>
bst<T>::bst(const bst<T> &other)
{
    copy(other);
}

// Assignment handling
template<typename T>
bst<T>& bst<T>::operator=(const bst<T> &other)
{
    if(this != &other)
    {
        nukem();
        copy(other);
    }
    return *this;
}

template<typename T>
bst<T>& bst<T>::operator<<(const T* data)
{
    if (data != NULL)
        insert(data);
    return *this;
}

template<typename T>
bst<T> &bst<T>::operator<<(const vector<T *> &data)
{
    uint datasize = data.size();
    for (uint i = 0; i < datasize; i++)
        *this << data[i];
    return *this;
}

template<typename T>
bst<T>& bst<T>::operator<<(const bst<T> &other)//Merge a tree into another tree
{
    // Have fun!!!! Inputting a tree can be 'real easy'

}

template<typename T>
vector<T *> bst<T>::thebstroot() const
{
    return bstroot;
}

template<typename T>
vector<T *> &bst<T>::thebstroot()
{
    return bstroot; // non const version
}

// Public access to nukem
template<typename T>
void bst<T>::clear()
{
    nukem();
}

// Public access to whether bstroot has node assigned to it
template<typename T>
bool bst<T>::empty()
{
    return bstroot.size() <= 1? true : false;
}

// Public access to find
template<typename T>
bool bst<T>::find(const T &data)
{
    return find(1,data) == -1? false:true;
}


// private returns the location in the vector
template<typename T>
int bst<T>::find(int from_idx, const T &data)
{
    if (from_idx >= bstroot.size() || from_idx <= 0)
        throw BAD_RANGE;
    // if it's not NULL and it is the data
    if( bstroot[from_idx] && *bstroot[from_idx] == *data)
        return from_idx;
    else
        if(bstroot[from_idx] && *bstroot[from_idx] > *data)
            return find(2*from_idx, data); // RIGHT <, REALLY LEFT
        else if(bstroot[from_idx] && *bstroot[from_idx] < *data)
            return find(2*from_idx+1, data); // LEFT >, REALLY RIGHT
    return -1;
}

template<typename T>
int bst<T>::nodeCount()
{
    return nodeCount(1);
}

template<typename T>
int bst<T>::dataCount()
{
    return dataCount(1);
}

template<typename T>
int bst<T>::nodeCount(int from_idx)
{
    return bstroot[from_idx] ? 1 + nodeCount(2*from_idx) + nodeCount(2*from_idx+1) : 0;
}

template<typename T>
int bst<T>::dataCount(int from_idx)
{
    return bstroot[from_idx] ? bstroot[from_idx]->theCount() + dataCount(2*from_idx) + dataCount(2*from_idx+1) : 0;
}

template<typename T>
int bst<T>::depth()
{
    return log10(1.*lastInserted)/log10(2.); // just the integer portion
}

template<typename T>
void bst<T>::insert(const T *data, int count)
{
    // available to public, no need to know bstroot
    insert(*data, count, 1);
}

template<typename T>
void bst<T>::insert(const T& data, int count, int at_idx)
{
    addDepth(2*at_idx+1);
    if(!bstroot[at_idx])
    {

        bstroot[at_idx] = new T(data); // T is a node pointer
        bstroot[at_idx]->height = 0;
        return; // height already assessed, move on to height of parents
    }
    else if (bstroot[at_idx] && (data == *bstroot[at_idx]))
    {
        *bstroot[at_idx] += data; // no height change or rebalancing needed
        return; // height does not change
    }
    else if (bstroot[at_idx] && (data < *bstroot[at_idx]))
    {   // height will change on the right!
        insert(data, count, 2*at_idx); // the recursive call, only returns .. this must exist.

        if ((height(2*at_idx) - height(2*at_idx+1)) == 2){
            if (bstroot[2*at_idx] && (data < *bstroot[2*at_idx])) // if data chain continued to the right, we rotate left
            {
                rotateLeft(at_idx);
            }
            else if (bstroot[2*at_idx])// if data went left after right, we double rotate left
            {
                doubleRotateLeft(at_idx);
            }
        }
    }
    else if (bstroot[at_idx] && data > *bstroot[at_idx])
    {   // height will change on the left!
        insert(data, count, (2*at_idx+1)); // the recursive call, only returns
        if (height(2*at_idx+1) - height(2*at_idx) == 2) {
            if (bstroot[(2*at_idx+1)] && data > *bstroot[(2*at_idx+1)])
            {
                rotateRight(at_idx); // LEFT LEFT
            }
            else if (bstroot[(2*at_idx+1)]){
                doubleRotateRight(at_idx); // LEFT RIGHT
            }
        }
    }
    if (at_idx != 0 && bstroot[at_idx])
        bstroot[at_idx]->theHeight() = max(height(2*at_idx), height(2*at_idx+1))+1;
}


template <typename T>
void bst<T>::addDepth(int idx)
{
    int current_depth = depth(idx);
    while (bstroot.size() < pow(2, current_depth)+1)
        bstroot.push_back(NULL); // put NULL POINTERS IN AT THE NEXT ROW, UP TO THE NUMBER NEEDED FOR ALL THE ELEMENTS THAT COULD BE A DEPENDENT OF THIS ROW, IF THEY DON'T EXIST
}

template <typename T>
int bst<T>::height(int at_idx)
{
    return ((bstroot.size()-1 < at_idx) || !bstroot[at_idx]) ? -1 : bstroot[at_idx]->theHeight();
    // If we have no node yet, the height below it is -1; Means doesn't exist
    // if we have a node, it's height is the height of the tree below it
}

template<typename T>
int bst<T>::findParent(int idx)
{
    return (bstroot.size() >= idx && bstroot[idx]) ? (idx/2) : -1;
}

template<typename T>
int bst<T>::findRightMost(int idx)
{
    return ((bstroot.size() >= 2*idx) && bstroot[2*idx]) ? findRightMost(2*idx) : idx;
}


/*
 * Rotate r's right child with it's Left child (LEFT ROTATE), then rotate r with it's new right child (ROTATE RIGHT)
 * Doubly Rotate right! we call it!
 *    r      r            s
 *     \      \          / \
 *      t =>   s  =>    r   t
 *     /        \
 *    s          t
 */
template<typename T>
void bst<T>::doubleRotateRight(int idx)
{
    rotateLeft(2*idx+1);
    rotateRight(idx);
}

/*
 * Rotate r's left child with it's right child (RIGHT ROTATE), then rotate r with it's new left child (ROTATE LEFT)
 * Doubly Rotate right! we call it!
 *
 *    r       r       q
 *   /       /       / \
 *  p  =>   q   =>  p   r  look, the height of q changes to r's height, and p and r change to p's height
 *   \     /
 *    q   P
 */
template<typename T>
void bst<T>::doubleRotateLeft(int idx)
{
    rotateRight(2*idx);
    rotateLeft(idx);

}

/* COUNTERCLOCKWISE
 * Rotate right! we call it!
 *     *********Rotate (r) with (it's LEFT child) (Remember, left is right here) ********
 *     Also, will work with internal rotations, because we keep track of r's parent to reattach
 *     it to the subtree to which it belongs
 *  ADDED 12...........                        ADD 13....                                                       ADD 10......
 *        6                      6                 6                                             _6_                    _6_         UNBALANCED HERE 3:1         __6__   THEN ROTATE RIGHT AT 6          ___8___
 *      /   \                  /   \             /   \                                         /     \                /     \          but, at 2*1+1           /     \                                 /       \
 *     5     8                5     8           5     8   unbalanced at this level!!    =>    r5     11              5       11        10 < 11 so double      5       8                               6        11
 *    / \   / \       =>     / \   / \   =>    / \   / \                                     / \    /   \           / \     /  \        rotate, 11 left      / \     /  \                             / \       / \
 *   1   2 7   9            1   2 7  11       1   2 7   11   Not unbalanced at this lvl    m1  n2  8    12         1   2   8   12                           1   2   7    11                          5   7     9   12
 *              \                    / \               / \                                        / \     \               / \    \                         /\  /\  / \   /  \                       / \ /  \    \    \
 *               11                 9   12             9  12  Not unbalanced at this lvl         7   9    13             7   9   13                              6.5 7.5 9  12                     1  2 6.5 7.5  10   13
 *                \                                        \                                                                  \                                        \     \
 *                 12                                       13                                                                 10                                       10   13  (12-(int)log2(7-1)) = 10
 *                                                                                                                                                                              (6-(int)log2())
 *
 * LOOKING at the last rotation, 6's left children get moved down, 5 moves from position 2 to position 2*2  6 moves to position 2
 * BUT REALLY THERE IS ONLY THE FORM
 * u               v
 *  \             / \             given idx u
 *   v      =>   u   x           u           v
 *    \                           \    =>   /
 *     x                           v       u
 */

template <typename T>
void bst<T>::rotateRight(int rotate_from)
{
    int rsChild = 2*rotate_from+1; // promoting left to right (counter clockwise, even though it sounds like clockwise)
    int rsRight = 2*rotate_from;
    if (bstroot[rsChild] == NULL)     // Make sure we can rotate the way we're rotating
        return;
    // I have to increment the right subtree POSTORDER, then the bstroot down to the right
    if (bstroot[rsRight]) reinsertpostright(rsRight, 2*rsRight); // right more right
    bstroot[rsRight] = bstroot[rotate_from];
    bstroot[rotate_from] = NULL;
    // Now reinsert in preorder from the right subtree of the left child
    if (bstroot[2*rsChild]) reinsertpreleft(2*rsChild, 2*rsRight+1); // Left's right to rotate_from's right's left
    // here is where we need to adjust the height... since old bstbstroot may now be less than before
    // height of the reinsert pre should stay the same and height of the reinsert post should stay the same
    // height of the old bstroot should just be the max of those two
    // height of the new bstroot will stay the same
    bstroot[rsRight]->theHeight() = max(height(2*rsRight), height(2*rsRight+1)) + 1;
    // Move the left child's chain up and right in PREORDER

    if (bstroot[rsChild]) reinsertpreright(rsChild, rotate_from); // pulls the left chain up to start from bstroot
    bstroot[rotate_from]->theHeight() = max(height(rsRight), height(rsChild)) + 1;
}


/* CLOCKWISE
 * Rotate left! we call it!
 *     *********Rotate (r) with (it's RIGHT child) (Remember, right is left here) ********
 *     Also, will work with internal rotations, because we keep track of r's parent to reattach
 *     it to the subtree to which it belongs
 *
 *       u             s
 *      / \          /   \   we just added in m ... and will rotate at t
 *     s   v  =>    r     u
 *    / \          /     / \
 *   r   t        m     t   v
 *  /
 * m
 *
 *        t    s
 *       /  =>  \
 *      s        t
 *
 */
template <typename T>
void bst<T>::rotateLeft(int rotate_from)
{
    int rsChild = 2*rotate_from; // promoting right to left (clockwise, even though it sounds like counterclockwise)
    int rsLeft = 2*rotate_from+1;
    if (bstroot[rsChild] == NULL)     // Make sure we can rotate the way we're rotating
        return;

    // I have to increment the spots left in POSTORDER
    reinsertpostleft(rsLeft, 2*rsLeft+1); // Left more left
    bstroot[rsLeft] = bstroot[rotate_from];
    bstroot[rotate_from] = NULL;
    // Now reinsert in preorder from the right subtree of the left child
    if (bstroot[2*rsChild+1]) reinsertpreright(2*rsChild+1, 2*rsLeft); // right's left subtree => rotate_from's left's right
    if (bstroot[rsLeft]) bstroot[rsLeft]->theHeight() = max(height(2*rsLeft), height(2*rsLeft+1)) + 1;
    // Move the right child's chain up and left in PREORDER
    if (bstroot[rsChild]) reinsertpreleft(rsChild, rotate_from); // pulls the left chain up to start from bstroot
    bstroot[rotate_from]->theHeight() = max(height(rsLeft), height(rsChild)) + 1;
}

// reinsert a subtree preorder so it goes in in the same structure, we can even move up and left on the same tree by following a preorder and have it keep our structure
// but move its space
template <typename T>
void bst<T>::reinsertpreright(int idx, int new_idx)
{
    // FOLLOW A PRE ORDER TRAVERSAL
    if (bstroot[idx])
    {
        bstroot[new_idx] = bstroot[idx]; // copy the pointer from the bstroot
        bstroot[idx] = NULL; // clear up the other space
    }
    if (bstroot.size() >= 2*idx && bstroot[2*idx])
        reinsertpreright(2*idx, 2*new_idx); // move the old index's right to the new index's right
    if (bstroot.size() >= 2*idx+1 && bstroot[2*idx+1])
        reinsertpreright(2*idx+1, 2*new_idx+1); // and the same for left
}

template <typename T>
void bst<T>::reinsertpreleft(int idx, int new_idx)
{
    addDepth(2*idx+1); // make space if necessary
    // useful when copying from a right node to a left node
    // useful when pulling a tree off a subtree and attaching it to another subtree.
    // FOLLOW A PRE ORDER TRAVERSAL
    if (bstroot[idx])
    {
        bstroot[new_idx] = bstroot[idx]; // copy the pointer from the bstroot
        bstroot[idx] = NULL; // clear up the other space
    }
    if (bstroot.size() >= 2*idx+1 && bstroot[2*idx+1])
        reinsertpreleft(2*idx+1, 2*new_idx+1); // and the same for left
    if (bstroot.size() >= 2*idx && bstroot[2*idx])
        reinsertpreleft(2*idx, 2*new_idx); // move the old index's right to the new index's right
}

template <typename T>
void bst<T>::reinsertpostleft(int idx, int new_idx)
{
    addDepth(2*idx+1); // make space if necessary
    // FOLLOW A POST ORDER TRAVERSAL, this is useful when pushing the tree down and to the right
    // I think it will also be useful when pulling a tree from the left of something to the right of something.

    if (bstroot.size() >= 2*idx+1 && bstroot[2*idx+1])
        reinsertpostleft(2*idx+1, 2*new_idx+1); // and the same for left
    if (bstroot.size() >= 2*idx && bstroot[2*idx])
    {
        reinsertpostleft(2*idx, 2*new_idx); // move the old index's right to the new index's right
    }
    if (bstroot.size() >= idx &&  bstroot[idx])
    {
        bstroot[new_idx] = bstroot[idx]; // copy the pointer from the bstroot
        bstroot[idx] = NULL; // clear up the other space
    }


}

template <typename T>
void bst<T>::reinsertpostright(int idx, int new_idx)
{
    addDepth(2*idx+1); // make space if necessary
    // FOLLOW A POST ORDER TRAVERSAL, this is useful when pushing the tree down and to the right
    // I think it will also be useful when pulling a tree from the left of something to the right of something.

    if (bstroot.size() >= 2*idx && bstroot[2*idx])
    {
        reinsertpostright(2*idx, 2*new_idx); // move the old index's right to the new index's right
    }
    if (bstroot.size() >= 2*idx+1 && bstroot[2*idx+1])
        reinsertpostright(2*idx+1, 2*new_idx+1); // and the same for left

    if (bstroot.size() >= idx &&  bstroot[idx])
    {
        bstroot[new_idx] = bstroot[idx]; // copy the pointer from the bstroot
        bstroot[idx] = NULL; // clear up the other space
    }


}

template<typename T>
T bst<T>::remove(const T &data, int count)
{
    bool right = false, left = false;
    int remove_idx = find(1,data);
    int parent_idx = findParent(remove_idx); // if remove_idx = 1, parent_idx = 0
    (2*parent_idx == remove_idx) ? (right = true) : (left = true); // 2*0 != 1, so left will be true

    if (remove_idx == -1)
        throw NOT_FOUND;

    if (bstroot[remove_idx])
    {
        if ((bstroot[remove_idx]->theCount()) - count <= 0)
        {
            delete bstroot[remove_idx];
            bstroot[remove_idx] = NULL;
        }
        else
        {
            bstroot[remove_idx]->theCount()-=count;
            return data; // Still has data at remove_idx
        }
    }
    // ONLY happens if we have removed remove_idx!!!!
    if (bstroot[2*remove_idx] && bstroot[2*remove_idx+1])
    {
        // we have right and left data from where remove was located
        // get new index for remove_idx's right chain
        int leftsRightmost = findRightMost(2*remove_idx+1);
        reinsertpre(2*remove_idx, 2*leftsRightmost); // move the right subtree of remove_idx to the left's rightmost as a bstroot
        reinsertpre(2*remove_idx+1, remove_idx); // move up the whole tree to the bstroot location;
    } else
    {
        if (bstroot[2*remove_idx])
            reinsertpre(2*remove_idx, remove_idx);
        else
            reinsertpre(2*remove_idx+1, remove_idx);
    }
    // change height... since we removed from under the parent
    if (bstroot[remove_idx])
        bstroot[remove_idx]->theHeight() = max(height(2*remove_idx),height(2*remove_idx+1))+2;
    return data;
}

template<typename T>
void bst<T>::setPrintOrder(PRINTORDER order)
{
    printOrder = order;
}

template<typename T>
PRINTORDER bst<T>::getPrintOrder() const
{
    return printOrder;
}


template<typename T>
void bst<T>::nukem()
{
    bstroot.clear(); // CLEAR ITERATORS
}

template<typename T>
void bst<T>::copy(const bst<T> &other)
{
    if(other.thebstroot().size() > 1)
        copy(other.bstroot);
}

template<typename T>
void bst<T>::copy(const vector<T*> &v)
{
    for (uint i = 0; i < v.size(); i++)
        (bstroot.push_back(v[i]));
}

template<typename T>
int bst<T>::depth(int idx)
{
    return log10(1.*idx)/log10(2.);
}

template<typename T>
QTextStream& bst<T>::print(QTextStream &out, int idx)
{
    printOrder = IN_ORDER; // FORCE THE SORT ORDER!

    if (idx >= bstroot.size())
        return out;
    if(bstroot[idx] && bstroot[2*idx])
        print(out, 2*idx);
    if(bstroot[idx] && printOrder == IN_ORDER)
        out<< bstroot[idx]->theData().toUtf8().constData() << "\t#:" << bstroot[idx]->theCount() << endl;
    if(bstroot[idx] && bstroot[2*idx+1])
        print(out, 2*idx+1);
    return out;
}

template<typename U, typename V>
V& operator<<(V &out, bst<U> &tree)
{
    tree.print(out, 1);
    return out;
}

template<typename U, typename V>
V& operator>>(V &in, bst<U*> &tree)
{
    tree.bstroot.clear();
    string line;
    U value;
    int idx, count, height;
    PRINTORDER theOrder;
    stringstream ss;

    getline(in,line);
    if (line == "PRE_ORDER")
        theOrder = PRE_ORDER;
    else if (line == "POST_ORDER")
        theOrder = POST_ORDER;
    else
        theOrder = IN_ORDER;
    tree.setPrintOrder(theOrder);
    while (line != "")
    {
        // process line
        getline(in, line);
        ss.str().clear();
        ss.clear();
        ss << line.substr(0,line.find(":::")).c_str();
        ss>>idx;
        //        tree.addDepth(2*idx);
        line.erase(0,line.find(":::")+3);
        //        cout << "line" << line << endl;
        cout << "IDX:" << setw(20) <<  idx;
        ss.str().clear();
        ss.clear();
        ss << line.substr(0,line.find(":::")).c_str();
        ss >> value;
        cout << "\tVAL:" << setw(20) << value;
        ss.clear();
        ss.str().clear();
        line.erase(0,line.find(":::")+3);
        //        cout << "line" << line << endl;
        ss << line.substr(0,line.find(":::")).c_str();
        ss >> count;
        cout << "\tCNT:" << setw(20)<< count;
        ss.clear();
        ss.str().clear();
        line.erase(0,line.find(":::")+3);
        //        cout << "line" << line << endl;
        ss << line.c_str();
        ss >> height;
        cout << "\tHGT:" << setw(20) << height << endl;
        while(tree.bstroot.size() <= idx)
            tree.bstroot.push_back(NULL);
        cout << "CURRENT IDX: " << idx << "TREE SIZE: " << tree.bstroot.size() << endl;
        tree.bstroot[idx]=new node<U>(value, count, height);
        tree.addDepth(2*idx);
    }
    return in;
}

template<typename U>
int max(int lhs, int rhs)
{
    return lhs > rhs? lhs : rhs; // it does make sense, if left hand side is bigger it gets returned, otherwise rhs is returned because it is max??? no, it's not! it may be equal!
}

#endif // BST_H
