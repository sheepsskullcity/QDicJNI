#ifndef FB2BOOK_H
#define FB2BOOK_H

#include <QObject>
#include <QXmlStreamReader>
#include <mystring.h>
#ifdef Q_OS_ANDROID
#include <jni.h>
#endif

class FB2Book : public QObject
{
    Q_OBJECT
public:
//    explicit FB2Book(QObject *parent = 0);
#ifdef Q_OS_ANDROID
    FB2Book(MyString *, bool *, JavaVM *, jobject);
#endif
#ifdef Q_OS_WIN32
    FB2Book(MyString *, bool *);
#endif
    void fb2Replace(const QString, const QString, bool *);


private:
    QList<QString> textTagsList;
    QList<QString> skipTagsList;
    QString temp;
    bool *e;
    MyString *replace;
    int sizeOfBook(QString);
    float checkProgress(int, int);

    #ifdef Q_OS_ANDROID
    JavaVM *m_jm;
    jobject m_thiz;
    #endif

    bool checkStartName(QXmlStreamReader &);
    bool checkSkipName(QXmlStreamReader &);
    bool checkEndName(QXmlStreamReader &, QString &);

};

#endif // FB2BOOK_H

