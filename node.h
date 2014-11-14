#ifndef node_H
#define node_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <QTextStream>
#include <QString>
using namespace std;

template<typename T>
struct node
{
    node(const T &d, int c = 0, int syll = 0, int book = 0, int chap = 0, int line = 0, int p = 0, int h = 0);
    ~node();
    node(const node<T> &other);
    node<T>& operator=(const node<T>& other);
    T& operator=(const T& other);
    node<T>& operator+=(const node<T>& rhs);
    T& theData();
    T theData() const;
    int &theCount();  // only for bst
    int &theSyll();
    int &theHeight(); // only for BST
    vector<int> &theLine(); // line of the text [0] for heap [i] for BST
    vector<int> &theParagraph(); // paragraph of the text [0] for heap [i] for BST
    vector< vector <int> > &theBook(); // book of the text


    friend
    ostream& operator<<(ostream& out, node<QString> &n);

    template<typename U, typename V>
    friend
    V& operator<<(V& out, node<U> &n);

    template<typename U>
    friend
    istream& operator>>(istream& in,  node<U> &n);

    template<typename U>
    friend
    bool operator<(const node<U> &n, const node<U> &m );

    template<typename U>
    friend
    bool operator>(const node<U> &n, const node<U> &m );


    template<typename U>
    friend
    bool operator==(const node<U> &n, const node<U> &m );

    template<typename U>
    friend
    bool operator!=(const node<U> &n, const node<U> &m );

    T data;
    vector< vector <int> > text_book; // contains the chapters // first book, say 4 with chapter 3 <0><0>,<0><0>,<0><0>,<0><0>,<1><0>
    vector<int> text_line;
    vector<int> paragraph;
    int syllables;
    int count;
    int height;
    void nukem();
    void copy(const node<T> &other);
};

template<typename T>
node<T>::node(const T& d, int c, int syll, int book, int chap, int textline, int p, int h)
{
    data = d;
    count = c;
    height= h;

    // We keep track of the line, the paragraph, the book's chapter and the book of each word

    vector<int> temp;
    while(text_book.size() < book)
        text_book.push_back(temp); // push in an empty vector with size 0
    temp.push_back(chap);
    text_book.push_back(temp); // the right book now has a vector size of 1 with a copy of temp which has a value of the chapter as it's element

    if (textline != 0)
        text_line.push_back(textline);
    if (p != 0)
        paragraph.push_back(p);
    if (syll > 0)
        syllables = syll;
    else
        (syllables = 1);
}


template<typename T>
node<T>::~node()
{
    nukem();
}

template<typename T>
node<T>::node(const node<T> &other)
{
    copy(other);
}

template<typename T>
node<T>& node<T>::operator=(const node<T>& other)
{
    if(this != &other)
    {
        nukem();
        copy(other);
    }
    return *this;
}

template<typename T>
node<T>& node<T>::operator+=(const node<T> &rhs)
{
    vector<int> books;
    while(text_book.size() <= rhs.text_book.size())
        text_book.push_back(books);
    text_book[(rhs.text_book.size()-1)].push_back(rhs.text_book[rhs.text_book.size()-1][0]); // push in the book at the end
    paragraph.push_back(rhs.paragraph[0]);
    text_line.push_back(rhs.text_line[0]);
    count+=rhs.count;
    // does not change balance
    return *this;
}

template<typename T>
T& node<T>::theData()
{
    // Return by ref the data so we can assign to it as LVALUE
    return data;
}

template<typename T>
T node<T>::theData() const
{
    // Return by ref the data so we can assign to it as LVALUE
    return data;
}

template<typename T>
int &node<T>::theSyll()
{
    // Return by ref the data so we can assign to it as LVALUE
    return syllables;
}

template<typename T>
int& node<T>::theCount()
{
   // Return by ref the count, so we can assign to it!
    return count;

}

template<typename T>
int& node<T>::theHeight()
{
   // Return by ref the height, so we can assign to it!
    return height;
}

template<typename T>
vector<int> &node<T>::theParagraph()
{
    return paragraph;
}

template<typename T>
vector<int> &node<T>::theLine()
{
    return text_line;
}

template<typename T>
vector< vector<int> > &node<T>::theBook()
{
    return text_book;
}

template<typename T>
void node<T>::nukem()
{
    // there is no DYNAMIC MEMORY, NO LEFT RIGHT! HANDLED NOW BY ITS INDEX
}

template<typename T>
void node<T>::copy(const node<T> &other)
{
    count = other.count;
    data = other.data;
    text_book = other.text_book;
    text_line = other.text_line;
    paragraph = other.paragraph;
}

template<typename U>
U& operator<<(U& out, node<QString> &n)
{
    out<< (n.theData().toUtf8().constData()) << " ";
    if (n.theCount() > 1)
        out << "C:"<<n.theCount() <<" ";
    if (n.theSyll() >= 1)
        out << "S: " << n.theSyll() <<  " ";
    for (uint i = 0; i < n.theBook().size(); i++)
    {
        if (n.theBook()[i].size() > 0) // if the book has chapter references
        {
            out << "B[" << i<< "]: ";
            for (uint j = 0; j < n.theBook()[i].size(); j++)
                out << "c. " << n.theBook()[i][j] << " ";
        }

    }
    for (uint i = 0; i < n.theParagraph().size(); i++)
    {
        out << "p: " << n.theParagraph()[i] << " ";
    }
    for (uint i = 0; i < n.theLine().size(); i++)
    {
        out << "l: " << n.theLine()[i] << " ";
    }

    return out;
}

template<typename U>
istream& operator>>(istream& in,  node<U> &n)
{
    return in;
}


template<typename U>
bool operator<(const node<U> &n,const node<U> &m )
{
    return (n.data < m.data) ?
                true :
                (n.data == m.data) ? (n.text_line < m.text_line) ? true :
                                                                   false :
                                                                   false;
}

template<typename U>
bool operator>(const node<U> &n,const node<U> &m )
{
    return (n.data > m.data);
}

template<typename U>
bool operator==(const node<U> &n,const node<U> &m )
{
    return n.data == m.data;
}

template<typename U>
bool operator!=(const node<U> &n,const node<U> &m )
{
    return n.data != m.data;
}


#endif // node_H
