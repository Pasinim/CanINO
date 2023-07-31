# CanINO

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

## Funzionalità


Le funzionalità offerte sono le seguenti:

1. Dosaggio del Cibo: Il dosatore può erogare una quantità specifica di cibo, regolata tramite il potenziometro, nel piatto del cane o del gatto.
2. Programmazione dell'Orario: È possibile impostare l'ora in cui erogare il cibo. Il dosatore erogherà automaticamente il cibo alla stessa ora ogni giorno.
3. Riempimento dell'Acqua: La pompa dell'acqua automaticamente riempirà la ciotola dell'acqua potabile ad intervalli regolari.
4. Display delle Informazioni: Il display OLED visualizza l'ora impostata, la quantità di cibo dosata e altre informazioni utili.
5. Misurazione del Peso: La cella di carico misura il peso del cibo nel contenitore e visualizza informazioni sul display.

## Schema dei Collegamenti

Segue uno schema dei collegamenti tra i componenti utilizzati nel progetto:

```
//TODO 
```

## Funzionamento

Il progetto "CanINO" funziona nel seguente modo:

1. L'utente regola la quantità di cibo desiderata tramite il potenziometro.
2. L'utente imposta l'ora desiderata per l'erogazione del cibo.
3. Il dosatore monitora costantemente l'ora dal modulo DS3231 RTC.
4. Quando arriva l'ora impostata, il dosatore attiva i servo motori per erogare la quantità di cibo impostata nel piatto del cane o del gatto.
5. La cella di carico misura il peso del cibo nel contenitore e aggiorna le informazioni sul display.
6. La pompa dell'acqua riempie automaticamente la ciotola dell'acqua potabile ad intervalli regolari.

