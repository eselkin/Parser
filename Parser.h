#ifndef PARSER_H
#define PARSER_H
#include <QElapsedTimer>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QTextStream>
#include "node.h"
using namespace std;

typedef unsigned long long int ullint;

template <typename T>
class Parser
{

public:
    Parser();
    ~Parser();
    ullint& theWords();
    vector<node <T> *>& theWordlist();
    void clear();
    void fleschkinkaid();
    int find_syll(const QString &mystring);
    double FGL,FRL;

    template <typename U>
    friend
    QTextStream& operator>>(QTextStream& in, Parser<U> &P);

    template <typename U>
    friend
    QTextStream& operator<<(QTextStream& out, Parser<U> &P);

    ullint num_words;
    ullint len_words;
    double avg_word_len;
    double avg_sentence_len;
    double avg_syllables;
    ullint num_sentences;
    ullint num_syllables;
private:
    void nukem();
    QRegExp Expressions[15]; // sets of keywords and delimiters for regex  // "CHAPTER" "
    vector < node<T> *> wordlist;
};


template<typename T>
Parser<T>::Parser()
{
    num_words=0;
    len_words=0;
    avg_word_len=0;
    avg_sentence_len=0;
    avg_syllables = 0;
    num_sentences=0;
    num_syllables=0;
    Expressions[2].setPattern("\\b[A-Z0-9\\-\\(\\)\\'\\s]+\\b"); // any word even contractions and hyphenated words...
    Expressions[3].setPattern("[\\;\\.\\?\\!]+"); // sentence termination including "..." ellipsis
    Expressions[4].setPattern("(?![A-Za-z0-9\\'\\;\\.\\?\\!\\-\\s]+)"); // match something that's not A-Za-z.;'?!- or space
    Expressions[5].setPattern("[\\;\\.\\!\\?\\'\\s]+"); // not that you would have multiple ! or ; or ? or ', but get them out if they exist
    Expressions[6].setPattern("(([\\s]+)|(\\-{2,2}))");
    Expressions[8].setPattern("\\'");
    Expressions[9].setPattern("(n\\'t|\\'ve|\\'d|\\'ll|it\\'s|he\\'s|how\\'s|I\\'m|it\\'s|let\\'s|ma\\'am|o\\'clock|that\\'s|\\'re|when\\'s|what\\'s|here\\'s|who\\'s|why\\'s|'\\'all|d\\'|l\\'|\\'a)");
    Expressions[10].setPattern("\\'s*$"); // possessive or that's ..etc.
    Expressions[11].setPattern("^\\'[A-Za-z0-9\\-]+"); // start with an apostrophe
    Expressions[12].setPattern("(etc|i\\.e|e\\.g|r\\.s\\.\\v\\.p|e\\.t\\.\\a|p\\.s|a\\.d|b\\.c|b\\.c\\.e)\\.$"); //
    Expressions[13].setPattern("^\\(+[0-9]*\\)+"); // don't insert this word
    Expressions[14].setPattern("^\\-[A-Za-z0-9\\'\\;\\.\\!\\?]+"); // find a word or anything that starts with a -
    Expressions[0].setCaseSensitivity(Qt::CaseSensitive);
    Expressions[1].setCaseSensitivity(Qt::CaseSensitive);
    Expressions[2].setCaseSensitivity(Qt::CaseInsensitive);
    Expressions[4].setCaseSensitivity(Qt::CaseInsensitive);
    Expressions[9].setCaseSensitivity(Qt::CaseInsensitive);
    Expressions[12].setCaseSensitivity(Qt::CaseInsensitive);
}

template<typename T>
Parser<T>::~Parser()
{
    nukem();
}

template<typename T>
ullint &Parser<T>::theWords()
{
    return num_words;
}

template<typename T>
vector<node<T> *> &Parser<T>::theWordlist()
{
    return wordlist;
}

template <typename T>
void Parser<T>::clear()
{
    nukem();
}

template<typename T>
void Parser<T>::fleschkinkaid()
{
    avg_word_len = (len_words*1.) / (num_words *1.);
    avg_sentence_len = (num_words*1.) / (num_sentences*1.);
    avg_syllables = (num_syllables*1.) / (num_words*1.);
    FGL = (0.39 * avg_sentence_len) + (11.8 * avg_syllables) - 15.59;
    FRL = 206.835 - (1.015*avg_sentence_len) - (84.6*avg_syllables);
}



