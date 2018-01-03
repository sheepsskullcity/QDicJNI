#ifndef LISTARRAY_H
#define LISTARRAY_H

#include <QObject>

class ListArray : public QObject
{
    Q_OBJECT
public:
//    explicit ListArray(QObject *parent = 0);
    ListArray(const QList<int> *, const QList<QString> *, bool *);
    void writeListOfRules(QString, bool *);
    void appendListOfRules(QString, bool *);

private:
    bool *e;
    const QList<int> *listOfRules;
    const QList<QString> *stringList;

};

#endif // LISTARRAY_H
