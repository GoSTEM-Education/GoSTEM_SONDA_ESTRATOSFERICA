# LoRa-GPS-Tracker
Aquest repositori presenta els codis prinipals que permeten treballar amb els harware de GoSTEM. 

Existeixen dos codis per la ground station i per l'ordinador abord de la sonda.

1-Onboard Computer: Codi per treballar amb un T-Beam que registra la posició, temperatura, altura i preció. Les dades les guarde en una SD i alhora són enviades per LoRa a la ground station per poder fer un seguiment en temps directe. 

2-Ground Station: Receptor LoRa que rep les senyals enviades per la sonda i les guarda en un format CSV. Permet fer un seguiment a temps real. Requreix un ordinador per guardar les dades i poder-les mostrar
