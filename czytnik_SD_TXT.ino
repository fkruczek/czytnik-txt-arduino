//Autor: Fryderyk Kruczek
//biblioteki do obslugi modulu SD
#include <SPI.h>
/*
 * Początkowo używałem bibliotei SDFat.h gdyż była polecana przez użytkowników 
 * (podobno SD.h przy niektórych konfiguracjach może nie działać poprawnie)
 * Następnie zrezygnowałem z SDFat.h na rzecz SD.h ze względu na brak zaktualizowanej dokumentacji
 * Początkowo występowały problemy z odczytaniem karty, lecz po wgraniu jednej z poprzednich wersji biblioteki
 * program zaczął działać poprawnie.
 */
#include <SD.h>

//biblioteki do obslugi LCD z konwerterem I2C
#include<LiquidCrystal_I2C.h>
#include <Wire.h>

//biblioteka ktora pomoze przy formatowaniu tekstu
#include <ctype.h>

#define BACKLIGHT_PIN 3
#define ZATWIERDZ  2 
#define DALEJ 3

// Ustawienie adresu ukladu na 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); 


File root;

void setup() {
  //konfiguracja pinów przycisków (ustawienie na INPUT_PULLUP)
  //cały czas stan wysoki, a gdy wcisniemy przycisk to stan niski
  pinMode(ZATWIERDZ, INPUT_PULLUP);
  pinMode(DALEJ, INPUT_PULLUP);
  
  //konfiguracja ekranu
  lcd.begin(16,2);  
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();
  
  //otwieramy port i ustalamy predkosc transmisji
  //dzieki temu bedziemy kontrolowac dzialanie programu
  Serial.begin(9600);
  while (!Serial) {
    ; // czykamy na serial port
  }

  Serial.print("inicjalizowanie karty SD");

  if (!SD.begin(4)) {
    Serial.println("blad inicjalizacji");
    while (1);
  }
  
  Serial.println("karta SD zainicjalizowana");

  //ustawiamy katalog w ktorym bedziemy przegladac pliki
  root = SD.open("/");

  //funkcja inicjalizujaca menu glowne
  menuGlowne(root);
}

void loop() {
}

void menuGlowne(File dir) {
  while (true) {
    lcd.clear();
    lcd.print("Wybierz plik:");
    File obecny =  dir.openNextFile();

    //jezeli koniec katalogu
    if (! obecny) {
      dir.rewindDirectory();
      continue;
    }

    //jezeli obecny to katalog
    if (obecny.isDirectory()) continue;
     
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print(">");
    lcd.print(obecny.name());
    lcd.print(" ");
    lcd.print(obecny.size());

    //opoznienie wymagane do poprawnego działania przyciskow
    delay(300); 
    while(true){
      //przycisk akceptacji 
      if(digitalRead(ZATWIERDZ) == LOW){
        czytajPlik(obecny, obecny.size());
        delay(100);
        break;
      }
      //przycisk przewijania
      if(digitalRead(DALEJ) == LOW){
        delay(100);
        break;
      }
    }
 
    obecny.close();
    }
 }

 void czytajPlik(File plik, int dlugosc){
   char pierwsza[16] = "               ";
   char druga[16] = "               ";
   char znak, pomocniczy;

   //zapisanie pierwszej linijki do wyswietlenia
    for(int i=0; i<16 && plik.available();)
     {
      znak = plik.read();
      //skorzystanie z biblioteki ctype
      if(!(isprint(znak))) znak = ' ';
      pierwsza[i++] = znak;
     }
     
   do{
     //zapisanie drugiej linijki do wyswietlenia
     for(int i=0; i<16 && plik.available();)
     {
      znak = plik.read();
 
      //obsluga dzielenia wyrazow
      if(i == 0 && isalpha(znak) && isalpha(pierwsza[15])){
        pomocniczy = pierwsza[15];
        pierwsza[15] = '-';
        druga[i++] = pomocniczy;  
      }
      if(!(isprint(znak))) znak = ' ';
      druga[i++] = znak;
     }

     lcd.clear();
     lcd.print(pierwsza);
     lcd.setCursor(0,1);
     lcd.print(druga);
     
     delay(300);
     while(true){
      //przycisk akceptacji = wyjscie z pliku
      if(digitalRead(ZATWIERDZ) == LOW){
        delay(100);
        return;
      }
      //przycisk przewijania
      if(digitalRead(DALEJ) == LOW){
        delay(100);
        break;
      }
    }

     strcpy(pierwsza, druga);
     strcpy(druga, "               ");
   } while(plik.available());
   
 }
