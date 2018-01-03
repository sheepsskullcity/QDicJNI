#include <QString>
#include <QDebug>
#include <QObject>

#ifdef Q_OS_ANDROID
#include <jni.h>
#endif
#ifdef Q_OS_WIN32
#include <QCoreApplication>
#endif

#include <mydic.h>
#include <myrexdic.h>
#include <mystring.h>
#include <bookaslist.h>
#include <fb2book.h>
#include <listarray.h>

#ifdef Q_OS_ANDROID
static JavaVM *s_javaVM = 0;
static jclass s_ClassID = 0;
static jobject myobj = NULL;

static bool b_work = false;
static bool not_canceled = true;

const MyDic *dicObj = NULL;
const MyRexDic *rexObj = NULL;
MyString *stringReplace = NULL;

QString dic_enc;
QString book_enc;

int a = 0;
int b = 0;

int setEncoding(JNIEnv *env, jobject thiz, jstring denc, jstring benc)
{
    const char* str1 = env->GetStringUTFChars(denc, 0);
    const char* str2 = env->GetStringUTFChars(benc, 0);
    dic_enc = QString::fromUtf8(str1);
    book_enc = QString::fromUtf8(str2);
    env->ReleaseStringUTFChars(denc, str1);
    env->ReleaseStringUTFChars(benc, str2);
    return 0;
}

int initDic(JNIEnv *env, jobject thiz, jstring dstr)
{
    const char* str1 = env->GetStringUTFChars(dstr, 0);
    QString dic_path(str1);
    env->ReleaseStringUTFChars(dstr, str1);

    bool errors = false;
    dicObj = new const MyDic(dic_path, &errors, dic_enc);
    stringReplace = new MyString(dicObj);

    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int initRexDic(JNIEnv *env, jobject thiz, jstring dstr)
{
    const char* str1 = env->GetStringUTFChars(dstr, 0);
    QString dic_path(str1);
    env->ReleaseStringUTFChars(dstr, str1);

    bool errors = false;
    rexObj = new const MyRexDic(dic_path, &errors, dic_enc);
    stringReplace = new MyString(rexObj);
    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int startTask(JNIEnv *env, jobject thiz, jstring bstr, jstring estr, jstring lstr
              , jboolean fb2, jboolean make_list, jboolean append_list)
{
    bool errors = false;
    b_work = true;
    not_canceled = true;
    myobj = env->NewLocalRef(thiz);

    const char* str1 = env->GetStringUTFChars(bstr, 0);
    const char* str2 = env->GetStringUTFChars(estr, 0);
    const char* str3 = env->GetStringUTFChars(lstr, 0);
    QString book_path(str1);
    QString book_out_name(str2);
    QString list_name(str3);
    env->ReleaseStringUTFChars(bstr, str1);
    env->ReleaseStringUTFChars(estr, str2);
    env->ReleaseStringUTFChars(lstr, str3);

    if (!fb2) {
        BookAsList bookObj(stringReplace, &errors, book_enc, s_javaVM, myobj);
        bookObj.bookReplace(book_path, book_out_name, &not_canceled);
    } else {
        FB2Book fb2Obj(stringReplace, &errors, s_javaVM, myobj);
        fb2Obj.fb2Replace(book_path, book_out_name, &not_canceled);
    }

    a = stringReplace->getUsedRulesAmount();
    b = stringReplace->getRules();

    if (make_list) {
        const QString listFileName = list_name;

        ListArray listObj(stringReplace->getURulesList()
                          , (dicObj != NULL ? dicObj->getDicStrings() : rexObj->getDicStrings()), &errors);
        if (!append_list) {
            listObj.writeListOfRules(listFileName, &not_canceled);
        } else {
            listObj.appendListOfRules(listFileName, &not_canceled);
        }
    }

    b_work = false;
    env->DeleteLocalRef(myobj);
    myobj = NULL;
    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int clearStatus(JNIEnv *env, jobject thiz)
{
    if (b_work) {
        return -1;
    }
    if (dicObj != NULL) {
        delete dicObj;
        dicObj = NULL;
    }
    if (rexObj != NULL) {
        delete rexObj;
        rexObj = NULL;
    }
    if (stringReplace != NULL) {
        delete stringReplace;
        stringReplace = NULL;
    }
    return 0;
}

int cancel(JNIEnv *env, jobject thiz)
{
    not_canceled = false;
    return 0;
}

int getTotal(JNIEnv *env, jobject thiz)
{
    return a;
}

int getRules(JNIEnv *env, jobject thiz)
{
    return b;
}

JNINativeMethod methods[] = {
    {"setEncoding", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)setEncoding},
    {"initDic", "(Ljava/lang/String;)I", (void *)initDic},
    {"initRexDic", "(Ljava/lang/String;)I", (void *)initRexDic},
    {"startTask", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZZ)I", (void *)startTask},
    {"clearStatus", "()I", (void *)clearStatus},
    {"cancel", "()I", (void *)cancel},
    {"getRules", "()I", (void *)getRules},
    {"getTotal", "()I", (void *)getTotal}
};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        qCritical()<<"Can't get the enviroument";
        return -1;
    }
    s_javaVM = vm;

    jclass clazz=env->FindClass("org/qtproject/qdic/QDicService");
    if (!clazz)
    {
        qCritical()<<"Can't find java class from JNI";
        return -1;
    }
    // keep a global reference to it
    s_ClassID = (jclass)env->NewGlobalRef(clazz);

    if (env->RegisterNatives(s_ClassID, methods, 8) < 0)
    {
        qCritical()<<"RegisterNatives failed";
        return -1;
    }

    return JNI_VERSION_1_6;
}
#endif
#ifdef Q_OS_WIN32

