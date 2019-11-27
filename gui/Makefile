#qt
qin = /usr/include/x86_64-linux-gnu/qt5
uic = /usr/lib/x86_64-linux-gnu/qt5/bin/uic
moc = /usr/lib/x86_64-linux-gnu/qt5/bin/moc
qde = /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++

#compiler
CXX = g++
WARS = -Wno-reorder -Wformat=0  -Wno-return-type
LIBS += -l pthread -l GL -l GLU -l boost_system -l boost_filesystem -l qcustomplot
LIBS += -l Qt5Core -l Qt5Gui -l Qt5Widgets -l Qt5OpenGL -l Qt5PrintSupport -l umfpack -l lapack
DEFS = -D QT_CORE_LIB -D QT_GUI_LIB -D QT_WIDGETS_LIB -D QT_SHARED -D QT_OPENGL_LIB -D QT_CONSOLE -D QT_PRINTSUPPORT_LIB
INCS = -I $(qde) -I $(qin) -I $(qin)/QtCore -I $(qin)/QtGui -I $(qin)/QtWidgets -I $(qin)/QtOpenGL -I inc/ -I build/ -I ../mat/inc/ -I ../fea/inc/
CXXFLAGS = -m64 -fPIC -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	CONF = debug
	CXXFLAGS += -ggdb3
else
	CONF = release
	CXXFLAGS += -Ofast -Wno-unused-result
endif

#ouput
OUT = dist/$(CONF)/gui.out
MAT = dist/$(CONF)/libmat.a
FEA = dist/$(CONF)/libfea.a

#interfaces
uif := $(sort $(shell find -path './ui/*.ui'))

#headers
hea := $(sort $(shell find -path './inc/*.h'))

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#qt files
uig := $(subst ./ui/, build/uic/, $(patsubst %.ui, %.h, $(uif)))
mog := $(subst ./inc/, build/moc/, $(patsubst %.h, %.cpp, $(hea)))

#objects
obj := $(subst ./src/, build/$(CONF)/obj/, $(patsubst %.cpp, %.o, $(src)))
obj += $(subst build/moc/, build/$(CONF)/moc/, $(patsubst %.cpp, %.o, $(mog)))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : fea $(OUT)
	@echo 'build($(CONF)): complete!'

run : fea $(OUT)
	@./$(OUT)

$(OUT) : ../fea/$(FEA) $(uig) $(mog) $(obj)
	@echo 'executable($(CONF)): $@'
	@mkdir -p dist/$(CONF)
	@$(CXX) -o $(OUT) $(obj) ../fea/$(FEA) ../mat/$(MAT) $(LIBS)
	
fea : 
	@cd ../fea && make -f Makefile m=$m t=$t

mat : 
	@cd ../mat && make -f Makefile m=$m t=$t

build/uic/%.h : ui/%.ui
	@echo 'uicing($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(uic) $< -o $@

build/moc/%.cpp: inc/%.h
	@echo 'mocing($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(moc) $< -o $@ -nw

build/$(CONF)/obj/%.o : src/%.cpp build/$(CONF)/obj/%.d
	@echo 'compiling($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

build/$(CONF)/moc/%.o : build/moc/%.cpp build/$(CONF)/moc/%.d
	@echo 'compiling($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf build/uic 
	@rm -rf build/moc
	@rm -rf dist/$(CONF)
	@rm -rf build/$(CONF) 
	@echo 'clean($(CONF)): complete!'

cleanlib : 
	@cd ../fea && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all run fea clean cleanlib cleanall print-%
