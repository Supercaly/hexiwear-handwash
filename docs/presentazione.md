---
# title: "Habits"
# author: John Doe
# date: March 22, 2005
geometry: "left=1cm,right=1cm,top=1cm,bottom=1.5cm"
# output: pdf_document
---

# Testo della presentazione

Saluti vari... Il titolo della mia tesi è **Riconoscimento Del Lavaggio Delle Mani Tramite Smartwatch Per Ridurre La Trasmissione Da Contatto Di Agenti Patogeni.**

Di seguito vediamo i vari argomenti che tratteremo in questa presentazione:

1. Gli algoritmi di machine learning
2. L'HEXIWEAR
3. Metodi
4. Esperimenti su PC
5. Esperimenti sull'HEXIWEAR
6. Sviluppo dell’applicazione
7. Sviluppi futuri

## Intro

Le linee guida della World Health Organization (WHO) indicano che la malattia SARS-CoV-2 COVID-19 si trasmette principalmente attraverso il contatto corporeo e tramite le goccioline respiratorie (droplets). Per questo motivo una delle misure più importanti che una persona può mettere in pratica per prevenire la trasmissione di germi è prendersi cura dell’igiene delle proprie mani.

Per garantire un’igiene delle mani sufficiente la WHO suggerisce di lavarle accuratamente con acqua e sapone oppure sanificarle utilizzando una sostanza a base di alcool. Queste procedure sono composte da molti passaggi di diversa durata; lavare le mani con acqua e sapone prevede undici step dalla durata complessiva di 40/60 secondi mentre la sanificazione è formata solo da 8 passaggi di 20/30 secondi. Nonostante la provata efficienza di questi due metodi un numero significativo di persone non li mette in pratica a causa della loro difficoltà d’implementazione e si limitano a lavare le loro mani come hanno sempre fatto. Quando non si seguono le procedure raccomandate dalla WHO l’attività di lavare le mani è eseguita in maniera molto personale il che ci porta ad ottenere dei dati completamente non strutturati. 

In questa tesi ci siamo concentrati sul riconoscimento dei lavaggi e delle sanificazioni delle mani partendo da dati non strutturati, in particolare, abbiamo introdotto uno studio estensivo sull'abilità dei sistemi di machine learning di distinguere i gesti di lavaggio e sanificazione dal resto delle attività che ogni persona compie giornalmente senza l’utilizzo di strumenti invasivi, ma facendo unicamente affidamento su dispositivi indossabili comuni come smartwatch disponibili commercialmente. Inoltre abbiamo presentato lo sviluppo di un’applicazione per il monitoraggio dell’igiene delle mani creata per lo smartwatch HEXIWEAR e che fa largo utilizzo dei modelli di machine learning per il riconoscimento degli eventi.

## Gli algoritmi di Machine Learning

Per quanto riguarda gli algoritmi di machine learning, in questo lavoro abbiamo valutato in totale 4 modelli: due modelli di machine learning classici (SVM, ES-KNN) e due modelli di deep learning (CNN, LSTM).

### Support Vector Machine (SVM)

SVM è uno degli algoritmi di machine learning più semplici e più utilizzati tradizionalmente per i problemi di regressione e di classificazione con un numero ridotto di campioni. Un modello SVM rappresenta i dati in input come dei punti nello spazio, mappati in modo tale che i dati appartenenti alle diverse classi siano separati da un margine ampio. I dati sconosciuti sono inseriti nello stesso spazio e la loro categoria dipende dalla regione in cui cadono.
Una buona separazione si può ottenere dall'iperpiano con la distanza maggiore dal punto del training set più vicino di ognuna delle classi; in generale maggiore è il margine fra questi punti, minore è l’errore di generalizzazione commesso dal classificatore.

### Ensemble Subspace con K-Nearest Neighbors (ES-KNN)

