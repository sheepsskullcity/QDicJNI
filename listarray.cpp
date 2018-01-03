#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <listarray.h>

/*ListArray::ListArray(QObject *parent) :
    QObject(parent)
{
}
*/

ListArray::ListArray(const QList<int> *list, const QList<QString> *slist, bool *er)
{
    e = er;
    listOfRules = list;
    stringList = slist;
}

void ListArray::writeListOfRules(QString fileName, bool *b_work)
{
    QFile file(fileName);
    int i = 0;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "can't create list file";
        *e = true;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    while (i < listOfRules->size() && *b_work) {
        if(listOfRules->at(i) != 0) {
            out << listOfRules->at(i) << " - " << stringList->at(i) << "\n";
        }
        ++i;
    }
    file.close();
    if (!*b_work) {file.remove();}
}

void ListArray::appendListOfRules(QString fileName, bool *b_work)
{
    QFile file(fileName);
    int i = 0;

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
    {
        qWarning() << "can't create list file";
        *e = true;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    while (i < listOfRules->size() && *b_work) {
        if(listOfRules->at(i) != 0) {
            out << listOfRules->at(i) << " - " << stringList->at(i) << "\n";
        }
        ++i;
    }
    file.close();
    if (!*b_work) {file.remove();}
}
