Volevo evidenziare alcuni elementi che potrebbero aiutare con la correzione:
- il menu permette di modificare i parametri e si dovrà poi scegliere se confermarli o se ripristinare gli originali uscendo dalla schermata

- il menù permette di selezionare quale tipologia di AI utilizzare per la partita ed è già presente
  il metodo che permetta effettivamente di cambiarla a run time. Purtroppo non sono riuscito a realizzare
  quella intelligente, quindi qualsiasi sia la selezione verrà sempre selezionata quella randomica.

- teoricamente si dovrebbe poter aumentare o ridurre il numero di minifigure spawnabili tramite modifiche di blueprint e
  dovrebbe quindi essere possibile svolgere partite con n minifigure (sempre delle due sole tipologie, non essendo implemetato un metodo di spawn
  per altre tipologie di classi). Non ho implementato un meccanismo di controllo se il numero di minifigure sia impostato a 0 (dovrebbe crashare e dare errore)
  e non ho neanche implementato un metodo di controllo sul numero massimo in relazione al numero di caselle libere (impossibile giocare, ci si trova in uno stallo,
  senza mosse possibili da eseguire)
  --> non ci dovrebbero essere problemi di svolgimento di una partita con n minifigure per parte: è possibile anche che i giocatori abbiano un numero
  differente di minifigure da utilizzare; le condizioni di vittoria dovrebbero essere definite in maniera tale da non creare problemi.

- è possibile che Unreal segnali un problema di riferimento a valori non presenti in codice una volta che si chiude il gioco (errore in rosso). Il problema è dato
  dal fatto che il riferimento dell'HUD sulla minifigura cliccata, quando questa viene eliminata, risulta puntare a valori che non esistono più (vengono cancellati
  in seguito alla onDestroy()). Sono riuscito tramite una funzione su blueprint a fare in modo che questo errore compaia solo per il tempo compreso tra
  la distruzione della minifigura e il tick successivo (inizialmente rimaneva tipo loop facendo crashare); non sono riuscito però a evitare che comparisse, essendo l'HUD
  collegata allo Human Player e la distruzione delle minifigure legata alla minifigura stessa (e allo stesso tempo al player che la possedeva), non al gestore dell'HUD quindi.

- all'interno del codice ci sono parecchie parti che permetterebbero teoricamente una estensione del gioco, con aggiunta di classi, tipologie di attacco, aggiunta di texture, ...
  ma allo stesso tempo ci sono alcune parti che si basano sul fatto che stiamo lavorando solo con due giocatori; in alcuni casi ho provato ad estenderle ma i limiti di tempo
  e la complessità in alcuni casi me lo hanno impedito.

- non ho creato un pulsante per poter tornare al menù una volta avviato il gioco, prevalentemente a causa di come ho gestito lo spawn delle minifigure e di come ho gestito lo stato
  dei player: mentre la random AI sarebbe ancora rimpiazzabile (cancello la vecchia e ne creo una nuova) e lo stesso lo potrei fare a livello di minifigure e di campo di gioco (tiles + ostacoli),
  non sono riuscito a trovare un modo per resettare in maniera efficiente lo Human Player, cosa che mi impediva di ripristinare il gioco da 0.

- sono riuscito a stampare a schermo tutte le mosse che vengono effettuate anche so non sono riuscito a inserirli in una Vertical Box in una scroll Box, per un problema con l'interazione tra 
  la insert Child View per la VerticalBox che, nonostante riuscissi a chiamare la funzione su blueprint da C++, non permetteva l'inserimento di una nuova entrata (creazione di una nuova view).
  


Non mi sembra ci sia altro, se mi ricorderò poi di qualche altra cosa, la aggiungerò in futuro
