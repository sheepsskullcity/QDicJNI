#ifndef MYREXDIC_H
#define MYREXDIC_H

#include <QObject>

class MyRexDic : public QObject
{
    Q_OBJECT
public:
    MyRexDic(QString, bool *, QString);
    ~MyRexDic();

    int getDicSize() const;
    const QRegularExpression *getLeftRule(int) const;
    const QString *getRightRule(int) const;
    const QList<QString> *getDicStrings() const;

private:
    int dSize;
    QString dicFileName;
    QString encoding;
    QList<QString> dic;
    QList<const QRegularExpression *> rulesArrayL;
    QList<QString> rulesArrayR;
    bool *e;

    int fileStrs(QString);
    void readDicFile(QString);
    void setLeftRules();
    void setRightRules();

};

#endif // MYREXDIC_H



