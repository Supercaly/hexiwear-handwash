PRINCIPALI = tesi.tex ia_urb_thesis.sty Makefile

SORGENTI = $(wildcard sorgenti/*.tex)

FIGURE_PNG = $(wildcard figure/*.png)
FIGURE_JPG = $(wildcard figure/*.jpg)
FIGURE_EPS = $(wildcard figure/*.eps)
FIGURE_PDF = $(patsubst %.eps,%.pdf,${FIGURE_EPS})

.PHONY: all clean clean_all

all: tesi.pdf

tesi.pdf: $(PRINCIPALI) $(SORGENTI) $(FIGURE_PNG) $(FIGURE_JPG) $(FIGURE_PDF)
	pdflatex tesi.tex
	pdflatex tesi.tex
	# dvipdfm -o tesi.pdf tesi.dvi
	# acroread tesi.pdf &

%.pdf: $(FIGURE_EPS)
	epstopdf $*.eps $@
	
clean:
	git clean -fXd