static bool b_work = false;
static bool not_canceled = true;

const MyDic *dicObj = NULL;
const MyRexDic *rexObj = NULL;
MyString *stringReplace = NULL;

int a = 0;
int b = 0;

int initDic(QString dstr, QString dic_enc)
{
    bool errors = false;
    dicObj = new const MyDic(dstr, &errors, dic_enc);
    stringReplace = new MyString(dicObj);

    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int initRexDic(QString dstr, QString dic_enc)
{
    bool errors = false;
    rexObj = new const MyRexDic(dstr, &errors, dic_enc);
    stringReplace = new MyString(rexObj);
    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int clearStatus()
{
    if (b_work) {
        return -1;
    }
    if (dicObj != NULL) {
        delete dicObj;
        dicObj = NULL;
    }
    if (rexObj != NULL) {
        delete rexObj;
        rexObj = NULL;
    }
    if (stringReplace != NULL) {
        delete stringReplace;
        stringReplace = NULL;
    }
    return 0;
}

int startTask(QString book_path, QString book_out_name, QString list_name, QString book_enc
              , bool fb2, bool make_list, bool append_list)
{
    bool errors = false;
    b_work = true;
    not_canceled = true;

    if (!fb2) {
        BookAsList bookObj(stringReplace, &errors, book_enc);
        bookObj.bookReplace(book_path, book_out_name, &not_canceled);
    } else {
        FB2Book fb2Obj(stringReplace, &errors);
        fb2Obj.fb2Replace(book_path, book_out_name, &not_canceled);
    }

    a = stringReplace->getUsedRulesAmount();
    b = stringReplace->getRules();

    if (make_list) {
        const QString listFileName = list_name;

        ListArray listObj(stringReplace->getURulesList()
                          , (dicObj != NULL ? dicObj->getDicStrings() : rexObj->getDicStrings()), &errors);
        if (!append_list) {
            listObj.writeListOfRules(listFileName, &not_canceled);
        } else {
            listObj.appendListOfRules(listFileName, &not_canceled);
        }
    }

    b_work = false;
    if (errors) {
        return -1;
    }
    else {
        return 0;
    }
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}
#endif
