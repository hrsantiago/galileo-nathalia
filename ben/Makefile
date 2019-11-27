#compiler
CXX = g++
INCS = -I inc/ -I ../mat/inc/ -I ../fea/inc/
LIBS = -l GL -l boost_system -l boost_filesystem -l umfpack -l lapack
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

#ouput
OUT = dist/$(CONF)/test.out
MAT = dist/$(CONF)/libmat.a
FEA = dist/$(CONF)/libfea.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj := $(sort $(subst ./src/, build/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : fea $(OUT)
	@echo 'build($(CONF)): complete!'

run : fea $(OUT)
	@./$(OUT)

gui : 
	@cd ../gui && make -f Makefile run m=$m t=$t

$(OUT) : ../fea/$(FEA) $(obj)
	@echo 'executable($(CONF)): $@'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) -o $(OUT) $(obj) ../fea/$(FEA) ../mat/$(MAT) $(LIBS)
	
fea : 
	@cd ../fea && make -f Makefile m=$m t=$t

mat : 
	@cd ../mat && make -f Makefile m=$m t=$t

build/$(CONF)/%.o : src/%.cpp build/$(CONF)/%.d
	@echo 'compiling($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf dist/$(CONF) && rm -rf build/$(CONF)
	@echo 'clean($(CONF)): complete!'

cleanlib : 
	@cd ../fea && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all run gui fea clean cleanlib cleanall print-%
