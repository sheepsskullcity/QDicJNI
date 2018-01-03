#ifndef MYSTRING_H
#define MYSTRING_H

#include <QObject>
#include <QHash>
#include <QRegularExpression>
#include <mydic.h>
#include <myrexdic.h>

class MyString : public QObject
{
    Q_OBJECT
public:
//    explicit MyString(QObject *parent = 0);

    MyString(const MyDic *);
    MyString(const MyRexDic *);
    void setString(QString *);
    void checkReplacedString();
    QString *getReplacedString();
    int getUsedRulesAmount();
    int getRules();
    const QList<int> *getURulesList();

private:
    void stringWordsToList(QString);
    void checkWords();
    void tempString(int, int, int b);
    void changeString(int);
    void changeRXString();

    QList<QString> arList;
    const MyDic *dicObj;
    const MyRexDic *rexObj;
    QList<int> boolIndex;
    int v;
    QString *newString;
    QString temp;
    int usedRules;
    QList<int> rules;
    QList<int> rulesN;
    const QList<QList<bool> *> *boo;
    const QList<int> *b;
    const QList<int> *c;
    QList<int> ruleWordsAmount;
    QList<QList<bool> > boolsAmount;
    bool isRex;
    QRegularExpression rxLS;
    QRegularExpression rxLI;

};

#endif // MYSTRING_H
