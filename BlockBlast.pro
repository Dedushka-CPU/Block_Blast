QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 static

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    source/blockshape.cpp \
    source/fallingshapeswidget.cpp \
    source/gameboard.cpp \
    main.cpp \
    source/mainwindow.cpp \
    source/shapewidget.cpp

HEADERS += \
    headers/blockshape.h \
    headers/fallingshapeswidget.h \
    headers/gameboard.h \
    headers/mainwindow.h \
    headers/shapewidget.h



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
