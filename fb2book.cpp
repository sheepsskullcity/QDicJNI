#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>
#include <mystring.h>
#include <fb2book.h>

/*FB2Book::FB2Book(QObject *parent) :
    QObject(parent)
{
}
*/

#ifdef Q_OS_ANDROID
FB2Book::FB2Book(MyString *stringReplace, bool *er, JavaVM *jm, jobject thiz)
{
    e = er;
    replace = stringReplace;
    textTagsList << "p" << "v";
    skipTagsList << "v";
    m_jm = jm;
    m_thiz= thiz;
}
#endif
#ifdef Q_OS_WIN32
FB2Book::FB2Book(MyString *stringReplace, bool *er)
{
    e = er;
    replace = stringReplace;
    textTagsList << "p" << "v";
    skipTagsList << "v";
}
#endif

void FB2Book::fb2Replace(const QString inputFile, const QString outputFile, bool *b_work)
{

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

    int f = sizeOfBook(inputFile);
    int i = 0;
    int prg = 0;

    QFile ifile(inputFile);
    QFile ofile(outputFile);

    if (!ifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "can't open book file";
        *e = true;
    }
    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "can't create output file";
        *e = true;
    }

    QXmlStreamReader reader(&ifile);
    reader.setNamespaceProcessing(false);
    QXmlStreamWriter writer(&ofile);
    writer.setAutoFormatting(false);

    while (!reader.atEnd() && !reader.hasError() && *b_work) {
        temp.clear();
        reader.readNext();
        writer.writeCurrentToken(reader);
        if (reader.isStartDocument()) {
            writer.writeCharacters("\n");
        }
        if (reader.isStartElement()) {
            if (checkStartName(reader)) {
                i++;
                QString txtTag = reader.name().toString();
                while (!reader.atEnd()) {
                    reader.readNext();
                    if (reader.isStartElement()) {
                        if (checkSkipName(reader)) {
                            QString skipTag = reader.name().toString();
                            while (!reader.atEnd()) {
                                if (reader.isEndElement()) {
                                    if (checkEndName(reader, skipTag)) {
                                        break;
                                    }
                                }
                                reader.readNext();
                            }
                        }
                    }
                    if (reader.isCharacters()) {
                        temp.append(reader.text());
                    }
                    if (reader.isEndElement()) {
                        if (checkEndName(reader, txtTag)) {
                            break;
                        }
                    }
                }
                replace->setString(&temp);
                replace->checkReplacedString();
                writer.writeCharacters(*(replace->getReplacedString()));
                writer.writeEndElement();

                float y = checkProgress(f, i);
                if ((int)(y * 100) > prg && ((int)(y * 100)) % 10 == 0) {
                    prg = (int)(y * 100);
                    #ifdef Q_OS_ANDROID
                    env->CallVoidMethod(m_thiz, s_MethodID, y);
                    if (env->ExceptionCheck()) {
                        env->ExceptionDescribe();
                    }
                    #endif
                }
            }
        }
    }
    if (reader.hasError()) {
        qWarning() << "XML Error in book file!";
        *e = true;
    }
    writer.writeEndDocument();
    ifile.close();
    ofile.close();
    if (!*b_work) {ofile.remove();}

    #ifdef Q_OS_ANDROID
    if (attached) {
        m_jm->DetachCurrentThread();
    }
    #endif
}

int FB2Book::sizeOfBook(QString fileName)
{
    int i = 0;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "can't open book file";
        *e = true;
    }
    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement){
            if (xml.name() == "p" || xml.name() == "v") {
                i++;
            }
        }
    }
    file.close();
    return i;
}

float FB2Book::checkProgress(int full, int i)
{
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

bool FB2Book::checkStartName(QXmlStreamReader &xsr) {
    return textTagsList.contains(xsr.name().toString());
}

bool FB2Book::checkSkipName(QXmlStreamReader &xsr) {
    return skipTagsList.contains(xsr.name().toString());
}

bool FB2Book::checkEndName(QXmlStreamReader &xsr, QString &str) {
    return xsr.name().toString() == str;
}
