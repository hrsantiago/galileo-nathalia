#mode
ifneq ($(m), r)
	m = d
endif

#platform
ifneq ($(t), w)
	t = l
endif

#rules
all : 
	@cd ./ben && make -f Makefile m=$m t=$t
	@cd ./gui && make -f Makefile m=$m t=$t

ben : 
	@cd ./ben && make -f Makefile run m=$m t=$t
	
gui : 
	@cd ./gui && make -f Makefile run m=$m t=$t

run : 
	@cd ./ben && make -f Makefile run m=$m t=$t
	@cd ./gui && make -f Makefile run m=$m t=$t

clean :
	@cd ./ben && make -f Makefile m=$m t=$t clean
	@cd ./gui && make -f Makefile m=$m t=$t clean

cleanlib : 
	@cd ./ben && make -f Makefile m=$m t=$t cleanlib
	@cd ./gui && make -f Makefile m=$m t=$t cleanlib

cleanall : 
	@cd ./ben && make -f Makefile m=$m t=$t cleanall
	@cd ./gui && make -f Makefile m=$m t=$t cleanall

install : 
	@mkdir -p /usr/share/galileo
	@cd ./gui && make -f Makefile m=r
	@cp -r gui/rsc/ /usr/share/galileo/
	@cp install/galileo.jpg /usr/share/icons/
	@cp install/galileo.desktop /usr/share/applications/
	@cp ./gui/dist/Linux/release/gui.out ./gui/dist/Linux/release/galileo
	@mv ./gui/dist/Linux/release/galileo /usr/bin/
	@echo Galileo: Installed!
	
.PHONY : all ben gui run clean cleanlib cleanall install
