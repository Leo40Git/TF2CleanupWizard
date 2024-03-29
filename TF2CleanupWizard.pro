QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cleanupdialog.cpp \
    cleanupwizard.cpp \
    cleanupworker.cpp \
    htmldelegate.cpp \
    intropage.cpp \
    main.cpp \
    selectuserpage.cpp \
    steampathpage.cpp \
    tf2pathpage.cpp \
    usersvdfparser.cpp \
    warningpage.cpp

HEADERS += \
    cleanupdialog.h \
    cleanupwizard.h \
    cleanupworker.h \
    htmldelegate.h \
    intropage.h \
    selectuserpage.h \
    steampathpage.h \
    tf2pathpage.h \
    usersvdfparser.h \
    warningpage.h

win32 {
    VERSION = 1.0.0.0 # major.minor.patch.build
    QMAKE_TARGET_COMPANY = "ADudeCalledLeo (Leo40Git)"
    QMAKE_TARGET_PRODUCT = "TF2 Cleanup Wizard"
    QMAKE_TARGET_DESCRIPTION = "Cleans up your Team Fortress 2 installation."
    QMAKE_TARGET_COPYRIGHT = "(C) ADudeCalledLeo (Leo40Git) 2019"
    RC_ICONS = icon.ico
} else:VERSION = 1.0.0    # major.minor.patch
DEFINES += APP_VERSION=\"\\\"$${VERSION}\\\"\" \

FORMS += \
    cleanupdialog.ui

win32: LIBS += -lpsapi

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
