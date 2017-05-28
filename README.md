Funkcjonalność urządzenia:
1. Pomiar stężenia pyłu zawieszonego PM2.5 i PM10 w powietrzu przy pomocy czujnika PMS5003, biblioteka do obsługi czujnika zostanie napisana od podstaw.
2. Pomiar temperatury i ciśnienia przy pomocy czujnika BMP180. Biblioteka do obsługi czujnika została już napisana i zostanie przeniesiona z frameworku HAL dla STM32.
3. Pomiar temperatury i wilgotności przy pomocy czujnika DHT22. Wykorzystana zostanie gotowa biblioteka dla Arduino - https://github.com/adafruit/DHT-sensor-library.
4. Komunikacja przez Bluetooth z profilem emulacji COM-a, przy pomocy SDK dla płytki nRF51822.
5. Zasilanie z powerbanku przez port USB.

Instrukcja instalacji środowiska:
1. Należy zainstalować najnowsze Arduino IDE oraz doinstalować do niego obsługę płytki nRF51822, zgodnie z instrukcją: https://github.com/RedBearLab/nRF51822-Arduino.
2. Następnie, należy zaimportować projekt i z menu w górnej części okna, wybrać opcje: Narzędzia -> Płytka -> RBL nRF51822 (V1.0 16KB).
3. Teraz można już skompilować i wgrać oprogramowanie na płytkę.