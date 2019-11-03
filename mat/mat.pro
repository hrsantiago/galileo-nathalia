include(../shared.pri)

HEADERS = \
    inc/quadrule.h \
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
    inc/linear/quat.h \
    inc/solvers/runge_kutta.h \
    inc/solvers/newmark.h \
    inc/solvers/newton_raphson.h \
    inc/sparse/solver.h \
    inc/sparse/umfpack.h \


SOURCES = \
    src/misc/stress.cpp \
    src/misc/fourier.cpp \
    src/misc/util.cpp \
    src/misc/rotation.cpp \
    src/linear/dense.cpp \
    src/linear/linear.cpp \
    src/linear/sparse.cpp \
    src/linear/lin3.cpp \
    src/linear/mat3.cpp \
    src/linear/matrix.cpp \
    src/linear/quat.cpp \
    src/linear/vec3.cpp \
    src/linear/vector.cpp \
    src/solvers/runge_kutta.cpp \
    src/solvers/newmark.cpp \
    src/solvers/newton_raphson.cpp \
    src/sparse/solver.cpp \
    src/sparse/umfpack.cpp \

TARGET = mat
TEMPLATE = lib
CONFIG += staticlib
