---
geometry: "left=1cm,right=1cm,top=1cm,bottom=1.5cm"
---

# Testo della presentazione

Saluti vari... Il titolo della mia tesi è **Riconoscimento Del Lavaggio Delle Mani Tramite Smartwatch Per Ridurre La Trasmissione Da Contatto Di Agenti Patogeni.**

Di seguito vediamo gli argomenti che tratteremo in questa presentazione:

1. Contesto
2. Algoritmi di machine learning
3. Piattaforma wearable
4. Contributi del lavoro
5. Risultati sperimentali
6. Sviluppo applicazione
7. Conclusioni e Sviluppi futuri

## Contesto

Le linee guida della World Health Organization (WHO) indicano che il COVID-19 si trasmette principalmente attraverso il contatto corporeo e tramite le goccioline respiratorie (droplets). Per questo motivo una delle misure più importanti che una persona può mettere in pratica per prevenire la trasmissione di germi è prendersi cura dell’igiene delle proprie mani.

Per garantire un’igiene delle mani sufficiente la WHO suggerisce di lavarle accuratamente con acqua e sapone oppure sanificarle utilizzando una sostanza a base di alcool. Queste procedure sono composte da molti passaggi di diversa durata. Nonostante la provata efficienza di questi due metodi un numero significativo di persone non li mette in pratica a causa della loro difficoltà d’implementazione e si limitano a lavare le loro mani come hanno sempre fatto. Quando non si seguono le procedure raccomandate dalla WHO l’attività di lavare le mani è eseguita in maniera molto personale il che ci porta ad ottenere dei dati completamente non strutturati. 

In questa tesi ci siamo concentrati sul riconoscimento dei lavaggi e delle sanificazioni delle mani partendo da dati non strutturati; in particolare, introduciamo uno studio estensivo sull'abilità dei sistemi di machine learning di distinguere i gesti di lavaggio e sanificazione dal resto delle attività che ogni persona compie giornalmente senza l’utilizzo di strumenti invasivi, ma facendo unicamente affidamento su dispositivi indossabili comuni come smartwatch disponibili commercialmente. Inoltre presentiamo lo sviluppo di un’applicazione per smartwatch per il monitoraggio dell’igiene delle mani.

## Gli algoritmi di Machine Learning

In questo lavoro abbiamo valutato 4 tecnologie di machine learning, due appartengono al machine learning classico, mentre due fanno parte del moderno deep learning

- SVM
- ES-KNN
- CNN
- LSTM

## Piattaforma wearable

Durante la ricerca abbiamo utilizzato come smartwatch il dispositivo l’HEXIWEAR progettato da MikroElektronika ed NXP. HEXIWEAR è uno smartwatch completamente open-source dal costo di circa 50$ che consente lo sviluppo di applicazioni IoT in maniera molto semplice.

La piattaforma ha le seguenti caratteristiche: 

- 1024KB di memoria Flash e 256KB di memoria RAM
- accelerometro giroscopio a tre assi
- sensore di altitudine e pressione
- sensore di temperatura ed umidità
- sensore di luminosità
- sensore per il rilevamento dei battiti cardiaci
- diverse interfacce (USB, UART, SPI, I2C e SD-card)

Per programmare l’HEXIWEAR si è deciso di utilizzare la piattaforma MbedOS; un sistema operativo real-time open-source creato da ARM per i dispositivi embedded con memoria. Mbed fornisce agli sviluppatori un livello di astrazione tramite il quale possono creare applicazioni IoT scrivendole direttamente in C/C++.

## Contributi del lavoro

### Dataset

Per valutare l’accuratezza dei modelli di machine learning proposti è necessario addestrarli con un apposito dataset; in questa tesi abbiamo utilizzato: 

- uno creato ad hoc
- Daily Living Activities (DLA) disponibile pubblicamente

Per raccogliere i dati abbiamo utilizzato una IMU Shimmer3 equipaggiata con un accelerometro e un giroscopio triassale; questo dispositivo è progettato appositamente per essere indossato e può essere considerato una buona rappresentazione delle IMU correntemente equipaggiate all’interno degli smartwatch commerciali. Il dispositivo è stato programmato per campionare il suo accelerometro e giroscopio ad una frequenza di 100Hz. 

