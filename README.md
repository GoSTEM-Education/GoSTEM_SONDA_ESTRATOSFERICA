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

Data Logged in SD Card
LoRa Paquet Send Succesfully
No Fix - Num. satellites: (Minimum 5)0
Temperature = 20.39 °C
Pressure = 1006.85 hPa
Approx. Altitude = 103.14 m



IMPORTANT, WARNINGS!:
ES ESSENCIAL MAI ENCENDRE LA PLACA AMB LES ANTENES LORA I GPS DESCONECTADES. SI ES FA AIXÍ HI HA RISC DE CREMAR L'ELECTRONICA!!!