K-Nearest Neighbors (KNN) è un’altro algoritmo di apprendimento automatico molto utilizzato per le sue prestazioni e semplicità. L’algoritmo sfrutta una metrica di distanza per assegnare ad un dato ignoto una classe in base alla maggioranza dei voti dei suoi k vicini. Il parametro k è un intero positivo piccolo. Un punto è assegnato alla classe C se questa è la più frequente fra i k esempi più vicini all’oggetto sotto esame, i vicini sono presi da un insieme di oggetti per cui è nota la classificazione corretta.

Per migliorare le performance di classificazione del KNN nella letteratura sono state proposte delle tecniche di ensemble. In generale le tecniche di ensemble consistono nell’addestrare molteplici modelli per poi combinarli assieme, ad esempio facendo la media, per ottenere l’output desiderato. Un modo per creare un ensemble è quello di addestrare i classificatori con dataset differenti ottenuti suddividendo il training set originario; questa tecnica è nota come ensemble subspace. In questa tesi ci concentriamo su una classe di ensemble subspace nota come Ensemble Random Subspace KNN (ERS-KNN) per cui il training set è suddiviso randomicamente nei subspace.
  
### Convolutional Neural Network (CNN)

CNN è uno strumento di classificazione dei dati che appartiene alla categoria del deep learning, molto utilizzato negli ultimi anni grazie alla sua struttura che segue un approccio basato sulla computer vision. La sua struttura è ispirata ai processi biologici che avvengono nella corteccia visiva degli animali. Nella pratica CNN è un’estensione di un Multi-Layer Perceptron composto da uno strato di input, vari strati convoluzionali, di pooling e densamente connessi fra loro. Lo strato di input ha il compito di raccogliere i dati sotto forma di pixel di un’immagine e passarli agli strati successivi; i livelli convoluzionali sono il nucleo della rete e contengono diversi kernel che convolgono con i dati di input. La convoluzione estrae automaticamente le feature più significative dai dati riducendo la dimensionalità. Infine uno o più livelli densamente connessi si comportano come un Perceptron tradizionale prendendo le feature in input e producendo una classe in output.

### Long Short-Term Memory (LSTM)

LSTM è una variante delle Reti Neurali Ricorrenti (RNN) utilizzata nel campo del deep learning e dell’intelligenza artificiale per la sua abilità nel classificare e fare predizioni basandosi sui dati delle serie temporali; per questo motivo è spesso usata in ambiti come il riconoscimento della scrittura, il riconoscimento del parlato e la traduzione di testi.

Un’unità LSTM è composta da una cella, un gate di input, un gate di output ed un forget gate; la cella ricorda valori per un tempo arbitrario, mentre i tre gate regolano il flusso delle informazioni dentro e fuori dalla cella prendendo decisioni su cosa ricordare e cosa dimenticare. Il nome LSTM si riferisce al fatto che questo modello ha sia una memoria a lungo termine che una a breve termine, analoga alla struttura del cervello umano.

## L'HEXIWEAR

Lo smartwatch utilizzato durante la ricerca è l’HEXIWEAR progettato da MikroElektronika ed NXP. HEXIWEAR è uno smartwatch completamente open-source dal costo di circa 50$ che consente lo sviluppo di applicazioni IoT in maniera molto semplice; al suo interno monta ben due MCU: la prima è una Kinetis K64F con architettura ARM Cortex- M4, una frequenza massima di 120MHz, 1024KB di memoria Flash e 256KB di memoria RAM; la seconda è un SoC Kinetis KW40z che permette l’utilizzo delle funzionalità Bluetooth Low Energy. 
All’interno dell’HEXIWEAR sono inoltre presenti una grande quantità di sensori integrati, come ad esempio:

- accelerometro e magnetometro a sei assi FXOS8700 
- giroscopio a tre assi FXAS21002
- sensore di altitudine e pressione MPL3115A2
- sensore di temperatura ed umidità HTU21D
- sensore di luminosità TSL2561
- sensore per il rilevamento dei battiti cardiaci MAX30101
- display OLED da 1.1 pollici
- batteria ricaricabile Li-Po da 190mAh 
- diverse interfacce (USB, UART, SPI, I2C e SD-card)

