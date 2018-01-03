#include <QString>
#include <QHash>
#include <QtAlgorithms>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include <mydic.h>

/*MyDic::MyDic(QObject *parent) :
    QObject(parent)
{
}
*/

MyDic::MyDic(QString dicFile, bool *er, QString enc)
    : dicFileName(dicFile), encoding(enc)
{
        x = 0;
        e = er;
        dSize = fileStrs(dicFileName);
        readDicFile(dicFileName);
}

MyDic::~MyDic()
{
      QHash<QString, QList<int> *>::iterator i1;
      for (i1 = hTableFull.begin(); i1 != hTableFull.end(); ++i1)
      {
          delete i1.value();
      }
      QHash<QString, QList<int> *>::iterator i2;
      for (i2 = hTableStrAr.begin(); i2 != hTableStrAr.end(); ++i2)
      {
          delete i2.value();
      }
      for (int i = 0; i < matrix.size(); ++i) {
          delete matrix.at(i);
      }
}

int MyDic::fileStrs(QString fileName)
{
    int fSize = 0;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) // | QIODevice::Text))
    {
        *e = true;
        qWarning() << "can't open file for 'size in strings'";
        return 0;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        in.readLine();
        ++fSize;
    }
    file.close();
    return fSize;
}

void MyDic::readDicFile(QString fileName)
{
        int i = 0;
        bool sort = true;
        QString str;
        QRegularExpression sorted("^#sorted$", QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::CaseInsensitiveOption);
        QRegularExpression rx("^(.+)=.*$");
        QRegularExpression comments("^#.*$");
        QRegularExpressionMatch match;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) // | QIODevice::Text))
        {
            qWarning() << "can't open dic file";
            *e = true;
        }

        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName(encoding.toLatin1()));

        while (i < dSize) {
            str = in.readLine();
            if (sort && str.contains(sorted)) {sort = false;}
            if (!str.isEmpty()  && !str.contains(comments) && str.contains(rx)) {
                dic << str;
            }
            ++i;
        }
        file.close();

        if (sort) {sortArray();}

        QList<QString> dic1;

        for (int r = 0; r < dic.size(); ++r)
        {
            match = rx.match(dic.at(r).toLower());
            dic1 << match.captured(1);
        }

        dicHashFill(dic1);
}

QRegularExpression MyDic::rxp1 = QRegularExpression("^(.+)=");
QRegularExpression MyDic::rxp2 = QRegularExpression("^(.+)=");
QRegularExpressionMatch MyDic::match1;
QRegularExpressionMatch MyDic::match2;

bool MyDic::compare(const QString &s1, const QString &s2)
{
        match1 = rxp1.match(s1);
        match2 = rxp2.match(s2);
        return (match1.capturedLength(1) > match2.capturedLength(1));
}

void MyDic::sortArray()
{
        qSort(dic.begin(), dic.end(), MyDic::compare);
}

