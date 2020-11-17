#!/bin/bash
./racine_sleep files/serveursNomsR 8050& ./racine_sleep files/serveursNomsR 8051& ./racine_sleep files/serveursUS 8090& ./racine_sleep files/serveursUS 8091& ./racine files/serveursMA 8060& ./racine files/serveursMA 8061& ./domaine files/serveursSapienceUS 8000& ./domaine_sleep files/serveursSapienceUS 8001& ./domaine files/serveursAouniMA 8063& ./domaine files/serveursAouniMA 8064&
./client files/serveursRacines files/tourniquet
pkill racine && pkill domaine&
