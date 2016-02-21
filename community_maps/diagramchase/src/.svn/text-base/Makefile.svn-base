# makefile for diagram chase map
# felix salfelder 2007

FILELIST=diag.png config.xml diag_thumb.jpg diag_sky.jpg particle.png
PREFIX=~/.wormux

#particlesize
PSIZE=64

#animation stepping
PNUM=10

all: $(FILELIST)

diag_sky.jpg:
	convert -size 2000x2000 gradient:white-blue $@


config.xml: config.xml.tpl
	cat $< | sed -e s/%%PSIZE%%/$(PSIZE)/g \
		| sed -e s/%%PNUM%%/$(PNUM)/g > $@

diag_thumb.jpg: diag.tmp.epsi 
	convert -rotate -90 -density 100x100 -extent 300x225 $< $@

diag.tmp.tex: diag.tex
	grep -v srcltx $<  > $@


diag.tmp.dvi: diag.tmp.tex
	latex $<

	
diag.tmp.ps: diag.tmp.dvi
	dvips -t landscape $<
	#dvips -t a3 $<

diag.tmp.epsi: diag.tmp.ps
	ps2epsi $<

diag.png: diag.tmp.epsi 
	convert -rotate -90 -density 300x300 -channel RGBA $< $@

particle.0.epsi: particle.0.ps
	ps2epsi $<


particle.0.png: particle.0.epsi Makefile
	for i in `seq $(PNUM)`; do SEQ+=" ( -rotate `expr $$i \* 9` -density 500x500 -channel RGBA -resize $(PSIZE)x$(PSIZE) -background transparent  particle.0.epsi )"; done; \
	convert $$SEQ   +append particle.0.png 


particle.0.ps: particle.0.dvi
	dvips -a3 $<
particle.0.dvi: particle.0.tex
	latex $<

particle.png: particle.0.png
	convert particle.0.png +append $@


install: $(FILELIST)
#	[ -e $(PREFIX)/map/diagram ] || mkdir -f $(PREFIX)/map/diagram 
	install -d $(PREFIX)
	install -d $(PREFIX)/map
	install -d $(PREFIX)/map/diagram
	install -m 444 $(FILELIST) $(PREFIX)/map/diagram

clean:
	rm -rf *tmp*

realclean: clean
	rm -rf diagram diag.png diag.epsi diag.dvi *.log *.ps *.epsi $(FILELIST) *~ *.dvi *.aux *.png *.jpg