Per facilitare lo sviluppo ed il debug delle applicazioni HEXIWEAR ha una docking station al quale è possibile connettere lo smartwatch.
Per sviluppare le applicazioni nell’HEXIWEAR si è deciso di utilizzare la piattaforma MbedOS. Mbed è un sistema operativo real-time open-source creato da ARM per i dispositivi embedded con memoria limitata e che richiedono un basso consumo di corrente. Mbed fornisce agli sviluppatori un livello di astrazione tramite il quale possono creare applicazioni IoT scrivendole direttamente in C/C++; esso consiste in un insieme di librerie che forniscono i driver per le periferiche del micro-controllore, l’accesso alla rete, il controllo dell’ambiente RTOS, gli strumenti di build e degli script per test e debug, inoltre è disponibile una repository online dalla quale é possibile scaricare librerie esterne create dalla community.

## Metodi 

### I dataset

Prima di poter valutare l’accuratezza dei modelli di machine learning proposti è necessario addestrarli con un apposito dataset; in questa tesi i modelli sono stati valutati utilizzandone due: uno creato ad hoc per questa tesi ed il Daily Living Activities (DLA) disponibile pubblicamente.
Al momento della ricerca non sono disponibili dataset pubblici contenenti dati di lavaggi e sanificazioni delle mani mediante segnali di accelerometro e giroscopio; per questo motivo si è deciso di crearne uno personalizzato raccogliendo i dati da una Inertial Measurement Unit (IMU) posizionata sul polso della mano dominante di quattro partecipanti durante attività di vita reale.

Per raccogliere i dati abbiamo utilizzato una IMU Shimmer3 equipaggiata con un accelerometro e un giroscopio triassale; questo dispositivo è progettato appositamente per essere indossato ed è spesso utilizzato nel monitoraggio delle attività nell’ambito delle scienze sportive e mediche. Lo Shimmer3 può essere considerato una buona rappresentazione delle IMU correntemente equipaggiate all’interno degli smartwatch commerciali. Il dispositivo è stato programmato per campionare il suo accelerometro e giroscopio ad una frequenza di 100Hz e salvare i dati raccolti in una SD-card interna. Per rimuovere il bias dei sensori il dispositivo è stato calibrato all’inizio della ricerca lasciandolo su una superficie piana per circa 30 secondi in modo da ottenere una traccia di calibrazione. 

È importante notare che i soggetti non sono istruiti sulla corretta maniera in cui lavare o sanificare le mani e sono stati lasciati completamente liberi di usare il loro metodo abituale in modo da raccogliere dati il più possibile non strutturati. 

Il secondo dataset preso in considerazione è il Daily Living Activities (DLA) che è uno dei pochi dataset disponibili contenente dati di lavaggi di mani campionati attraverso sensori inerziali. Questo dataset è stato creato registrando diverse parti del corpo ed utilizzando differenti sensori indossabili da otto volontari in salute. Il dataset è composto da differenti attività di uso comune, ma, abbiamo filtrato solo i dati raccolti dal polso e rimosso tutte quelle azioni che non hanno a che vedere con le mani. In questo dataset non sono presenti dati raccolti durante la sanificazione, inoltre i sensori utilizzati per la raccolta non sono equipaggiati con un giroscopio il chè può sembrare svantaggioso, ma ci consente di soppesare l’utilità dei segnali del giroscopio e di valutare l’abilità dei modelli proposti nel generalizzare.

### Le finestre di campionamento

Le tracce raccolte dal dataset costruito ad hoc sono composte da sei segnali distinti, tre per l’accelerometro e tre per il giroscopio, mentre quelle del dataset DLA hanno solo tre tracce per l’accelerometro; in entrambi i casi le tracce sono state suddivise in finestre ognuna delle quali è considerata come un campione da usare per l’addestramento dei modelli. Ogni campione è stato etichettato durante la fase di raccolta dei dati marchiandolo con un’etichetta che rappresenta il soggetto ed una che rappresenta la categoria di azione compiuta fra queste: lavaggio, sanificazione, altro.

