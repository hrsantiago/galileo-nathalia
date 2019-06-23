#-------------------------------------------------
#
# Project created by QtCreator 2019-06-20T20:00:00
#
#-------------------------------------------------

include(shared.pri)

SUBDIRS = \
    ben \
    fea \
    gui \
    mat \
    #tests \

TEMPLATE = subdirs

fea.depends = mat
gui.depends = mat fea
ben.depends = mat fea
