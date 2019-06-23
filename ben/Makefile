#compiler
DEFS = 
INCS = -I inc/ -I ../mat/inc/ -I ../fea/inc/
WARS = -Wall -Wno-unused-variable -Wno-unused-variable -Wno-format-security -Wno-return-type -Wno-unused-function -Wformat-overflow=0
CXXFLAGS = -m64 -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	m = d
	CONF = debug
	CXXFLAGS += -ggdb3
else
	CONF = release
	CXXFLAGS += -Ofast
endif

#platform
ifneq ($(t), w)
	CXX = g++
	PLA = linux
	OUT = dist/$(PLA)/$(CONF)/test.out
	LIBS = -l GL -l boost_system -l boost_filesystem -l umfpack
else
	PLA = windows
	CXX = x86_64-w64-mingw32-g++
	OUT = dist/$(PLA)/$(CONF)/test.exe
	LIBS = -l opengl32 -l boost_system -l boost_filesystem -l umfpack
endif

#ouput
MAT = dist/$(PLA)/$(CONF)/libmat.a
FEA = dist/$(PLA)/$(CONF)/libfea.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj := $(sort $(subst ./src/, build/$(PLA)/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : fea $(OUT)
	@echo 'build($(PLA) - $(CONF)): complete!'

run : fea $(OUT)
	@./$(OUT)

gui : 
	@cd ../gui && make -f Makefile run m=$m t=$t

$(OUT) : ../fea/$(FEA) $(obj)
	@echo 'executable($(PLA) - $(CONF)): $@'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) -o $(OUT) $(obj) ../fea/$(FEA) ../mat/$(MAT) $(LIBS)
	
fea : 
	@cd ../fea && make -f Makefile m=$m t=$t

mat : 
	@cd ../mat && make -f Makefile m=$m t=$t

build/$(PLA)/$(CONF)/%.o : src/%.cpp build/$(PLA)/$(CONF)/%.d
	@echo 'compiling($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf dist/$(PLA)/$(CONF) && rm -rf build/$(PLA)/$(CONF)
	@echo 'clean($(PLA) - $(CONF)): complete!'

cleanlib : 
	@cd ../fea && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all run gui fea clean cleanlib cleanall print-%
