#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T14:40:34
#
#-------------------------------------------------

QT += core
QT -= gui

TARGET = QDic

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-source

SOURCES += \
    main.cpp \
    mydic.cpp \
    mystring.cpp \
    bookaslist.cpp \
    fb2book.cpp \
    listarray.cpp \
    myrexdic.cpp

HEADERS += \
    mydic.h \
    mystring.h \
    bookaslist.h \
    fb2book.h \
    listarray.h \
    myrexdic.h

OTHER_FILES += \
    android-source/AndroidManifest.xml \
    android-source/src/org/qtproject/qdic/AndroidClient.java \
    android-source/src/org/qtproject/qdic/QDicService.java \
    android-source/res/drawable-hdpi/ic_launcher.png \
    android-source/res/drawable-ldpi/ic_launcher.png \
    android-source/res/drawable-mdpi/ic_launcher.png \
    android-source/res/drawable-xhdpi/ic_launcher.png \
    android-source/res/drawable-xxhdpi/ic_launcher.png \
    android-source/licences/LGPL.txt \
    android-source/licences/MPL.txt

