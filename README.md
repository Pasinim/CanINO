# CanINO

## Introduzione

Il progetto `CanINO` è un dosatore automatico per animali, progettato per fornire cibo agli animali domestici in modo automatico, seguendo quantità e orari prestabiliti.

## Sensori ed attuatori utilizzati

| Sensore/Attuatore | Funzione                                                                                                                |
| ----------------- | ----------------------------------------------------------------------------------------------------------------------- |
| Arduino           | Microcontrollore                                                                                                        |
| Potenziometro     | Permette il controllo dell'orario di erogazione e della quantità di cibo erogabile (in modalità setup)                                                  |
| Display OLED I2C  | Permette la visualizzazione dell'orario corrente, dell'orario di erogazione e della quantità dì cibo che verrà erogata. |
| 28BYJ-48          | Stepper motor che permette l'erogazione del cibo                                                                        |
| ULN2003           | Driver per il controllo dello stepper motor                                                                             |
| Cella di carico   | Produce un segnale di uscita in base alla deformazione a cui è soggetto, viene utilizzato per pesare il cibo erogato    |
| HX711             | Convertirore analogico-digitale che converte l'output della cella di carico in grammi                                   |
| DS3231            | RTC che mantiene l'orario impostato in assenza di alimentazione alla board                                              |
| Pulsante          | Utilizzato per cambiare tra la modalità di configurazione tra orario e quantità di cibo e viceversa                     |
| Switch            | Utilizzato per entrare in modalità setup                                                                                |

## Funzionalità

Le funzionalità offerte sono le seguenti.

1. Dosaggio del cibo: il dosatore può erogare una quantità specifica di cibo, regolata tramite il potenziometro,
2. Programmazione dell'orario: è possibile impostare l'ora in cui erogare il cibo. Il dosatore erogherà automaticamente il cibo alla stessa ora ogni giorno,
3. Display delle Informazioni: il display OLED permette di visualizzare la quantità di cibo (g) impostata e l'orario dell'erogazione,
4. Misurazione del peso: la cella di carico misura il peso del cibo, disattivando l'erogatore quando la quantità erogata corrisponde a quella impostata.
5. Mantenimento dell'ora corretta (dopo averla impostata correttamente la prima volta) anche in seguito allo spegnimento della board, grazie al modulo DS3231 che è alimentato da una pila.

## Macchina a stati finiti

![](https://github.com/Pasinim/CanINO/blob/main/images/fsm.drawio.png)

## Funzionamento

Il progetto "CanINO" funziona nel seguente modo:

1. L'utente sceglie se avviare la modalità "Setup" o "Idle" tramite un interruttore;

2. _Modalità Setup_:
   
   - Il pulsante permette di passare dalla regolazione dell'orario alla regolazione della quantità di cibo da erogare e viceversa
   
   - La quantità di cibo e l'orario di ergoazione sono impostabili tramite il potenziomentro
   
   - La quantità di cibo erogabile va da un limite inferiore definita nel codice tramite la costante `lower` mentre quella massima è definita dalla costante `upper`.

3. _Modalità Idle_:
   
   -  Quando arriva l'ora impostata, il motore del dosatore si attiva per erogare la quantità di cibo impostata;

   -  La cella di carico misura il peso del cibo nel contenitore e disattiva il motore dell'erogatore al raggiungimento del peso.

## Schema dei Collegamenti

![](https://github.com/Pasinim/CanINO/blob/main/images/CanINO_bb.png)


---

## Problemi riscontrati

- Uno dei principali problemi incontrati riguarda il processo di erogazione del cibo. Inizialmente, il progetto prevedeva l'utilizzo di un imbuto sigillato con un pezzo di ferro, mentre l'erogazione era regolata tramite dei servo motori collegati con degli elastici all'imbuto erogatore e al pezzo di metallo. Per erogare il cibo, era necessario azionare i servo motori per aprire il pezzo di metallo come una sorta di botola.

Con questa implementazione però non era possibile controllare con precisione il cibo erogato quindi ho stampato un dosatore in 3D che permette l'erogazione in modo più uniforme e preciso. Anche in questo modo però ho riscontrato dei problemi, infatti sia con del riso sia con delle crocchette per cani il cibo si incastrava nel dosatore fermando il motore. Per questo motivo ho utilizzato delle sfere di metallo. Ovviamente non si tratta di una soluzione ideale, soprattutto perchè le sfere si incastrano comunque nel cilindo superiore dell'erogatore. Per ovviare a questi problemi bisognerebbe modellare l'erogatore in un altro modo.

- Durante l'erogazione del cibo l'orario visualizzato sul display non viene aggiornato ma rimane fermo (al secondo precedente) all'orario di erogazione. Si tratta di un problema di poco conto, anche se bisogna considerare che l'erogazione del cibo non avviene in modo rapido, impiegado anche minuti se la quantità di cibo è elevata. Per risolvere questo problema si potrebbe ricorrere ad un motore con voltaggio superiore per eseguire più rotazioni al minuto oppure aggiornando la funzione di erogazione inserendo al suo interno il refresh del display.

---

## Codice rilevante

#### Impostazione dell'orario corrente

Il modulo RS3231 permette il mantenimento dell'orario anche se la board non è alimentata. Tuttavia è necessaria la configurazione se il modulo non è in esecuzione e ciò viene fatto con il seguente codice.

```c
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
```

#### Calibrazione cella di carico

Il HX711 è un convertitore analogico-digitale (ADC) con una risoluzione fissa di 24 bit e prima di utilizzarla per misurare il peso è necessario effettuare una calibrazione per ottenere letture accurate e coerenti dalla cella di carico collegata.

La calibrazione è stata eseguita con il seguente [codice ]
(https://github.com/RobTillaart/HX711), i valori ottenuti sono stati salvati nelle variabili `offset_hx711` e `calibration_factor`.
