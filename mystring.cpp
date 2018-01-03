#include <QString>
#include <QList>
#include <QHash>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QObject>
#include <QDebug>
#include <mydic.h>
#include <myrexdic.h>
#include <mystring.h>

/*MyString::MyString(QObject *parent)
    : QObject(parent)
{
}
*/

MyString::MyString(const MyDic *obj)
{
        dicObj = obj;
        v = dicObj->getDicSize();
        isRex = false;
        usedRules = 0;
        boo = dicObj->getBoolDicWords();
        for (int i = 0; i < v; ++i) {rulesN << 0;}
}

MyString::MyString(const MyRexDic *obj)
{
        rexObj = obj;
        isRex = true;
        usedRules = 0;
        for (int i = 0; i < rexObj->getDicSize(); ++i) {rulesN << 0;}
}

void MyString::setString(QString *str)
{
        newString = str;
        if (!isRex) {
            boolIndex.clear();
            arList.clear();
        }
}

void MyString::checkReplacedString()
{
        if (!isRex) {
            stringWordsToList(newString->toLower());
            checkWords();
        } else {
            changeRXString();
        }
}

void MyString::stringWordsToList(QString str)
{
        QRegularExpression rx("[-\\w\\*]+", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpressionMatch match;
        QRegularExpressionMatchIterator itr;

        itr = rx.globalMatch(str);
        while (itr.hasNext()) {
            match = itr.next();
            arList << match.captured(0);
        }
}

void MyString::checkWords()
{
        int a = 0;

        if (arList.size() > 0) {
            if (arList.size() == 1) {
                if (dicObj->getNumOfRules(arList.at(0)) != 0) {
                    c = dicObj->checkFullRule(arList.at(0));
                    if (c != NULL && !c->isEmpty()) {
                        for (int q = 0; q < c->size(); ++q) {
                            if (!boolIndex.contains(c->at(q))) {
                                boolIndex << c->at(q);
                            }
                        }
                    }
                }
            }
            else {
                for (int i = 0; i < arList.size(); ++i) {
                    a = dicObj->getNumOfRules(arList.at(i));
                    ruleWordsAmount.clear();
                    boolsAmount.clear();
                    if (a != 0) {
                        b = dicObj->getListOfRules(arList.at(i));
                        for (int j = 0; j < a; ++j) {
                            if (!ruleWordsAmount.contains( boo->at(b->at(j))->size() )) {
                                ruleWordsAmount << boo->at(b->at(j))->size();
                            }
                            if (!boolsAmount.contains( *(boo->at(b->at(j))) )) {
                                boolsAmount << *(boo->at(b->at(j)));
                            }

                        }
                        for (int k = 2; k <= dicObj->getLongest(); ++k) {
                            if (ruleWordsAmount.contains(k) && (k <= (arList.size() - i))) {
                                for (int w = 0; w < boolsAmount.size(); ++w) {
                                    if (boolsAmount.at(w).size() == k) {
                                        tempString(i, k, w);
                                        c = dicObj->checkFullRule(temp);
                                        if (c != NULL && !c->isEmpty()) {
                                            for (int e = 0; e < c->size(); ++e) {
                                                if (!boolIndex.contains(c->at(e))) {
                                                    boolIndex << c->at(e);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        c = dicObj->checkFullRule(arList.at(i));
                        if (c != NULL && !c->isEmpty()) {
                            for (int t = 0; t < c->size(); ++t) {
                                if (!boolIndex.contains(c->at(t))) {
                                    boolIndex << c->at(t);
                                }
                            }
                        }
                    }
                }
            }
        }
}

void MyString::tempString(int p, int n, int b)
{
        int count = 0;
        temp = "";
        const QList<bool> *list = &(boolsAmount.at(b));
        while (n != 0) {
            if (n != 1) {
                if (list->at(count) == false) {
                    temp += arList.at(p) + " ";
                } else {
                    temp += "* ";
                }
            }
            else {
                if (list->at(count) == false) {
                    temp += arList.at(p);
                } else {
                    temp += "*";
                }
            }
            count++;
            p++;
            n--;
        }
}

QString *MyString::getReplacedString()
{
        if (!isRex) {
            for (int i = 0; i < v; ++i) {
                if (boolIndex.contains(i)) {
                    changeString(i);
                }
            }
        }
        return newString;
}

void MyString::changeString(int i)
{
        QRegularExpression rx("^\\$(.+)$");
        QRegularExpressionMatch match;
        QRegularExpressionMatchIterator itr;
        QString right;
        QString left;
        dicObj->getLeftRule(*(dicObj->getRule(i)), left);

        match = rx.match(left);
        if (match.hasMatch()) {
            rxLS.setPattern(match.captured(1));
            rxLS.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
            if (rxLS.match(*newString).hasMatch()) {
                itr = rxLS.globalMatch(*newString);
                while (itr.hasNext()) {usedRules++; rulesN[i]++; itr.next();}
                if (!rules.contains(i)) {rules << i;}
            }
            dicObj->getRightRule(*(dicObj->getRule(i)), right);
            newString->replace(rxLS, right);
        }
        else {
            rxLI.setPattern(left);
            rxLI.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::CaseInsensitiveOption);
            if (rxLI.match(*newString).hasMatch()) {
                itr = rxLI.globalMatch(*newString);
                while (itr.hasNext()) {usedRules++; rulesN[i]++; itr.next();}
                if (!rules.contains(i)) {rules << i;}
            }
            dicObj->getRightRule(*(dicObj->getRule(i)), right);
            newString->replace(rxLI, right);
        }
}

void MyString::changeRXString()
{
    const QRegularExpression *rx;
    QRegularExpressionMatchIterator itr;

    for (int i = 0; i < rexObj->getDicSize(); ++i) {
        rx = rexObj->getLeftRule(i);
        if (rx->isValid()) {
            itr = rx->globalMatch(*newString);
            if (itr.hasNext()) {
                while (itr.hasNext()) {usedRules++; rulesN[i]++; itr.next();}
                if (!rules.contains(i)) {rules << i;}
                newString->replace(*rx, *(rexObj->getRightRule(i)));
            }
        }
    }
}

int MyString::getUsedRulesAmount() {
        return usedRules;
    }

int MyString::getRules()
{
        return rules.size();
}

const QList<int> *MyString::getURulesList()
{
        return &rulesN;
}
