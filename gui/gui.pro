include(../shared.pri)

HEADERS = \
    inc/Actions/Help/About.h \
    inc/Actions/Help/Index.h \
    inc/Actions/Mesh/Cells/Cells.h \
    inc/Actions/Mesh/Cells/Line.h \
    inc/Actions/Mesh/Cells/Plane.h \
    inc/Actions/Mesh/Cells/Volume.h \
    inc/Actions/Mesh/Elements/ElementsMesh.h \
    inc/Actions/Mesh/Materials/Heat/Heat.h \
    inc/Actions/Mesh/Materials/Materials.h \
    inc/Actions/Mesh/Materials/Mechanic/Concrete.h \
    inc/Actions/Mesh/Materials/Mechanic/Steel.h \
    inc/Actions/Mesh/Nodes/Nodes.h \
    inc/Actions/Mesh/Sections/Box.h \
    inc/Actions/Mesh/Sections/Circle.h \
    inc/Actions/Mesh/Sections/I.h \
    inc/Actions/Mesh/Sections/Rebars.h \
    inc/Actions/Mesh/Sections/Rectangle.h \
    inc/Actions/Mesh/Sections/Ring.h \
    inc/Actions/Mesh/Sections/Sections.h \
    inc/Actions/Mesh/Sections/T.h \
    inc/Actions/Model/Model.h \
    inc/Actions/Results/Deformed.h \
    inc/Actions/Results/Elements.h \
    inc/Actions/Results/Equilibrium_Path.h \
    inc/Actions/Results/Limits.h \
    inc/Actions/Results/Nodal.h \
    inc/Actions/Results/Paths.h \
    inc/Actions/Results/Plots.h \
    inc/Canvas/Bound.h \
    inc/Canvas/Canvas.h \
    inc/Canvas/Click.h \
    inc/Canvas/Section.h \
    inc/Galileo/Galileo.h \
    inc/Limit/Dof.h \
    inc/Limit/State.h \
    inc/Results/Canvas.h \
    inc/Results/Path.h \
    inc/Results/Results.h \
    inc/Util/Recent.h \
    inc/qcustomplot.h \


SOURCES = \
    src/Actions/Help/About.cpp \
    src/Actions/Help/Index.cpp \
    src/Actions/Mesh/Cells/Cells.cpp \
    src/Actions/Mesh/Cells/Line.cpp \
    src/Actions/Mesh/Cells/Plane.cpp \
    src/Actions/Mesh/Cells/Volume.cpp \
    src/Actions/Mesh/Elements/ElementsMesh.cpp \
    src/Actions/Mesh/Materials/Heat/Heat.cpp \
    src/Actions/Mesh/Materials/Materials.cpp \
    src/Actions/Mesh/Materials/Mechanic/Concrete.cpp \
    src/Actions/Mesh/Materials/Mechanic/Steel.cpp \
    src/Actions/Mesh/Nodes/Nodes.cpp \
    src/Actions/Mesh/Sections/Box.cpp \
    src/Actions/Mesh/Sections/Circle.cpp \
    src/Actions/Mesh/Sections/I.cpp \
    src/Actions/Mesh/Sections/Rebars.cpp \
    src/Actions/Mesh/Sections/Rectangle.cpp \
    src/Actions/Mesh/Sections/Ring.cpp \
    src/Actions/Mesh/Sections/Sections.cpp \
    src/Actions/Mesh/Sections/T.cpp \
    src/Actions/Model/Model.cpp \
    src/Actions/Results/Deformed.cpp \
    src/Actions/Results/Elements.cpp \
    src/Actions/Results/Equilibrium_Path.cpp \
    src/Actions/Results/Limits.cpp \
    src/Actions/Results/Nodal.cpp \
    src/Actions/Results/Paths.cpp \
    src/Actions/Results/Plots.cpp \
    src/Actions/Results/Results.cpp \
    src/Canvas/Bound.cpp \
    src/Canvas/Canvas.cpp \
    src/Canvas/Click.cpp \
    src/Canvas/Section.cpp \
    src/Galileo/Galileo.cpp \
    src/Limit/Dof.cpp \
    src/Limit/State.cpp \
    src/Results/Canvas.cpp \
    src/Results/Path.cpp \
    src/Results/Results.cpp \
    src/Util/Recent.cpp \
    src/main.cpp \
    src/qcustomplot.cpp \


TARGET = gui
TEMPLATE = app

LIBS += \
    -lfea \
    -lmat \
    -lGL \
    -lGLU \
    -lboost_system \
    -lboost_filesystem \
    -lumfpack \
    -llapack

FORMS += \
    ui/Galileo/Galileo.ui \
    ui/Help/About.ui \
    ui/Help/Index.ui \
    ui/Limits/Dof.ui \
    ui/Limits/Path.ui \
    ui/Limits/State.ui \
    ui/Mesh/Cells/Cells.ui \
    ui/Mesh/Cells/Line.ui \
    ui/Mesh/Cells/Plane.ui \
    ui/Mesh/Cells/Volume.ui \
    ui/Mesh/Elements/ElementsMesh.ui \
    ui/Mesh/Materials/Heat/Heat.ui \
    ui/Mesh/Materials/Materials.ui \
    ui/Mesh/Materials/Mechanic/Concrete.ui \
    ui/Mesh/Materials/Mechanic/Steel.ui \
    ui/Mesh/Nodes/Nodes.ui \
    ui/Mesh/Sections/Box.ui \
    ui/Mesh/Sections/Circle.ui \
    ui/Mesh/Sections/I.ui \
    ui/Mesh/Sections/Properties.ui \
    ui/Mesh/Sections/Rebars.ui \
    ui/Mesh/Sections/Rectangle.ui \
    ui/Mesh/Sections/Ring.ui \
    ui/Mesh/Sections/Sections.ui \
    ui/Mesh/Sections/T.ui \
    ui/Model/Model.ui \
    ui/Results/Deformed.ui \
    ui/Results/Elements.ui \
    ui/Results/Equilibrium_Path.ui \
    ui/Results/Nodal.ui \
    ui/Results/Paths.ui \
    ui/Results/Plots.ui \