// FRIENDS
template<typename U>
QTextStream &operator>>(QTextStream &in, Parser<U> &P)
{
    int pos, syllablect = 0;
    QString this_word, line;
    node<U>* new_word;
    bool paragraph_reset = false, got_line = false;
    int paragraph = 1, line_of_book = 1;
    while ( !in.atEnd() )
    {
        line = in.readLine();
        //        // Fails at EOF ... next 2 lines cuts out anything but new line chars (auto by getline) and A-Za-z Space or period

        line = line.normalized(QString::NormalizationForm_D); // normalize QString... take out accent marks/diacriticals

        pos = 0;
        while ((pos = line.indexOf(P.Expressions[4], pos)) < line.size())
            line.remove(pos, 1); // will always get \n at the last char

        //        // THIS MAY RUN FOREVER, TEST!

        if (line.isEmpty())
        {
            (paragraph_reset) && (paragraph_reset = false);
            continue; // to next line
        }

        got_line = true; // reset got new line
        QStringList ListOfWords;
        ListOfWords = line.split(P.Expressions[6], QString::SkipEmptyParts); // split on spaces, AND remove spaces
        int listsize = ListOfWords.size();

        (listsize > 0 && !paragraph_reset) && (paragraph++) && (paragraph_reset = true); // get a lot of lines with nothing, the paragraph reset stays false... when it hits something the paragraph reset turns to
        (listsize > 0) && line_of_book++; // only including lines of text

        for (int i = 0; i < listsize; i++) // however many words there are
        {
            this_word = ListOfWords[i];
            if ((!this_word.contains(P.Expressions[12])) && ((pos = this_word.indexOf(P.Expressions[3])) != -1))
            {
                // got a terminating sequence for a sentence // not an abbrev and  the next word is capitalized!
                P.num_sentences++;
                // trim it off the last word
                this_word.remove(pos, this_word.size()-pos);
            }
            // trimmed regular expresion's match off the word
            if (this_word[0] == '-' || this_word[0] == '\'') this_word.remove(0,1);// word starts with - or '
            if ((!this_word.contains(P.Expressions[9])) && (this_word.contains(P.Expressions[10])))
                this_word.chop(2); // remove 's at end of word
            if (this_word[0] == '\0' || this_word == "")
                break;
            if (this_word[this_word.size()-1] == '-')
                this_word.chop(1);
            if (this_word.contains(P.Expressions[13]))
                break;
            this_word[0] = this_word[0].toUpper();
            syllablect = P.find_syll(this_word);
            new_word = new node<U>(this_word, 1, syllablect, line_of_book, paragraph, 0);
            P.num_syllables+=syllablect;
            P.num_words++;
            P.len_words+=this_word.size();
            P.wordlist.push_back(new_word);
        }
    }
    return in;
}

template <typename U>
QTextStream& operator<<(QTextStream& out, Parser<U> &P)
{
    for (uint i = 0; i < P.wordlist.size(); i++)
        out << "[" << i << "]" << *P.wordlist[i] << endl;
    return out;
}

template <typename T>
int Parser<T>::find_syll(const QString &mystring)
{
    int pos = 0;
    int count = 0;
    QRegExp VOWELS("[AEIOUY]");
    QRegExp DipTrip("(ea|au|ay|yi|ya|oo|oi|oa|ou|ei|ee|ai|ire|ide|ine|ime|ure|eye|awe|oar|oya|iou)");
    QRegExp ESLES("(le|es|les)$");
    VOWELS.setCaseSensitivity(Qt::CaseInsensitive);
    DipTrip.setCaseSensitivity(Qt::CaseInsensitive);
    while ((pos = mystring.indexOf(VOWELS,pos)) != -1 && pos < mystring.size())
    {
        count++;
        pos++;
    }
    pos = 0;
    while ((pos = mystring.indexOf(DipTrip,pos)) != -1 && pos < mystring.size())
    { // don't count final vowel
        count--;
        pos++;
    }
    pos = 0;
    if (mystring.indexOf(ESLES, pos) != -1)
        count++;
    (count < 0) && (count = 1); // if our index causes it to be misinterpreted as having two dipthongs in 2 vowels
    return count;
}

template <typename T>
void Parser<T>::nukem()
{
    ullint wordsz = wordlist.size();
    for (ullint i; i < wordsz; i++)
    {
        if (wordlist[i]) delete wordlist[i];
        wordlist[i] = NULL;
    }
    wordlist.clear();

}
#endif // PARSER_H
