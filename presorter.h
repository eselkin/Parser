#ifndef PRESORTER_H
#define PRESORTER_H
#include "heapsort.h"
#include "bst.h"
#include "node.h"
#include "Parser.h"
#include <vector>
#include <QChar>
#include <QString>
#include <QElapsedTimer>

using namespace std;

enum PROCESSTYPE{HEAP,AVL};
enum PARSE_ERRORS { OUT_OF_BOUND };

template <typename T = node<QString>* >
class presorter {

public:
    presorter( PROCESSTYPE proc = HEAP); // default to faster
    ~presorter();
    vector< vector < ullint > >& theReferences();
    vector< vector < node<T> * > >& theAlphas();
    int getalpha(const QChar& first_char);
    presorter<T>& operator << (PROCESSTYPE thetype); // change the type from HEAP
    presorter<T>& operator << (const vector< node<T> *> &inputv );
    presorter<T>& operator >> (vector< vector<ullint> > &outputv);
    presorter<T>& operator >> (vector<bst<node<T> > > &outputv);
    vector<bst<node<T> > > &theBST();
    void clear();
    double time_last_sort, time_to_parse;

private:
    PROCESSTYPE theprocess;
    vector< bst< node<T> > > bsts;        // the orchard of sbbsts
    vector< Heap< node<T>* > > heaps;  // the orchard of heapsorts

    vector< vector < node<T> *> > alphavectors; // (0-9A)-Z
    vector< vector < ullint > > references;
    void nukem();
};


template <typename T>
presorter<T>::presorter( PROCESSTYPE proc )
{
    theprocess = proc;
}

template <typename T>
presorter<T>::~presorter()
{
    nukem();
}

template <typename T>
vector<vector<ullint> > &presorter<T>::theReferences()
{
    return references;
}

template <typename T>
vector<vector<node<T> *> > &presorter<T>::theAlphas()
{
    return alphavectors;
}

template <typename T>
int presorter<T>::getalpha(const QChar &first_char)
{
    int firstchar = first_char.toLatin1();
    if (firstchar <= 'A')
        return 0;
    if (firstchar <= 'Z')
        return (firstchar-'A');
    throw OUT_OF_BOUND;
}

template <typename T>
presorter<T> &presorter<T>::operator <<(PROCESSTYPE thetype)
{
    theprocess = thetype;
    // set HEAP or AVL
    return *this;
}

template <typename T>
presorter<T> &presorter<T>::operator <<(const vector<node<T> *> &inputv)
{
    int tempc;
    vector <node<T> *> temp;
    for (uint i = 0; i < 26; i++)
        alphavectors.push_back(temp);
    // We could just directly push the words to the correct heap, but, the problem there is that we need threading to make that
    // a valuable process...
    // This is the Parser.theWordlist() vector, getting it's pointers pushed to the correct vector in alphavectors

    for (uint i = 0; i < inputv.size(); i++){
        tempc = getalpha(inputv[i]->theData()[0]);
        alphavectors[tempc].push_back(inputv[i]);
    }
    return *this;
}


template <typename T>
presorter<T> &presorter<T>::operator >>(vector< vector<ullint> > &outputv)
{
    QElapsedTimer QET;
    vector <ullint> tempu;
    Heap< node<T>* > temph;

    for (uint i = 0; i < 26; i++)
    {
        references.push_back(tempu);
        heaps.push_back(temph);
    }
    // HEAPIFY UNSORTED LIST
    QET.start();
    // Now each alpha vector has the list, unordered, so the heaps must be created from the lists
    ullint alphasize = alphavectors.size();
    for (uint i = 0; i < alphasize; i++)
        heaps[i] << alphavectors[i]; // heapify each alphabetic bin
    time_last_sort = (1.0*QET.elapsed())/1000.0;

    // Now each alpha vector has the list, unordered, so the heaps must be created from the lists
    for (uint i = 0; i < alphasize; i++)
        heaps[i] >> references[i]; // pull out the list of references for each subletter
    outputv = references;
    return *this;
}

template <typename T>
presorter<T> & presorter<T>::operator >>(vector< bst< node<T> > > &outputv)
{
    // bst sort and return
    QElapsedTimer QET;
    bst< node<T> > tempb; // different than heapsort because we have to make copies
    tempb.thebstroot().push_back(NULL);
    for (uint i = 0; i < 26; i++)
    {
        bsts.push_back(tempb);
        outputv.push_back(tempb);
    }
    // We could just directly push the words to the correct heap, but, the problem there is that we need threading to make that
    // a valuable process...
    // This is the Parser.theWordlist() vector, getting it's pointers pushed to the correct vector in alphavectors

    QET.start();
    for (uint i = 0; i < alphavectors.size(); i++)
        bsts[i] << alphavectors[i];
    outputv = bsts;
    time_last_sort = (1.0*QET.elapsed())/1000.0;
    return *this;
}

template <typename T>
vector< bst<node<T> > > &presorter<T>::theBST()
{
    return bsts;
}
template <typename T>
void presorter<T>::clear()
{
    nukem();
}

template <typename T>
void presorter<T>::nukem()
{
    ullint alphasize = theAlphas().size(),
            heapsize = heaps.size(),
            bstsize  = bsts.size();
    for (uint i = 0; i < alphasize; i++)
        theAlphas()[i].clear();
    theAlphas().clear();
    for (uint i = 0; i < heapsize; i++)
        heaps[i].clear();  // clear each heap
    for (uint i = 0; i < bstsize; i++)
        bsts[i].clear();  // clear each bst
}

#endif // PRESORTER_H