È importante notare che i soggetti non sono istruiti sulla corretta maniera in cui lavare o sanificare le mani e sono stati lasciati completamente liberi di usare il loro metodo abituale in modo da raccogliere dati il più possibile non strutturati. 

Il secondo dataset preso in considerazione è il Daily Living Activities (DLA) che è uno dei pochi dataset disponibili contenente dati di lavaggi di mani campionati attraverso sensori inerziali. Questo dataset è stato creato registrando diverse parti del corpo ed utilizzando differenti sensori indossabili. Il dataset è composto da differenti attività di uso comune, ma, abbiamo filtrato solo i dati raccolti dal polso e rimosso tutte quelle azioni che non hanno a che vedere con le mani. 

In questo dataset non sono presenti dati raccolti durante la sanificazione, inoltre non sono presenti tracce del giroscopio.

### Preprocessing

Per l'addestramento dei modelli le tracce sono state suddivise in finestre ognuna delle quali è considerata come un campione da usare per l’addestramento dei modelli. Ogni campione è stato etichettato durante la fase di raccolta dei dati marchiandolo con un’etichetta che rappresenta il soggetto ed una che rappresenta la categoria di azione compiuta.

Decidere la dimensione della finestra non è semplice poiché influisce sulle performance dei modelli in molti modi diversi; dev’essere larga abbastanza per includere informazioni significative sul'attività, ma non troppo grande da includere multipli eventi successivi.

Nel nostro caso abbiamo valutato finestre di dimensione dai 2 secondi fino ai 20 secondi utilizzando un overlap del 50% nella fase di training. 

### Estrazione delle feature

Per addestrare i modelli di machine learning tradizionali (SVM, KNN) i segnali in input devono essere processati in modo da estrarre una serie di feature significative; abbiamo delineato tre tipologie di feature: 

* base
    - media
    - deviazione standard 
    - massimo e minimo 
    - mediana
* hjorth
    - attività
    - mobilità
    - complessità
* shape
    - curtosi
    - skewness

### Classificatori deep learning

Nel caso degli approcci di deep learning non è necessario eseguire l’estrazione delle feature.

La rete LSTM riceve in input le sei sequenze (tre nel caso del DLA) originali della serie temporale la cui lunghezza dipende dalla dimensione della finestra.

Per quanto riguarda la CNN è necessario un passaggio di preprocessing che converte i dati delle serie temporali in un formato visuale, poiché CNN è un tipo di rete che lavora sulle immagini. In questo studio usiamo un metodo di codifica Gramian Angular Field.

### Gramian Angular Field (GAF)

Gramian Angular Field è una tecnica che converte le serie temporali in immagini mantenendo intatte le relazioni temporali fra i dati. In particolare usiamo due tecniche: 
- Gramian Summation Field (GASF)
- Gramian Difference Field (GADF)

Di seguito vediamo le due immagini GASF/GADF prodotte da una traccia reale di accelerometro con una finestra di 2 secondi.

## Risultati sperimentali 

### Caratterizzazione dei modelli - riconoscimento attività

Nella Tabella possiamo vedere i risultati ottenuti eseguendo l'addestramento e l'inferenza dei 4 modelli su PC e conducendo esperimenti di tipo hold-out con il 75% dei dati dedicato al training ed il 25% al testing.

I risultati fanno riferimento ai migliori modelli addestrati sia sul dataset costruito ad hoc che sul dataset DLA per il riconoscimento delle attività di lavaggio; come possiamo vedere i risultati sono tutti superiori al 90%, con un picco del 94%.

Per entrambi i dataset i risultati migliori sono stati ottenuti con finestre di lunghezza: SVM=12s, ERS-KNN=8s, CNN=6s, LSTM=2s e, nel caso di SVM ed ERS-KNN, utilizzando tutte le feature.

### Caratterizzazione dei modelli - riconoscimento utenti

Il secondo insieme di esperimenti ha lo scopo di identificare l'utente al posto dell’attività di per sé, per questo motivo abbiamo rimosso dai dataset tutti i sample che non fanno parte delle categorie di lavaggio e sanificazione e abbiamo cambiato le etichette per identificare la persona piuttosto che l’attività. 

Il riconoscimento degli utenti è un compito più semplice rispetto al riconoscimento delle attività; il dataset costruito ad hoc raggiunge un'accuratezza del 99%. Questo indica che le attività di lavaggio e sanificazione non strutturate contengono una sorta di impronta digitale che permette di identificare facilmente l’utente che le svolge. 

