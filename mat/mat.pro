include(../shared.pri)

HEADERS = \
    inc/misc/stress.h \
    inc/misc/fourier.h \
    inc/misc/rotation.h \
    inc/misc/util.h \
    inc/misc/defs.h \
    inc/linear/sparse.h \
    inc/linear/linear.h \
    inc/linear/vector.h \
    inc/linear/dense.h \
    inc/linear/mat3.h \
    inc/linear/vec3.h \
    inc/linear/matrix.h \
    inc/linear/lin3.h \
    inc/solvers/runge_kutta.h \
    inc/solvers/newmark.h \
    inc/solvers/newton_raphson.h \


SOURCES = \
    src/misc/stress.cpp \
    src/misc/fourier.cpp \
    src/misc/util.cpp \
    src/misc/rotation.cpp \
    src/linear/dense.cpp \
    src/linear/linear.cpp \
    src/linear/sparse.cpp \
    src/solvers/runge_kutta.cpp \
    src/solvers/newmark.cpp \
    src/solvers/newton_raphson.cpp \

TARGET = mat
TEMPLATE = lib
CONFIG += staticlib
