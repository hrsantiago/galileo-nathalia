#qt
qin = /usr/include/x86_64-linux-gnu/qt5
uic = /usr/lib/x86_64-linux-gnu/qt5/bin/uic
moc = /usr/lib/x86_64-linux-gnu/qt5/bin/moc
qde = /usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-g++

#compiler
WARS = -Wno-reorder -Wformat=0  -Wno-return-type
DEFS = -D QT_CORE_LIB -D QT_GUI_LIB -D QT_WIDGETS_LIB -D QT_SHARED -D QT_OPENGL_LIB -D QT_CONSOLE -D QT_PRINTSUPPORT_LIB
INCS = -I $(qde) -I $(qin) -I $(qin)/QtCore -I $(qin)/QtGui -I $(qin)/QtWidgets -I $(qin)/QtOpenGL -I inc/ -I build/ -I ../mat/inc/ -I ../fea/inc/
CXXFLAGS = -m64 -fPIC -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	m = d
	CONF = debug
	LIBS = -l qcustomplotd
	CXXFLAGS += -ggdb3
else
	CONF = release
	LIBS = -l qcustomplot
	CXXFLAGS += -Ofast -Wno-unused-result
endif

#platform
ifneq ($(t), w)
	CXX = g++
	PLA = linux
	OUT = dist/$(PLA)/$(CONF)/gui.out
	LIBS += -l gomp -l pthread -l GL -l GLU -l boost_system -l boost_filesystem -l Qt5Core -l Qt5Gui -l Qt5Widgets -l Qt5OpenGL -l Qt5PrintSupport -l umfpack
else
	PLA = windows
	CXX = x86_64-w64-mingw32-g++
	OUT = dist/$(PLA)/$(CONF)/gui.exe
	LIBS += -l opengl32 -l boost_filesystem -l Qt5Core -l Qt5Gui -l Qt5Widgets -l Qt5OpenGL -l Qt5PrintSupport -l umfpack
endif

#ouput
MAT = dist/$(PLA)/$(CONF)/libmat.a
FEA = dist/$(PLA)/$(CONF)/libfea.a

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
obj := $(subst ./src/, build/$(PLA)/$(CONF)/obj/, $(patsubst %.cpp, %.o, $(src)))
obj += $(subst build/moc/, build/$(PLA)/$(CONF)/moc/, $(patsubst %.cpp, %.o, $(mog)))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : fea $(OUT)
	@echo 'build($(PLA) - $(CONF)): complete!'

run : fea $(OUT)
	@./$(OUT)

$(OUT) : ../fea/$(FEA) $(uig) $(mog) $(obj)
	@echo 'executable($(PLA) - $(CONF)): $@'
	@mkdir -p dist/$(PLA)/$(CONF)
	@$(CXX) -o $(OUT) $(obj) ../fea/$(FEA) ../mat/$(MAT) $(LIBS)
	
fea : 
	@cd ../fea && make -f Makefile m=$m t=$t

mat : 
	@cd ../mat && make -f Makefile m=$m t=$t

build/uic/%.h : ui/%.ui
	@echo 'uicing($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(uic) $< -o $@

build/moc/%.cpp: inc/%.h
	@echo 'mocing($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(moc) $< -o $@ -nw

build/$(PLA)/$(CONF)/obj/%.o : src/%.cpp build/$(PLA)/$(CONF)/obj/%.d
	@echo 'compiling($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

build/$(PLA)/$(CONF)/moc/%.o : build/moc/%.cpp build/$(PLA)/$(CONF)/moc/%.d
	@echo 'compiling($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf dist/$(PLA)/$(CONF) && rm -rf build/$(PLA)/$(CONF) && rm -rf build/uic && rm -rf build/moc
	@echo 'clean($(PLA) - $(CONF)): complete!'

cleanlib : 
	@cd ../fea && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all run fea clean cleanlib cleanall print-%
