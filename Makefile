RTAI = 

MODE=release #default mode

all: release

release: MODE=release
# release: utils engine models pdae pdif pdppt pdme ini simu 
release: utils engine models py2pdevs pdae pdif pdppt pdme ini simu 
# release: utils engine models py2pdevs pdae pdif pdppt pdme ini bin/lcd bin/knob bin/rtview simu 
no-py2powerdevs: utils engine models pdae pdif pdppt pdme ini simu

debug: MODE=debug
debug: utils engine models py2pdevs pdae pdif pdppt pdme ini bin/lcd bin/knob bin/rtview simu

.PHONY: engine

gui: pdme pdppt pdae pdif ini bin/lcd bin/knob bin/rtview

utils:
	make -C src/utils MODE=$(MODE)

engine: utils
	make -C src/engine MODE=$(MODE)
	
models: utils
	make -C atomics MODE=$(MODE)
	
py2pdevs: utils engine models pdppt
	make -C src/py2pdevs MODE=$(MODE)	

py2pdevs_remote: utils engine models pdppt
	make -C src/py2pdevs -f Makefile2 MODE=$(MODE)	
	
simu: utils engine models
	make -C build MODE=$(MODE)	

bin/lcd:src/atomics/lcd/* 
	cd ./src/atomics/lcd && qmake CONFIG+=$(MODE) 
	make -C ./src/atomics/lcd MODE=$(MODE)

bin/knob:src/atomics/knob/* 
	cd ./src/atomics/knob && qmake CONFIG+=$(MODE) 
	make -C ./src/atomics/knob MODE=$(MODE)

bin/rtview:src/atomics/rtview
	cd ./src/atomics/rtview && qmake CONFIG+=$(MODE) 
	make -C ./src/atomics/rtview MODE=$(MODE)

pdae:
	cd ./src/pdae && qmake CONFIG+=$(MODE) 
	make -C ./src/pdae MODE=$(MODE)

pdppt:
	cd ./src/pdppt && qmake CONFIG+=$(MODE) 
	make -C ./src/pdppt MODE=$(MODE)

pdif:
ifeq ($(RTAI),yes)
	cd ./src/pdif && qmake CONFIG+=$(MODE) QMAKE_CXXFLAGS+=-DRTAIOS 
else
	cd ./src/pdif && qmake CONFIG+=$(MODE) 
endif
	make -C ./src/pdif MODE=$(MODE)

doc/PD_UserGuide.pdf: src/doc/commands.tex src/doc/pd_userguide.kilepr src/doc/chap_intro.tex src/doc/images src/doc/images/pdevsmodel.jpg src/doc/images/pdmodeling.jpg src/doc/images/pdsimulation.jpg src/doc/chap_get_start.tex src/doc/PD_UserGuide.tex
	cd ./src/doc && pdflatex PD_UserGuide.tex -output-directory ../../doc
	cp ./src/doc/PD_UserGuide.pdf ./doc


pdme: ini
	cd ./src/pdme_v2 && qmake CONFIG+=$(MODE) 
	make -C ./src/pdme_v2 MODE=$(MODE)

ini:
ifeq (,$(wildcard bin/powerdevs.ini))
	@cp bin/original.ini bin/powerdevs.ini
endif

clean:
	make -C src/utils clean
	make -C src/engine clean
	make -C atomics clean
	make -C src/py2pdevs clean
	-make -C ./src/pdae clean
	-make -C ./src/pdif clean
	-make -C ./src/pdppt clean
	-make -C ./src/pdme_v2 clean
	-make -C ./src/atomics/lcd clean
	-make -C ./src/atomics/knob clean
	-make -C ./src/atomics/rtview clean
	rm -rf bin/pdae bin/pdppt bin/pdif bin/pdme bin/lcd bin/slider bin/rtview
