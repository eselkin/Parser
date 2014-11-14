#ifndef HEAPSORT_H
#define HEAPSORT_H
#define DEBUG 1;
#include <vector>
#include <QTextStream>
using namespace std;

typedef unsigned int uint;
typedef unsigned long long int ullint;

enum HTYPE {LESSTHAN, GREATERTHAN};
template <typename T>
class heapsort
{

public:
    heapsort(const HTYPE& theType = LESSTHAN, const T& data = T()); // default is null; though that means you can't start with 0!
    heapsort(const vector<T>& data); // to start with a vector, you need to actually pass it a vector
    ~heapsort();
    heapsort<T>& operator=(const heapsort<T> &other);

    vector<T*>& theWorkingVec();

    heapsort<T>& operator >> (vector<T>& vdata); // copy the vector the vector to vdata
    heapsort<T>& operator >> (vector<ullint> &vdata);
    heapsort<T>& operator << (const T& data); // the vector one will call this one
    heapsort<T>& operator << (const vector<T>& vdata); // copy all of the pointers and make new pointers

    template <typename U, typename V>
    friend
    V& operator << (V& out, const heapsort<U>& thisheap);

    void clear();
    bool empty();

private:
    vector <T> working_vector;
    vector <ullint> idx_vector;
    HTYPE working_type;
    ullint position, positionp;
    void move_reheap();
    void heapify(int idx); // from last inserted // after each insert
    void reheapify(int parent, int end); // from root // after each swap and removal
    bool _swap(T &x, T &y);
    bool insert(const T &data); // or pointer
    void copy(const vector<T>& vdata); // copy the whole other vector
    void copyp(const vector<T *> &vdata); // or a vector of pointers
    void copy(const heapsort<T>& otherheap);
    void nukem();

};

template <typename T>
heapsort<T>::heapsort(const HTYPE& theType, const T &data)
{
    position = positionp = 0;
    working_type = theType;
    (data != T()) && (insert(data));
}

template <typename T>
heapsort<T>::heapsort(const vector<T> &data)
{
    position = positionp = 0;
}

template <typename T>
heapsort<T>::~heapsort()
{
    nukem();
}

template <typename T>
heapsort<T> &heapsort<T>::operator=(const heapsort<T> &other)
{
    // if we get a heapsort of type node<QString>* .. .etc.

    return *this;
}

template <typename T>
vector<T *>& heapsort<T>::theWorkingVec()
{
    return working_vector;
}


template <typename T>
heapsort<T> &heapsort<T>::operator >>(vector<ullint> &vdata)
{
    while (!this->empty())
        move_reheap(); // just pull off so we do the work to get the vector list
    vdata = idx_vector;
    return *this;
}

template <typename T>
void heapsort<T>::move_reheap()
{
    ullint  w_size = working_vector.size() - 1,
            temp = idx_vector[0];

    working_vector[0] = NULL; // we don't care about the data... we just want the ullint vector
    working_vector[0] = working_vector[w_size];

    idx_vector[0] = idx_vector[w_size];
    idx_vector[w_size] = temp;
    working_vector.pop_back(); // the pointer is still held at root
    reheapify(0,w_size); // from root // size will have been reduced
}



template <typename T>
bool heapsort<T>::insert(const T& data)
{
    // node pointer
    if (data) {
        T ptr = *new T(data); // make the new pointer
        working_vector.push_back(ptr); // copy the ptr
        idx_vector.push_back(positionp++); // push in the position
        heapify(working_vector.size()-1); // heapify from the start? no, heapify from the the last element?
        return true;
    }
    return false;
}

template <typename T>
void heapsort<T>::copy(const vector<T> &vdata)
{
    uint other_size = vdata.size();
    for (uint i = 0; i < other_size; i++)
        *this << vdata[i]; // copy in the data 1 by 1
}


template <typename T>
heapsort<T> &heapsort<T>::operator <<(const T &data)
{
    // node pointer
    insert(data);
    return *this;
}

template <typename T>
heapsort<T> &heapsort<T>::operator <<(const vector<T> &vdata)
{
    copy(vdata);
    return *this;
}

template <typename T>
void heapsort<T>::clear()
{
    nukem();
}


template <typename T>
void heapsort<T>::copy(const heapsort<T> &otherheap)
{
    // I don't really think this is a good idea! Unless we pull out sorted, reverse the direction and push into this list... optimal?
}

template <typename T>
void heapsort<T>::nukem()
{
    // we made everything a pointer
    for (uint i = 0; i < working_vector.size(); i++)
        if (working_vector[i])
            delete working_vector[i];
    working_vector.clear();
}


template <typename T>
bool heapsort<T>::empty()
{
    // works whether pointers or not
    return working_vector.size() == 0;
}

template <typename T>
void heapsort<T>::heapify(int idx)
{
    // gets called after we insert at the end
    // idx is the element we just added... so anything before is already heaped, so we just have to check with our parent
    int parent = (idx-1)/2;
    if (idx == parent || parent < 0 || idx > working_vector.size()-1)
        return;
    if( *working_vector[idx] < *working_vector[parent])
    {
        swap(idx_vector[parent], idx_vector[idx]);
        // strict less than heap
        _swap(working_vector[parent], working_vector[idx]);
        heapify(parent); // heaping up if we switched something above us, to see if we're smaller than its parent too
    }
}

template <typename T>
void heapsort<T>::reheapify(int parent, int end)
{
    // gets called after we remove from the root
    int leftChild = parent*2+1,
            rightChild = parent*2+2,
            smallestChild;
    if(leftChild <= end )
    {
        smallestChild = (rightChild > end) ?
                    leftChild : *working_vector[rightChild] < *working_vector[leftChild] ?
                        rightChild : leftChild;
        if ( *working_vector[smallestChild] < *working_vector[parent])
        {

            _swap(working_vector[parent],working_vector[smallestChild]);
            swap(idx_vector[parent], idx_vector[smallestChild]);
            reheapify(smallestChild, end);
        }
    }
}

template <typename T>
bool heapsort<T>::_swap(T& x, T& y)
{
    T temp = x; // node pointer
    x = y;
    y = temp;
    return true;
}

#endif // HEAPSORT_H
