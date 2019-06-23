QT    += core gui widgets printsupport opengl
CONFIG += c++14 object_parallel_to_source

DESTDIR = ../libs
LIBS += -L../libs \
    -L/usr/local/lib/ \

INCLUDEPATH += \
    $$PWD/mat/inc/ \
    $$PWD/fea/inc/ \
    $$PWD/gui/inc/ \
    $$PWD/ben/inc/ \
    /usr/include/suitesparse/ \