Decidere la dimensione della finestra non è semplice poiché influisce sulle performance dei modelli in molti modi diversi; infatti, dev’essere larga abbastanza per includere informazioni significative sulla singola attività, ma non dev’essere troppo grande da includere multipli eventi successivi.
Nel nostro caso abbiamo valutato finestre di dimensione dai 2 secondi fino ai 20 secondi e senza overlap fra le finestre. 

### I classificatori

Per addestrare e testare l’accuratezza dei modelli di machine learning tradizionali i segnali in input devono essere processati in modo da estrarre una serie di feature significative; abbiamo delineato tre tipologie di feature: 

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

Nel caso degli approcci di deep learning non è necessario eseguire l’estrazione delle feature utilizzando. Per quanto riguarda la CNN è necessario un passaggio di preprocessing che converte i dati delle serie temporali in un formato visuale. In questo studio usiamo un metodo che codifica i dati delle serie temporali in immagini chiamato Gramian Angular Summation/Difference Field(GASF, GADF). Questo metodo rappresenta le serie temporali in un sistema di coordinate polari, invece delle tipiche coordinate Cartesiane.

D’altro canto la rete LSTM riceve in input le sei sequenze(tre nel caso del DLA) originali della serie temporale la cui lunghezza dipende dalla dimensione della finestra(WS).

### Metriche dei classificatori

Per valutare la qualità dei modelli proposti effettuiamo un test di k-fold cross-validation con k = 5 dal quale ricaviamo una matrice di confusione e gli indicatori $TP_i$, $TN_i$, $FP_i$, $FN_i$, con $i \in [1 \dots N]$, dove $i$ è un indice che identifica la classe specifica trattandosi di classificatori multi-classe

- $TP_i$ numero di veri positivi per la classe i
- $TN_i$ numero di veri negativi 
- $FP_i$ numero di falsi positivi 
- $FN_i$ numero di falsi negativi

Da questi identificatori abbiamo in seguito ricavato le vere e proprie metriche di qualità.

## Esperimenti su PC

In questa sezione discutiamo i risultati degli esperimenti sui modelli proposti condotti in ambiente PC.

### Metriche d’accuratezza

Nella Tabella possiamo vedere i risultati delle metriche ottenute dai quattro classificatori proposti sia sul dataset costruito ad hoc che sul dataset DLA; questi valori fanno riferimento ai migliori risultati ricavati da ogni modello durante un esperimento di 5-fold cross-validation variando la lunghezza della finestra di campionamento e, per i modelli classici, il numero di features; per ogni metrica è riportato in grassetto il miglior risultato fra i quattro modelli.

Nel caso del dataset ad hoc il classificatore SVM produce il miglior valore per la metrica di Recall (0.934), mentre LSTM ottiene la migliore Accuracy (0.947) e ERS-KNN produce le migliori Precision e F1-score(0.941 e 0.936). Per quanto riguarda il dataset DLA il modello CNN supera gli altri modelli in ogni metrica calcolata, nonostante questo le performance di ERS-KNN rimangono sopra al 91%. 

Paragonato agli esperimenti sul dataset ad hoc le performance hanno subito una decrescita del 1/8%; questa riduzione di può essere causata dal numero maggiore di etichette le quali tendono a disperdere maggiormente i risultati. Un’altra causa può essere la mancanza di segnali del giroscopio che impattano significativamente sulle capacità di classificazione.

Per entrambi i dataset i risultati migliori sono stati ottenuti con finestre di lunghezza: SVM=12s, ERS-KNN=8s, CNN=6s, LSTM=2s e, nel caso di SVM ed ERS-KNN, utilizzando tutte le feature.

### Riconoscimento degli utenti

Il secondo insieme di esperimenti ha lo scopo di identificare la persona che lava o sanifica le mani al posto dell’attività di per sé, per questo motivo abbiamo rimosso dai dataset tutti i sample che non fanno parte di queste categorie e abbiamo cambiato le etichette per identificare la persona piuttosto che l’attività. 

