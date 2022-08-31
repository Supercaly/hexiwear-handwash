PRINCIPALI = tesi.tex ia_urb_thesis.sty Makefile

SORGENTI = $(wildcard sorgenti/*.tex)

FIGURE_PNG = $(wildcard figure/%.png)
FIGURE_JPG = $(wildcard figure/%.jpg)

.PHONY: all clean clean_all

all: tesi.pdf

tesi.pdf: $(PRINCIPALI) $(SORGENTI) $(FIGURE_PNG) $(FIGURE_JPG)
	pdflatex tesi.tex
	pdflatex tesi.tex
	# dvipdfm -o tesi.pdf tesi.dvi
	# acroread tesi.pdf &

clean:
	git clean -fXd
