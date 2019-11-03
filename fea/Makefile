#compiler
CXX = g++
INCS = -I inc/ -I ../mat/inc/
WARS = -Wall -Wformat=0 -Wno-reorder -Wno-unused-result -Wno-write-strings
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
MAT = dist/$(CONF)/libmat.a
OUT = dist/$(CONF)/libfea.a

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#objects
obj = $(sort $(subst ./src/, build/$(CONF)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep = $(subst .o,.d, $(obj))

#rules
all : mat $(OUT)
	@echo 'build($(CONF)): complete!'

$(OUT) : ../mat/$(MAT) $(obj)
	@mkdir -p dist/$(CONF)
	@rm -rf $(OUT)
	@ar -rv $(OUT) $(obj)
	@ranlib $(OUT)
	@echo 'library($(CONF)): $@'

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
	@cd ../mat && make -f Makefile m=$m t=$t cleanall

cleanall : cleanlib clean

print-% :
	@echo $* = $($*)
	
.PHONY : all mat clean cleanlib cleanall print-%