Il riconoscimento degli utenti è un compito più semplice rispetto al riconoscimento delle attività; per il dataset costruito ad hoc la migliore accuratezza è dello 0.99 ottenuta con il classificatore SVM, questo indica che le attività di lavaggio e sanificazione non strutturate contengono una sorta di impronta digitale che permette la facile identificazione dell’utente che le svolge. Come nel caso del dataset costruito ad hoc, anche le metriche ricavate dal dataset DLA mostrano una maggiore semplicità nel riconoscere gli utenti rispetto l’attività; in questo caso i classificatori classici superano quelli di deep learning, ma, anche se gli esperimenti hanno una buona accuratezza (sopra il 95%), in generale si ha comunque una riduzione nelle performance rispetto al dataset costruito ad hoc.

### Utilizzo di memoria e tempistiche

Per valutare la complessità dei modelli di machine learning proposti, durante ogni esperimento, abbiamo misurato le performance e l’utilizzo di memoria di ogni classificatore; i modelli sono stati implementati nella piattaforma Matlab2021a® utilizzando gli strumenti di machine learning integrati e sono stati eseguiti su un PC desktop con una CPU Intel® Core i9 equipaggiata con 32GB di memoria RAM ed una GPU Nvidia® Quadro® RTXTM 4000. Sia la fase di addestramento che quella d’inferenza sono state svolte impostando la GPU come ambiente d’esecuzione in modo da sfruttare a pieno le potenzialità del parallelismo CUDA®.

La Tabella mostra le performance e l’utilizzo di memoria di ogni modello; come atteso la fase di addestramento dei modelli di deep learning è la più lunga con un massimo di 470 secondi misurati per l’LSTM, mentre l’ERS-KNN risulta essere il più veloce da addestrare con 5.8 secondi. Dal punto di vista dell’inferenza l’SVM sorpassa di molto gli altri modelli con 160us; è importante notare che nel caso dell’SVM e dell’ERS-KNN le tempistiche prevedono anche l’estrazione delle feature il cui costo non è trascurabile nel momento del passaggio all’esecuzione sul dispositivo embedded.

### Lunghezza della finestra

La dimensione della finestra di processing influisce in molti modi sulla classificazione dei modelli. Sia SVM che ERS-KNN mostrano un trend delle performance pressoché piano, anche se da un certo punto in poi le metriche iniziano a deteriorarsi; in particolare l’SVM vede un miglioramento delle performance fino ad una finestra di dimensione 12 secondi dopodiché Precision, Recall ed F1-score diminuiscono. In maniera del tutto simile, le performance dell’ERS-KNN aumentano fino ad una finestra di 8 secondi. Un trend opposto si può vedere nei risultati ottenuti dai classificatori deep learning, in questo caso le metriche mostrano un decremento monotono all’aumentare della finestra.

### Selezione delle feature

Oltre alla lunghezza della finestra di processing un altro parametro che influisce sulle prestazioni dei modelli di machine learning classici sono le feature che vengono calcolate. Per selezionare le feature utilizziamo un metodo detto di ***forward feature selection*** il quale sfrutta l’accuratezza come criterio per valutare l’impatto dell’aggiunta di nuove feature partendo da un insieme vuoto fino all’esaurimento. Ogni gruppo di feature Base(B), Hjorth(H) e Shape(S) è trattato come una sola unità che può essere aggiunta o rimossa. La Tabella mostra le performance per entrambi i classificatori al variare dei gruppi di feature selezionati. Le metriche mostrano tutte un aumento monotono che raggiunge le performance migliori nel momento in cui tutti i gruppi sono usati assieme; questo significa che ogni gruppo di feature aggiunge informazioni utili al processo di classificazione, inoltre, il gruppo Hjorth è quello che contiene le informazioni più utili essendo quello con i risultati migliori dei tre gruppi quando testati da soli.

## Esperimenti sull'HEXIWEAR

### Porting dei modelli su smartwatch

