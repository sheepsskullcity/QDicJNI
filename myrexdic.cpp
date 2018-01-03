#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "myrexdic.h"

/*MyRexDic::MyRexDic(QObject *parent) :
    QObject(parent)
{
}
*/

MyRexDic::MyRexDic(QString dicFile, bool *er, QString enc)
    : dicFileName(dicFile), encoding(enc)
{
        e = er;
        dSize = fileStrs(dicFileName);
        readDicFile(dicFileName);
        setLeftRules();
        setRightRules();
}

MyRexDic::~MyRexDic()
{
    for (int i = 0; i < rulesArrayL.size(); ++i) {
        delete rulesArrayL.at(i);
    }
}

int MyRexDic::fileStrs(QString fileName)
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

void MyRexDic::readDicFile(QString fileName)
{
        int i = 0;
        QString str;
        QRegularExpression rx("^.+#i?->#.*$");
        QRegularExpression comments("^#");
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) // | QIODevice::Text))
        {
            qWarning() << "can't open rexdic file";
            *e = true;
        }

        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName(encoding.toLatin1()));

        while (i < dSize) {
            str = in.readLine();
            if (!str.contains(comments) && !str.isEmpty()) {
                if (str.contains(rx)) {
                    dic << str;
                } else { qWarning() << "invalid expression in rex file"; }
            }
            ++i;
        }
        file.close();
}

int MyRexDic::getDicSize() const
{
        return dic.size();
}

void MyRexDic::setLeftRules()
{
        QRegularExpression rx("^(.+)(#i?->#).*$");
        QRegularExpressionMatch match;
        const QRegularExpression *rex;

        for (int i = 0; i < dic.size(); ++i) {
            match = rx.match(dic.at(i));
            rex = new const QRegularExpression(match.captured(1), (match.captured(2).contains("#i->#") ? QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption : QRegularExpression::UseUnicodePropertiesOption));
            if (!rex->isValid()) { qWarning() << "invalid RegExp: " << i + 1; }
            rulesArrayL << rex;
        }
}

void MyRexDic::setRightRules()
{
        QRegularExpression rx("^.+#i?->#(.*)$");
        QRegularExpressionMatch match;
        for (int i = 0; i < dic.size(); ++i) {
            match = rx.match(dic.at(i));
            rulesArrayR << match.captured(1);
        }
}

const QRegularExpression *MyRexDic::getLeftRule(int i) const
{
        return  rulesArrayL.at(i);
}

const QString *MyRexDic::getRightRule(int i) const
{
        return  &(rulesArrayR.at(i));
}

const QList<QString> *MyRexDic::getDicStrings() const
{
       return &dic;
}