***CAPIRE LA SLIDE 19***

### Performance d'esecuzione

Per valutare la complessità dei modelli di machine learning proposti, durante ogni esperimento, abbiamo misurato le performance e l’utilizzo di memoria di ogni classificatore; i modelli sono stati implementati nella piattaforma Matlab2021a® utilizzando gli strumenti di machine learning integrati e sono stati eseguiti su un PC desktop con una CPU Intel® Core i9 equipaggiata con 32GB di memoria RAM ed una GPU Nvidia® Quadro® RTXTM 4000 con 16 GB di RAM.

La Tabella mostra le migliori performance di ogni modello; come atteso la fase di addestramento dei modelli di deep learning è la più lunga con un massimo di 470 secondi per l’LSTM, mentre l’ERS-KNN risulta essere il più veloce da addestrare con 5.8 secondi. 

Dal punto di vista dell’inferenza l’SVM sorpassa di molto gli altri modelli con 160us; è importante notare che nel caso dell’SVM e dell’ERS-KNN le tempistiche prevedono anche l’estrazione delle feature il cui costo non è trascurabile nel momento del passaggio all’esecuzione sul dispositivo embedded.

### Sensibilità alla dimensione della finestra di processing

Nei seguenti grafici vediamo le performance dei modelli al variare della dimensione della finestra di processing; sia SVM che ERS-KNN mostrano un trend delle pressoché piano, anche se da un certo punto in poi le metriche iniziano a peggiorare (12 secondi e 8 secondi). Un trend opposto si può vedere nei risultati ottenuti dai classificatori deep learning che decrementano in modo monotono all’aumentare della finestra.

### Sensibilità alle feature utilizzate

Oltre alla lunghezza della finestra di processing un altro parametro che influisce sulle prestazioni dei modelli di machine learning classici sono le feature che vengono calcolate. Per selezionare le feature utilizziamo un metodo di *forward feature selection* il quale aggiunge nuove feature valutandone l'impatto e partendo da un insieme vuoto. 

La Tabella mostra le performance per entrambi i classificatori al variare dei gruppi di feature selezionati. Le metriche mostrano tutte un aumento monotono che raggiunge le performance migliori nel momento in cui tutti i gruppi sono usati assieme; questo significa che ogni gruppo di feature aggiunge informazioni utili al processo di classificazione. Se non si considera il caso di tutte le feature assieme la seconda combinazione migliore è B+H, mentre il gruppo B produce buoni risultati rimanendo il più semplice da calcolare.

### Test di significatività non parametrici (McNemar)

Di seguito vediamo un esperimento che compara in maniera statistica le performance dei modelli fra loro. Utilizziamo tre variazioni del test di McNemar

- test asintotico
- test del p-value medio
- test condizionale esatto

Il test di McNemar si assicura che il disaccordo fra i due casi corrisponda, commentando sul fatto che i due modelli siano in disaccordo fra loro (oppure non lo siano affatto); questo test non dice nulla sul fatto che uno dei due modelli sia più o meno accurato dell'altro.

La Tabella mostra i risultati dei tre test condotti per ogni coppia di classificatori.
Per ogni test, viene riportato il valore booleano *h* che rappresenta la decisione del test quando si verifica l'ipotesi nulla, ovvero che i due classificatori abbiamo la stessa accuratezza nella predizione del risultato atteso.

Fra i nostri modelli solo SVM e ERS-KNN possono essere considerati statisticamente equivalenti dal punto di vista delle prestazioni di classificazione.

### Porting sul dispositivo wearable

Nelle prossime slide parleremo nel dettaglio del porting e degli esperimenti svolti sul dispositivo indossabile. La fase d'addestramento è svolta sempre su ambiente desktop, mentre l'inferenza è svolta interamente sul dispositivo wearable; gli esperimento sono di tipo hold-out (75% training e 25% testing) durante i quali misuriamo il consumo energetico e di memoria del dispositivo.

Come detto in precedenza lo smartwatch scelto ha a disposizione una ridotta quantità di memoria, per questo motivo siamo stati in grado di portare solo i modelli SVM ed LSTM (CNN richiede una grande quantità di memoria per le immagini GAF, mentre per KNN non abbiamo trovato nessuna libreria in grado di sfruttare la tecnica di Ensemble Random Subspace direttamente sui dispositivi).