Vediamo nel dettaglio il porting dei modelli di machine learning sul dispositivo indossabile HEXIWEAR; come detto questo smartwatch ha al suo interno 1024KB di memoria Flash e 256KB di memoria RAM, il che, visto il trend dei moderni dispositivi mobile, non è una grande quantità di memoria, infatti sul mercato sono disponibili smartwatch che hanno a disposizione addirittura più di 1GB di memoria RAM, ma il loro costo è molto superiore, fanno uso di un ambiente di sviluppo proprietario più restrittivo ed hanno un consumo di energia tale che la batteria generalmente dura meno di una giornata.
Per via di queste limitazioni fisiche abbiamo dovuto modificare i modelli descritti in precedenza riducendo il numero di parametri da addestrare, ma, nonostante svariati tentativi, siamo stati comunque costretti a scartare i modelli di CNN e ERS-KNN lasciando solo SVM ed LSTM. Nel caso del CNN il fattore limitante è la memoria richiesta sia per contenere tutti i pesi dei parametri della rete che per salvare ed elaborare le 12 immagini GASF/GADF; nel caso dell’ERS-KNN invece, non siamo stati in grado di trovare nessuna libreria che eseguisse l’inferenza sfruttando la tecnica di Ensemble Random Subspace direttamente sui dispositivi embedded e consumando poche risorse.

Per eseguire i modelli sullo smartwatch sono state utilizzate due librerie: 

- TensorFlow 2.0 Lite Micro 
- libSVM

TensorFlow è una libreria open-source sviluppata da Google nel 2015 per facilitare le ricerche sul machine learning e le intelligenze artificiali; supporta vari tipi di tecnologie, ma si concentra particolarmente sull'addestramento e l’inferenza di reti neurali profonde(deep neural networks). In particolare, per l’inferenza sull'HEXIWEAR abbiamo utilizzato TensorFlow 2.0 Lite Micro, una versione di TensorFlow 2.0 pensata appositamente per l’ambito mobile ed i micro-controllori. 

LibSVM è una libreria di machine learning open-source sviluppata dalla National Taiwan University e scritta in C++ con l’obbiettivo di semplificare l’utilizzo dei modelli SVM da parte di sviluppatori non specializzati. LibSVM supporta sia la classificazione che la regressione offrendo varie funzionalità tra cui: efficiente classificazione multi-classe, cross- validation con k-fold ed estensioni per interagire con i più comuni linguaggi di programmazione.

In questa sezione discutiamo i risultati degli esperimenti condotti sui modelli proposti all’interno del dispositivo indossabile HEXIWEAR; a differenza degli esperimenti condotti su PC in questo caso viene utilizzato solo il dataset co- struito ad hoc. 

### Lunghezza della finestra di SVM

Di seguito esaminiamo la variazione delle performance del modello SVM al variare della lunghezza della finestra di processing; la Figura mostra un cambiamento generale delle performance molto ampio con dei picchi massimi per le finestre di lunghezza 12/14 secondi e dei picchi minimi per finestre di lunghezza 8/10/20 secondi. In particolare, notiamo che tutti i risultati si comportano pressoché allo stesso modo al variare della lunghezza della finestra, anche se l’accuratezza supera gli altri di sei punti percentuali, questo è sinonimo di una buona correlazione fra i valori.

Paragonate alle performance calcolate su PC nel caso del porting notiamo un generale peggioramento, infatti le metriche non superano mai il 90%, con il valore massimo dell’accuratezza al 88%. Questo fenomeno non ci sorprende poiché è sicuramente causato dalla riduzione di capacità che abbiamo dovuto apportare ai modelli durante la fase di porting.

### Selezione delle feature di SVM

Il secondo esperimento condotto sul classificatore SVM consiste nel variare le feature calcolate ed osservare il cambiamento delle performance che ne segue. Utilizziamo il metodo di ***forward feature selection*** per scegliere i gruppi di feature da valutare; nel porting sul dispositivo HEXIWEAR si è scelto di ridurre il numero di feature calcolate in modo da diminuire la memoria richiesta, di conseguenza le feature calcolate appartengono tutte al gruppo Base e sono:

