#ifndef MYDIC_H
#define MYDIC_H

#include <QObject>
#include <QHash>

class MyDic : public QObject
{
    Q_OBJECT
public:
//    explicit MyDic(QObject *parent = 0);
    MyDic(QString, bool *, QString);
    ~MyDic();

    const QList<QList<bool> *> *getBoolDicWords() const;
    int getLongest() const;
    int getNumOfRules(const QString &) const;
    const QList<int> *getListOfRules(const QString &) const;
    const QList<int> *checkFullRule(const QString &) const;
    void getLeftRule(const QString &, QString &) const;
    void getRightRule(const QString &, QString &) const;
    const QString *getRule(int) const;
    const QList<QString> *getDicStrings() const;
    int getDicSize() const;

    static bool compare(const QString &,const QString &);
    static QRegularExpression rxp1;
    static QRegularExpression rxp2;
    static QRegularExpressionMatch match1;
    static QRegularExpressionMatch match2;

private:
    int dSize;
    QString dicFileName;
    QString encoding;
    QList<QString> dic;
    QList<QList<bool> *> matrix;
    QHash<QString, QList<int> *> hTableFull;
    QHash<QString, QList<int> *> hTableStrAr;
    int x;
    bool *e;

    int fileStrs(QString);
    void readDicFile(QString);
    void sortArray();
    void dicHashFill(QList<QString> &);
    void maxDicWordsTable(QList<QString> &);
    void hashTableGen(QList<QString> &);
    void setLeftRule(const QString &, QString &) const;
    void setRightRule(const QString &, QString &) const;

};

#endif // MYDIC_H
