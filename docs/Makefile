PRINCIPALI = tesi.tex ia_urb_thesis.sty Makefile

SORGENTI = $(wildcard sorgenti/*.tex)

.PHONY: all clean clean_all

all: tesi.pdf

tesi.pdf: $(PRINCIPALI) $(SORGENTI) $(FIGURE)
	pdflatex tesi.tex
	pdflatex tesi.tex
	#dvipdfm -o tesi.pdf tesi.dvi
	# acroread tesi.pdf &

figure/consumo_senza_ps.pdf: figure/consumo_senza_ps.fig Makefile
	fig2dev -Lpdf -m0.40 -p1 figure/consumo_senza_ps.fig figure/consumo_senza_ps.pdf

clean:
	git clean -fXd
	
clean_all: clean
	rm -f *.pdf figure/*.pdf figure/*.eps