- Media(A)
- Deviazione Standard(S)
- Massimo e Minimo(M)

Ogni esperimento è stato condotto utilizzando una finestra di processing di 12 secondi.
La Tabella mostra le performance al variare dei gruppi di feature calcolati; anche in questo caso le metriche presentano un aumento monotono raggiungendo le performance migliori quando tutti i gruppi sono usati assieme, poiché ogni feature aggiunge informazioni utili alla classificazione. A differenza delle altre metriche, la Recall ha un valore maggiore nel caso di **Media + Deviazione Standard**; infatti questi due gruppi sono quelli che producono i risultati migliori se si esclude la combinazione di tutti tre. Inoltre il gruppo che contiene le informazioni più utili è quello della Media, producendo i risultati migliori quando preso singolarmente.

### Selezione dei parametri di LSTM

L’ultimo esperimento che prendiamo in esame sull'HEXIWEAR riguarda le performance del modello di deep learning LSTM variando alcuni dei suoi parametri interni. Nel seguente esperimento abbiamo preso come parametri interni il numero di unità nel modulo LSTM ed il numero di neuroni nascosti all’interno del primo livello densamente connesso, mentre l’ultimo livello denso è stato settato costante pari al numero di etichette.

In Tabella vediamo le metriche calcolate al variare dei seguenti parametri: 

- 64/256
- 32/128
- 16/64
- 8/32

Come possiamo immaginare il modello che produce i risultati migliori è quello con i parametri 64/256 con un’Accuracy dell’87%, ed una Recall dell’88%.

### Utilizzo di memoria e tempistiche

Per valutare meglio le performance complessive dei modelli di machine learning eseguiti sullo smartwatch HEXIWEAR, durante ogni esperimento abbiamo misurato le tempistiche, il consumo di memoria e l’energia di ogni classificatore.

La Tabella mostra i modelli con i risultati migliori fra quelli visti negli esperimenti precedenti. I parametri tempo e memoria dei sensori rappresentano il tempo in millisecondi e la memoria in byte occupati durante la fase di raccolta dei dati non elaborati dai sensori di accelerometro e giroscopio; questi due valori dipendono fortemente dalla lunghezza della finestra di processing, per questo motivo sono uguali per i due modelli SVM, che utilizzano entrambi una finestra di 12 secondi, e sono minori per il modello LSTM, che invece fa uso di una finestra di 2 secondi.

Il tempo e la memoria occupata per il calcolo delle feature sono presenti solo nei modelli SVM e, come possiamo immaginare, sono minori nel caso in cui viene calcolato un numero minore di feature. Per quanto riguarda il tempo d’inferenza notiamo una piccola differenza fra i modelli SVM, mentre il modello LSTM impiega circa il doppio del tempo, invece la memoria impiegata per l’inferenza ha valori simili fra LSTM e l’esperimento SVM con la finestra ed è minore per l’SVM con meno feature. Infine vediamo che la potenza è maggiore per il modello LSTM ed è simile fra i due modelli SVM, con l’esperimento sulle feature che ha la potenza minore.

L’ultima analisi svolta consiste nel paragonare alcune delle metriche di tempo, memoria ed energia con il valore della loss per i modelli presi in esame; la funzione loss indica il costo in termini di prestazioni del modello ed è pari ad 1-Accuracy.
I grafici in Figura riportano sull'asse x il valore della loss e sull'asse y la memoria totale ed il consumo di energia paragonando il modello SVM dell’esperimento sulla lunghezza della finestra di processing con quello sul calcolo delle feature. Nei grafici memoria totale vs. loss vediamo che i classificatori migliori risultano essere quello con una finestra di lunghezza 2 e quello con le feature calcolate A+M, poiché sono i due ad avere un basso utilizzo di memoria mantenendo anche un valore per la loss basso; allo stesso modo nei grafici consumo d’energia vs. loss i migliori risultano essere quello con finestra di lunghezza 12 e quello con le feature calcolate A+S.

