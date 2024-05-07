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
5. Misurazione del Peso: La cella di carico misura il peso del cibo, disattivando i servo motori quando la quantità erogata corrisponde a quella impostata.

## Schema dei Collegamenti

Segue uno schema dei collegamenti tra i componenti utilizzati nel progetto:

```
//TODO
```

## Funzionamento

Il progetto "CanINO" funziona nel seguente modo:

1. L'utente sceglie se avviare la modalità "Setup" o "Idle" tramite un interruttore

2. 1 _Modalità Setup_

   - L'utente regola la quantità di cibo desiderata tramite il potenziometro.

   - Premento il pulsante l'utente passa all'impostazione dell'ora regolabile tramite il potenziometro.

3. _Modalità Idle_

   - Quando l'orario corrente corrisponde con l'orario di erogazione impostato il dosatore attiva i servo motori per erogare il cibo,

   - La cella di carico miusura il peso del cibo e disattiva i servo motori al raggiungimento del peso

4. Quando arriva l'ora impostata, il dosatore attiva i servo motori per erogare la quantità di cibo impostata.

5. La cella di carico misura il peso del cibo nel contenitore e disattiva i servo motori al raggiungimento del peso

---

## Calibrazione cella di carico

Il HX711 è un convertitore analogico-digitale (ADC) con una risoluzione fissa di 24 bit e prima di utilizzarla per misurare il peso è necessario effettuare una calibrazione per ottenere letture accurate e coerenti dalla cella di carico collegata.

La calibrazione è stata eseguita con il seguente [codice ]
(https://github.com/RobTillaart/HX711), i valori ottenuti sono stati salvati nelle variabili `offset_hx711` e `calibration_factor`.

```c
void calibrate()
{
  Serial.println("\n\nCALIBRATION\n===========");
  Serial.println("remove all weight from the loadcell");
  //  flush Serial input
  while (Serial.available()) Serial.read();

  Serial.println("and press enter\n");
  while (Serial.available() == 0);

  Serial.println("Determine zero weight offset");
  myScale.tare(20);  // average 20 measurements.
  uint32_t offset = myScale.get_offset();

  Serial.print("OFFSET: ");
  Serial.println(offset);
  Serial.println();


  Serial.println("place a weight on the loadcell");
  //  flush Serial input
  while (Serial.available()) Serial.read();

  Serial.println("enter the weight in (whole) grams and press enter");
  uint32_t weight = 0;
  while (Serial.peek() != '\n')
  {
    if (Serial.available())
    {
      char ch = Serial.read();
      if (isdigit(ch))
      {
        weight *= 10;
        weight = weight + (ch - '0');
      }
    }
  }
  Serial.print("WEIGHT: ");
  Serial.println(weight);
  myScale.calibrate_scale(weight, 20);
  float scale = myScale.get_scale();

  Serial.print("SCALE:  ");
  Serial.println(scale, 6);

  Serial.print("\nuse scale.set_offset(");
  Serial.print(offset);
  Serial.print("); and scale.set_scale(");
  Serial.print(scale, 6);
  Serial.print(");\n");
  Serial.println("in the setup of your project");

  Serial.println("\n\n");
}
```

 per la calibrazione della cella di carico.
