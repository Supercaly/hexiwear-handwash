PRINCIPALI = tesi.tex ia_urb_thesis.sty Makefile

SORGENTI = sorgenti/cap_1.tex sorgenti/cap_2.tex \
	   sorgenti/app_a.tex sorgenti/biblio.tex

FIGURE = figure/consumo_senza_ps.pdf figure/consumo_con_ps.pdf figure/power_saving.pdf

tesi.pdf: $(PRINCIPALI) $(SORGENTI) $(FIGURE)
	pdflatex tesi.tex
	pdflatex tesi.tex
	#dvipdfm -o tesi.pdf tesi.dvi
	#acroread tesi.pdf &

figure/consumo_senza_ps.pdf: figure/consumo_senza_ps.fig Makefile
	fig2dev -Lpdf -m0.40 -p1 figure/consumo_senza_ps.fig figure/consumo_senza_ps.pdf

figure/consumo_con_ps.pdf: figure/consumo_con_ps.fig Makefile
	fig2dev -Lpdf -m0.40 -p1 figure/consumo_con_ps.fig figure/consumo_con_ps.pdf

figure/power_saving.pdf: figure/power_saving.fig Makefile
	fig2dev -Lpdf -m0.40 -p1 figure/power_saving.fig figure/power_saving.pdf

pulisci:
	rm -f *.toc *.lof *.lot *.aux *.log sorgenti/*.aux

pulisci_tutto:
	rm -f *.pdf *.toc *.lof *.lot *.aux *.log sorgenti/*.aux figure/*.pdf figure/*.eps