void MyDic::dicHashFill(QList<QString> &dic1)
{
        QRegularExpression rx1("[^-\\s\\w\\*]", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression rx2("\\s\\s+");
        QRegularExpression rx3("^\\s|\\s$");

        for (int i = 0; i < dic.size(); i++) {
            dic1[i].replace(rx1, " ");
            dic1[i].replace(rx2, " ");
            dic1[i].replace(rx3, "");
        }
        maxDicWordsTable(dic1);
        hashTableGen(dic1);
}

void MyDic::maxDicWordsTable(QList<QString> &dic2)
{
        QRegularExpression rx("[-\\w\\*]+", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression starRx("^\\*");
        QRegularExpressionMatchIterator itr;
        QRegularExpressionMatch match;
		
		int count = 0;
        bool boo = false;
        QList<bool> *ptr;

        for (int i = 0; i < dic.size(); ++i) {
            ptr = new QList<bool>();
            matrix.append(ptr);
        }

        for (int i = 0; i < dic.size(); ++i) {
            itr = rx.globalMatch(dic2.at(i));
            while (itr.hasNext()) {
                ++count;
                match = itr.next();
                if (!match.captured(0).contains(starRx) && !boo) {
                    boo = true;
                }
                if (!boo) {
                    --count;
                } else {
                    matrix.at(i)->append(match.captured(0).contains(starRx));
                }
            }
            if (x < count) {
                x = count;
            }
            count = 0;
            boo = false;
        }
}

const QList<QList<bool> *> *MyDic::getBoolDicWords() const
{
         return &matrix;
}

int MyDic::getLongest() const
{
        return x;
}

void MyDic::hashTableGen(QList<QString> &dic2)
{
        QRegularExpression rx1("^[-\\w\\*]+", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression rx2("[-\\w\\*]+", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression starRx("^\\*");
        QRegularExpressionMatch match;
        QRegularExpressionMatchIterator itr;
        QList<int> *arList;
        QList<QString> temp;
        QList<QString> list;
        QString str;
        int pos;

        for (int i = 0; i < dic.size(); ++i) {
            if (!dic2.at(i).isEmpty()) {
                if (!dic2.at(i).contains(starRx)) {
                    match = rx1.match(dic2.at(i));
                    temp << match.captured(0);

                    if (hTableStrAr.contains(temp.last())) {
                        arList = hTableStrAr.value(temp.last());
                        *arList << i;
                        hTableStrAr.insert(temp.last(), arList);
                    }
                    else {
                        if (!temp.at(i).isEmpty()) {
                            arList = new QList<int>();
                            *arList << i;
                            hTableStrAr.insert(temp.at(i), arList);
                        }
                    }
                } else {
                    itr = rx2.globalMatch(dic2.at(i));
                    match = itr.next();
                    while (itr.hasNext() && match.captured(0).contains(starRx)) {
                        match = itr.next();
                    }
                    if (match.captured(0).contains(starRx)) {
                        temp << "";
                    } else {
                        temp << match.captured(0);
                    }
                    if (hTableStrAr.contains(temp.last())) {
                        arList = hTableStrAr.value(temp.last());
                        *arList << i;
                        hTableStrAr.insert(temp.last(), arList);
                    }
                    else {
                        if (!temp.at(i).isEmpty()) {
                            arList = new QList<int>();
                            *arList << i;
                            hTableStrAr.insert(temp.at(i), arList);
                        }
                    }
                }
            }
            else {
                temp << dic2.at(i);
            }
        }

        for (int i = 0; i < dic.size(); i++) {
            list.clear();
            str.clear();
            pos = 0;
            itr = rx2.globalMatch(dic2.at(i));
            while (itr.hasNext()) {
                match = itr.next();
                list << match.captured(0);
            }
            if (!list.isEmpty()) {
                for (int j = 0; j < list.size(); ++j) {
                    pos = j;
                    if (!list.at(j).contains(starRx)) {
                        break;
                    }
                }
                while (pos < list.size()) {
                    if (!list.at(pos).contains(starRx)) {
                        pos == list.size() - 1 ? str += list.at(pos) : str += list.at(pos) + " ";
                    } else {
                        pos == list.size() - 1 ? str += "*" : str += "* ";
                    }
                    pos++;
                }
            }
            if (hTableFull.contains(str)) {
                arList = hTableFull.value(str);
                *arList << i;
                hTableFull.insert(str, arList);
            }
            else {
                if (!str.isEmpty()) {
                    arList = new QList<int>();
                    *arList << i;
                    hTableFull.insert(str, arList);
                }
            }
        }     
}

int MyDic::getNumOfRules(const QString &str) const
{
        if (hTableStrAr.contains(str)) {
            return hTableStrAr.value(str)->size();
        }
        else {
            return 0;
        }
}

const QList<int> *MyDic::getListOfRules(const QString &str) const
{
        if (hTableStrAr.contains(str)) {
            return hTableStrAr.value(str);
        }
        else return NULL;
}

const QList<int> *MyDic::checkFullRule(const QString &str) const
{
        if (hTableFull.contains(str)) {
            return hTableFull.value(str);
        }
        else return NULL;
}

void MyDic::setLeftRule(const QString &str, QString &tempRule) const
{
        QRegularExpression rx1("^(.+)=.*$");
        QRegularExpression rx2("^(\\w)", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression rx3("^(\\\\\\$)(\\w)", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression rx4("^(\\\\\\$)");
        QRegularExpression rx5("(\\w)$", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpression remove1("^\\\\\\*");
        QRegularExpression remove2("^(\\$)\\\\\\*");
        QRegularExpression repl("([^\\w])\\\\\\*(\\w+)\\b", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpressionMatch match;

            match = rx1.match(str);
            tempRule = match.captured(1);
            tempRule = QRegExp::escape(tempRule);
            tempRule.replace(rx2, "\\b\\1");
            tempRule.replace(rx3, "$\\b\\2");
            tempRule.replace(rx4, "$");
            tempRule.replace(rx5, "\\1\\b");
            tempRule.replace(remove1, "");
            tempRule.replace(remove2, "\\1");
            tempRule.replace(repl, "\\1([-\\w*]*)\\2");
}

void MyDic::setRightRule(const QString &str, QString &temp) const
{
        int count;
        QList<int> list;
        QList<bool> blist;
        QString s1 = "\\";
        QString s2 = "\\\\";
        QString nstr = "\\";
        QRegularExpression rx1("^(.+)=(.*)$");
        QRegularExpression rx2("^\\*|^\\$\\*");
        QRegularExpression rx3("[\\*\\w]+", QRegularExpression::UseUnicodePropertiesOption);
        QRegularExpressionMatch match;
        QRegularExpressionMatchIterator itr;

            match = rx1.match(str);
            temp = match.captured(1);
            temp.replace(rx2, "");
            count = 0;

            itr = rx3.globalMatch(temp);
            while (itr.hasNext()) {
                match = itr.next();
                list << match.capturedStart(0);
                blist << false;
            }

            for (int j = 0; j < list.size(); ++j) {
                if (temp.at(list.at(j)) == '*') {
                    blist.replace(j, true);
                    count++;
                }
            }

            match = rx1.match(str);
            temp = match.captured(2);
            if (temp.contains(s1)) {
                temp.replace(s1, s2);
            }

            list.clear();

            itr = rx3.globalMatch(temp);
            while (itr.hasNext()) {
                match = itr.next();
                list << match.capturedStart(0);
            }

            if (list.size() == blist.size()) {
                for (int k = list.size(); k > 0; --k) {
                    if (blist.at(k - 1)) {
                        temp.insert(list.at(k - 1), nstr + QString::number(count));
                        count--;
                    }
                }
            }
            list.clear();
            blist.clear();
}

void MyDic::getLeftRule(const QString &str1, QString &str2) const
{
        setLeftRule(str1, str2);
}

void MyDic::getRightRule(const QString &str1, QString &str2) const
{
        setRightRule(str1, str2);
}

const QString *MyDic::getRule(int i) const
{
        return &dic.at(i);
}

const QList<QString> *MyDic::getDicStrings() const
{
       return &dic;
}

int MyDic::getDicSize() const
{
    return dic.size();
}



