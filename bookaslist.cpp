#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <bookaslist.h>
#include <mystring.h>

/*BookAsList::BookAsList(QObject *parent) :
    QObject(parent)
{
}
*/

#ifdef Q_OS_ANDROID
BookAsList::BookAsList(MyString *stringReplace, bool *er, QString enc, JavaVM *jm, jobject thiz)
    : encoding(enc)
{
        e = er;
        replace = stringReplace;
        m_jm = jm;
        m_thiz= thiz;
}
#endif
#ifdef Q_OS_WIN32
BookAsList::BookAsList(MyString *stringReplace, bool *er, QString enc)
    : encoding(enc)
{
        e = er;
        replace = stringReplace;
}
#endif

int BookAsList::fileStrs(QString fileName)
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

void BookAsList::bookReplace(QString iFileName, QString oFileName, bool *b_work) {

    #ifdef Q_OS_ANDROID
    JNIEnv* env;

    bool attached = false;

    int getStat = m_jm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (getStat != JNI_OK) {
        qCritical() << "Can't get the enviroument";
        if (getStat == JNI_EDETACHED) {
            if (m_jm->AttachCurrentThread(&env, NULL) != 0) {
                qCritical() << "Failed to attach";
                return;
            } else {
                attached = true;
            }
        } else if (getStat == JNI_EVERSION) {
            qCritical() << "Env: version not supported";
            return;;
        }
    }

    jclass clazz=env->GetObjectClass(m_thiz);
    if (!clazz)
    {
        qCritical()<<"Can't find java class from JNI";
        return;
    }

    jmethodID s_MethodID = env->GetMethodID(clazz, "updateProgress", "(F)V");
    if (!s_MethodID)
    {
        qCritical()<<"Can't find java method";
        return;
    }
    #endif

        int i = 0;
        int prg = 0;

        QString temp;

        bSize = fileStrs(iFileName);

        QFile infile(iFileName);
        QFile outfile(oFileName);
        if (!infile.open(QIODevice::ReadOnly)) // | QIODevice::Text))
        {
            qWarning() << "can't open book file";
            *e = true;
        }

        if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qWarning() << "can't create output file";
            *e = true;
        }

        QTextStream in(&infile);
        QTextStream out(&outfile);
        in.setCodec(QTextCodec::codecForName(encoding.toLatin1()));
        out.setCodec(QTextCodec::codecForName("UTF-8"));

        while (i < bSize && *b_work) {
            temp = in.readLine();
            replace->setString(&temp);
            replace->checkReplacedString();
            out << *(replace->getReplacedString()) << "\n";
            float y = checkProgress(bSize, i);
            if ((int)(y * 100) > prg && ((int)(y * 100)) % 10 == 0) {
                prg = (int)(y * 100);
                #ifdef Q_OS_ANDROID
                env->CallVoidMethod(m_thiz, s_MethodID, y);
                if (env->ExceptionCheck()) {
                    env->ExceptionDescribe();
                }
                #endif
            }
            ++i;
        }
        outfile.close();
        infile.close();
        if (!*b_work) {outfile.remove();}

        #ifdef Q_OS_ANDROID
        if (attached) {
            m_jm->DetachCurrentThread();
        }
        #endif
}

float BookAsList::checkProgress(int full, int i) {
        if (full > 100){
            if (( i % (full / 100)) == 0) {
                float y = ((float) i / (float) full);
                return y;
            }
        } else {
            float y = ((float) i / (float) full);
            return y;
        }
        return 0;
}

