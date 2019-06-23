#compiler
DEFS = 
INCS = -I inc/
WARS = -Wall -Wno-maybe-uninitialized
CXXFLAGS = -m64 -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	CONF = debug
	CXXFLAGS += -ggdb3
else
	CONF = release
	CXXFLAGS += -Ofast -Wno-unused-result
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
OUT = dist/$(PLA)/$(CONF)/libmat.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj = $(sort $(subst ./src/, build/$(PLA)/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep = $(subst .o,.d, $(obj))

#rules
all : $(OUT)
	@echo 'build($(PLA) - $(CONF)): complete!'

$(OUT) : $(obj)
	@mkdir -p dist/$(PLA)/$(CONF)
	@rm -rf $(OUT)
	@ar -rv $(OUT) $(obj)
	@ranlib $(OUT)
	@echo 'library($(PLA) - $(CONF)): $@'

build/$(PLA)/$(CONF)/%.o : src/%.cpp build/$(PLA)/$(CONF)/%.d
	@echo 'compiling($(PLA) - $(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@echo 'cleaning($(PLA) - $(CONF))'
	@rm -rf dist/$(PLA)/$(CONF) && rm -rf build/$(PLA)/$(CONF)

cleanlib : ;

cleanall : clean cleanlib

print-% :
	@echo $* = $($*)
	
.PHONY : all clean cleanlib cleanall print-%