Per eseguire i modelli sullo smartwatch sono state utilizzate due librerie: 

- TensorFlow 2.0 Lite Micro 
- libSVM

I modelli portati sono comunque stati ridotti di dimensione per poterli eseguire; in Figura si può vedere il modello LSTM dopo il porting.

### Sensibilità alla dimensione della finestra (SVM)

La Figura mostra la variazione delle performance del modello SVM al variare della dimensione della finestra di processing; possiamo notare subito che le performance sono diminuite di molto rispetto quelle su PC, in questo caso non superano mai l'88%.

Questo non ci sorprende ed è causato sicuramente dalla riduzione delle capacità dei modelli.

### Sensibilità alle feature utilizzate (SVM)

Il secondo esperimento condotto sul classificatore SVM consiste nel variare le feature calcolate ed osservare il cambiamento delle performance che ne segue. In questo caso le feature calcolate sono:

- media
- deviazione standard
- massimo e minimo

Utilizziamo il metodo di *forward feature selection* per scegliere i gruppi di feature da valutare

La Tabella mostra le performance al variare dei gruppi di feature calcolati; anche in questo caso le le performance migliori si ottengono quando tutti i gruppi sono usati assieme. La combinazione A+S è la seconda migliore e la sua Recall è la più alta di tutte.

#### Curve di Pareto (SVM) - dimensione della finestra

Nei seguenti grafici vediamo un confronto del modello SVM nell'esperimento sulla dimensione della finestra fra la memoria totale, il consumo di energia e la loss; i valori migliori sono quelli cerchiati (2 e 12) poiché hanno un consumo minore ed una loss minore.

#### Curve di Pareto (SVM) - feature utilizzate

Nei seguenti grafici vediamo un confronto del modello SVM nell'esperimento sulle feature utilizzate fra la memoria totale, il consumo di energia e la loss; in questo caso valori migliori sono quelli cerchiati (A+M e A+S).

### Sensibilità ai parametri strutturali (LSTM)

L’ultimo esperimento riguarda le performance del modello di deep learning LSTM al variare dei suoi parametri interni. I parametri interni sono il numero di unità nel modulo LSTM ed il numero di neuroni nascosti all’interno del primo livello densamente connesso.

In Tabella vediamo le metriche calcolate al variare dei seguenti parametri: 

- 64/256
- 32/128
- 16/64
- 8/32

Come possiamo immaginare il modello che produce i risultati migliori è quello con i parametri 64/256 con le performance attorno all'87%.

#### Curve di Pareto (LSTM)

Di seguito vediamo i grafici per il modello LSTM che confrontano 

- l'energia con la loss 
- la dimensione del modello con la loss
- il tempo d'inferenza con la loss

In tutti e tre i casi il valore migliore è 32/128.

## Sviluppo dell’applicazione

Dopo aver eseguito tutti gli esperimenti sui modelli di machine learning e di deep learning abbiamo deciso di sviluppare un’applicazione per l’HEXIWEAR che metta in pratica quanto scoperto in questa tesi ad un contesto comune e più utile per tutti gli utenti.

L’applicazione sviluppata ha lo scopo di contare il numero di attività di lavaggio e di sanificazione della mani compiute da un utente nell’arco di una giornata; per fare questo raccoglie continuamente i dati dei sensori di accelerometro e giroscopio in real-time ed utilizza il modello LSTM visto in precedenza per classificare l’attività svolta ed aggiornare la UI.

L’applicazione nativa è stata sviluppata interamente in C/C++. In questa figura vediamo un diagramma di flusso dei tre thread interni all'applicazione. ***SPIEGO COSA FANNO I THREAD***

## Conclusioni e Sviluppi futuri

In questa tesi abbiamo:

- testato diversi modelli di machine learning
- eseguito il porting su una piattaforma wearable
- caratterizzato le performance e l'energia dei modelli
- sviluppato un'applicazione real-time

Gli sviluppi futuri prevedono:

- ampliare il dataset ad hoc
- testare smartwatch più performanti
- utilizzare tecniche di ottimizzazione dei modelli di ML come il partizionamento verticale

Da una parte di questo lavoro di tesi è stato prodotto un articolo scientifico pubblicato nel 2022 sulla rivista specializzata IEEE Access.

Grazie per l'attenzione, domande?