# CanINO

Il seguente README è ancoira in fase preliminare e sarà soggetto a modifiche o aggiunte

## Introduzione

Il progetto `CanINO` è un dosatore automatico per animali, progettato per fornire cibo e acqua agli animali domestici in modo automatico, seguendo quantità e orari prestabiliti. 

## Sensori ed attuatori utilizzati

`CanINO` utilizza i seguenti componenti:

- Arduino
- Potenziometro
- Display OLED I2C
- 2 Servo Motori
- Pompa acqua
- Cella di carico
- DS3231 RTC
- Pulsante

## Funzionalità

Le funzionalità offerte sono le seguenti:

1. Dosaggio del Cibo: Il dosatore può erogare una quantità specifica di cibo, regolata tramite il potenziometro,
2. Programmazione dell'orario: È possibile impostare l'ora in cui erogare il cibo. Il dosatore erogherà automaticamente il cibo alla stessa ora ogni giorno,
3. Riempimento dell'Acqua: La pompa dell'acqua riempirà la ciotola dell'acqua alla pressione dell'apposito tasto,
4. Display delle Informazioni: Il display OLED permette di visualizzare la quantità di cibo (g) impostata e l'orario dell'erogazione,
5. Misurazione del Peso: La cella di carico misura il peso del cibo, disattivando i servo motori quando  il peso raggiunto equivale a quello impostato.

## Schema dei Collegamenti

Segue uno schema dei collegamenti tra i componenti utilizzati nel progetto:

```
//TODO 
```

## Funzionamento

Il progetto "CanINO" funziona nel seguente modo:

1. L'utente regola la quantità di cibo desiderata tramite il potenziometro.
2. L'utente imposta l'ora desiderata per l'erogazione del cibo tramite il pulsante e il potenziometro.
3. Quando arriva l'ora impostata, il dosatore attiva i servo motori per erogare la quantità di cibo impostata.
4. La cella di carico misura il peso del cibo nel contenitore e disattiva i servo motori al raggiungimento del peso
   
   
   
   
