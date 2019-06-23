#compiler
DEFS = 
INCS = -I inc/ -I ../mat/inc/
WARS = -Wall -Wformat=0 -Wno-reorder -Wno-unused-result
CXXFLAGS = -m64 -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
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
else
	PLA = windows
	CXX = x86_64-w64-mingw32-g++
endif

#ouput
MAT = dist/$(PLA)/$(CONF)/libmat.a
OUT = dist/$(PLA)/$(CONF)/libfea.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj = $(sort $(subst ./src/, build/$(PLA)/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep = $(subst .o,.d, $(obj))

#rules
all : mat $(OUT)
	@echo 'build($(PLA) - $(CONF)): complete!'

$(OUT) : ../mat/$(MAT) $(obj)
	@mkdir -p dist/$(PLA)/$(CONF)
	@rm -rf $(OUT)
	@ar -rv $(OUT) $(obj)
	@ranlib $(OUT)
	@echo 'library($(PLA) - $(CONF)): $@'

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
	@cd ../mat && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all mat clean cleanlib cleanall print-%
