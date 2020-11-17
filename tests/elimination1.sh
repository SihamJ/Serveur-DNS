#!/bin/bash
./racine files/serveursNomsR 8050& ./racine files/serveursNomsR 8051& ./racine files/serveursUS 8090& ./racine files/serveursUS 8091& ./racine files/serveursMA 8060& ./racine files/serveursMA 8061& ./domaine files/serveursSapienceUS 8000& ./domaine_sleep files/serveursSapienceUS 8001& ./domaine files/serveursAouniMA 8063& ./domaine_sleep files/serveursAouniMA 8064&
./client files/serveursRacines files/tourniquet
pkill racine && pkill domaine&
