# GoSTEM Software per Sondes Estratofèriqeus 
Aquest repositori presenta els codis prinipals que permeten treballar amb els harware de GoSTEM. 

Existeixen dos codis per la ground station i per l'ordinador abord de la sonda.

1-**Onboard Computer**: Codi per treballar amb un T-Beam que registra la posició, temperatura, altura i preció. Les dades les guarde en una SD i alhora són enviades per LoRa a la ground station per poder fer un seguiment en temps directe. 

2-**Ground Station**: Receptor LoRa que rep les senyals enviades per la sonda i les guarda en un format CSV. Permet fer un seguiment a temps real. Requreix un ordinador per guardar les dades i poder-les mostrar. Es fa servrir un programa anomenat PuTTy el qual llegeix els missatges serial de la placa i els guarda directament en un CSV. Es molt intuitiu de fer servir i la configuració que requereix és molt similar a la del ardunio IDE (Selecció de Port Serial, Baudrate,.....)

Per fer funcionar correctament les plaques propcionades es important llegir la documetnació original del fabricant i seguit els passos que es detallen a continuació

https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series?spm=a2g0o.detail.1000023.1.3f7415bbH3saRE

Es requereix tenir Arduino IDE instal·lat i les llibreries que són necessaries per cada scritp.

Les plaques que es fan servir són: T-BEAM i LyliGo LoRa V1.0

Una delles està pensada per anar en el globus sonda i l'altre per rebre els missatges de telemetria a través de LoRa.

La següent imatge presenta la configuració clàssica de ordinador d'abord i el receptor terrestre. Es requereix fer ús d'una baterial de LiPo 18650 per l'ordinador d'abord i una targa SD de 8 GB. També es recomanable posar un punt de sil·licona a les soldadures per garantir que no es faran malbé en el moment de l'impacte. 

![Setup d'Exemple](https://github.com/GoSTEM-Education/GoSTEM_SONDA_ESTRATOSFERICA/blob/main/setup_demo.jpg?raw=true)


L'ordinador de bord un cop connectat al PC enviarà els següents missatges:

```
Data Logged in SD Card
LoRa Paquet Send Succesfully
No Fix - Num. satellites: (Minimum 5)0
Temperature = 20.39 °C
Pressure = 1006.85 hPa
Approx. Altitude = 103.14 m
```
Quant faci dues pampalluges de color blau seguides i de forma intermitant vol dir que està generant els missatges correctament, els està enviant i emagatzemant. Altres codis de llums inidquen un mal funcionament, es recomana fer un reset de la placa amb el boto dedicat o apagar-la i tornar-la a encendre amb el boto dedicat.

Les dades que es guarden a la SD de l'ordinador de bord tene el següent format. Les dades que s'envien per LoRA son imilars però reduides en tamany. 
```
TEMPERATURA,PRESSURE,ALTITUDE,LATITUDE,LONGITUDE,SPEED,COURSE,TIMERX
17.03,907.54,968.54,42.553662,1.325715,30.6,246.8,9:17:28,.559038,

```
Auesta es la frase que s'envia per LORA al receptor de terra i que serà guardat mitjançant PuTy. Mitjançant el cody en pyton de Plotte de GoSTEM es podrà graficar.

Per carregar el Ordinador de Bord seguir els següents passos:
1. Connectar per USB a un ordinador la PCB **Sempre carregar a 5V i 1A!** 
2. De forma automàtica aquesta s'encen i es veurà la llum vermella de Power encesa. Fent servir el boto de PWR aguantar-lo i apagar la placa.
3. No ha de quedar cap llum encesa, deixar connectat al ordinador apx 2  hores
4. Amb un mulímetre sense treure la bateria de la PCB i posant el punter del multimetre a cada un dels born exposats es pot veure com el voltage de la pila canvia. 
5. Descarregada completament: 2.5V - 3.0V // Carregada completament: 4.15V - 4.2 V

**IMPORTANT, WARNINGS!:**
```diff
- ES ESSENCIAL MAI ENCENDRE LA PLACA AMB LES ANTENES LORA I GPS DESCONECTADES. SI ES FA AIXÍ HI HA RISC DE CREMAR L'ELECTRONICA!!!
- Per alimentar la PCB i carregar-la sempre s'ha de fer a USB 5V/1A. Això es el que dona per defecte un ordinador. Es recomana sempre fer-ho des de un ordinador si es desconeix les característiques del transforamor extern en qüestió.
- Si s'està a interior i no hi ha GPS es normal que les dades de hora i posició siguin nul·les
- Són elements d'electrònica delicat i d'altes prestacions, es important tractar-los com a tals
- Es important llegir quina es la normativa de l'utilització de freqüències LoRa (https://avbentem.github.io/airtime-calculator/ttn/eu868)
- No treure la Bateria de la PCB
```
