#!/bin/bash
./racine_sleep files/serveursNomsR 8050& ./racine files/serveursNomsR 8051& ./racine files/serveursNomsR 8052&	./racine files/serveursUS 8090& ./racine files/serveursUS 8091& ./domaine files/serveursSapienceUS 8000& ./domaine files/serveursSapienceUS 8001& ./domaine files/serveursGoogleUS 8002& ./domaine files/serveursGoogleUS 8003& ./domaine files/serveursGoogleUS 8004 & ./domaine files/serveursAouniUS 8006&
./client files/serveursRacines files/requetes

pkill racine && pkill domaine && pkill racine_sleep&
