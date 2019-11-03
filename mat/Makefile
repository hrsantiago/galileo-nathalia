#compiler
CXX = g++
INCS = -I inc/
WARS = -Wall -Wno-maybe-uninitialized -Wno-unused-result -Wno-parentheses
CXXFLAGS = -m64 -std=c++11 -pipe -pthread -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	CONF = debug
	CXXFLAGS += -ggdb3
else
	CONF = release
	CXXFLAGS += -Ofast
endif

#ouput
OUT = dist/$(CONF)/libmat.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj = $(sort $(subst ./src/, build/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep = $(subst .o,.d, $(obj))

#rules
all : $(OUT)
	@echo 'build($(CONF)): complete!'

$(OUT) : $(obj)
	@mkdir -p dist/$(CONF)
	@rm -rf $(OUT)
	@ar -rv $(OUT) $(obj)
	@ranlib $(OUT)
	@echo 'library($(CONF)): $@'

build/$(CONF)/%.o : src/%.cpp build/$(CONF)/%.d
	@echo 'compiling($(CONF)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@echo 'cleaning($(CONF))'
	@rm -rf dist/$(CONF) && rm -rf build/$(CONF)

cleanlib : ;

cleanall : clean cleanlib

print-% :
	@echo $* = $($*)
	
.PHONY : all clean cleanlib cleanall print-%