Analogamente i grafici in Figura confrontano il valore della loss con l’energia, il tempo d’inferenza ed il peso del modello LSTM nell’esperimento sulla variazione dei parametri interni. Come vediamo tutti e tre i grafici mostrano una repentina diminuzione dei valori all’aumentare della loss, ovvero quando vengono considerati modelli con un numero minore di parametri interni. In tutti e tre i casi il classificatore con le migliori performance rispetto alla loss risulta essere quello con 32 unità LSTM e 128 neuroni nel primo strato nascosto.

## Sviluppo dell’applicazione

In questa sezione presentiamo lo sviluppo di un’applicazione nativa per lo smartwatch HEXIWEAR. Dopo aver eseguito tutti gli esperimenti sui modelli di machine learning e di deep learning abbiamo deciso di sviluppare un’applicazione per l’HEXIWEAR che metta in pratica quanto scoperto in questa tesi ad un contesto comune e più utile per tutti gli utenti.

L’applicazione sviluppata ha lo scopo di conteggiare il numero di attività di lavaggio e di sanificazione della mani compiute da un utente nell’arco di una giornata; per fare questo raccoglie in maniera continua i valori non elaborati dai sensori di accelerometro e giroscopio ed utilizza uno dei modelli di machine learning visti precedentemente per classificare l’attività svolta.

L’applicazione nativa è stata sviluppata interamente in C/C++ facendo uso delle API messe a disposizione dal sistema operativo real-time MbedOS.
Nell’applicazione sono state utilizzate quattro librerie esterne: 

- hexiwear_FXAS21002
- hexiwear_FXOS8700
- hexiwear_KW40Z
- oled_ssd1351

a differenza delle altre librerie, create e mantenute dal team di sviluppo dell’HEXIWEAR, l'ultima è stata creata quasi da zero da noi partendo dalla versione ufficiale modificandola pesantemente per adeguarla alle nostre esigenze e risolvere alcuni fastidiosi bug.

In Figura possiamo vedere la schermata principale dell’applicazione in esecuzione direttamente sul dispositivo HEXIWEAR; il design è semplice, composto da due soli contatori: 

- uno per i lavaggi delle mani 
- uno per le sanificazioni

Premendo il pulsante Settings saremo portati nella pagina delle impostazioni, nella quale possiamo resettare i contatori a fine giornata.

Per il rilevamento delle attività l’applicazione fa uso del modello di deep learning LSTM con i parametri interni settati a 64 unità per il modulo LSTM e 256 neuroni per il primo livello densamente connesso. Si è scelto di utilizzare questa tipologia di rete poiché risulta essere la più prestante in termini delle metriche di accuratezza (tutte le metriche superano l’87%); inoltre, anche se il tempo d’inferenza e la memoria utilizzata sono maggiori rispetto ai modelli SVM, il consumo d’energia è molto inferiore che, in definitiva, questo parametro non è da trascurare quando si parla di applicazioni embedded o indossabili che hanno bisogno di una lunga durata della batteria.

## Sviluppi futuri

- approfondire la ricerca svolta in tutti gli ambiti, in particolare ampliando il dataset costruito ad hoc raccogliendo ancora più ore di dati da un maggior numero di soggetti di genere ed età diversi; in questo modo si va a creare un data- set pubblico di ottima qualità specifico per le misurazioni delle attività non strutturate di lavaggio e sanificazione delle mani, che, dalle nostre ricerche, non sembra ancora esistere
- la qualità dei modelli portati sullo smartwatch; a causa delle grandi limitazioni in termini di memoria siamo stati costretti a ridurre severamente la dimensione dei modelli eseguiti sull'HEXIWEAR. 
- analisi dei dispositivi più performanti in grado di eseguire gli stessi modelli studiati nell’ambiente PC.

Da parte di questo lavoro di tesi è stato prodotto un articolo scientifico pubblicato nel 2022 su un’importante rivista specializzata.