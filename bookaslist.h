#ifndef BOOKASLIST_H
#define BOOKASLIST_H

#include <QObject>
#include <mystring.h>
#ifdef Q_OS_ANDROID
#include <jni.h>
#endif

class BookAsList : public QObject
{
    Q_OBJECT
public:
//    explicit BookAsList(QObject *parent = 0);
#ifdef Q_OS_ANDROID
    BookAsList(MyString *, bool *, QString, JavaVM *, jobject);
#endif
#ifdef Q_OS_WIN32
    BookAsList(MyString *, bool *, QString);
#endif
    void bookReplace(QString, QString, bool *);

private:
    int bSize;
    QString encoding;
    bool *e;
    MyString *replace;

    #ifdef Q_OS_ANDROID
    JavaVM *m_jm;
    jobject m_thiz;
    #endif

    int fileStrs(QString);
    float checkProgress(int, int);

};

#endif // BOOKASLIST_H
