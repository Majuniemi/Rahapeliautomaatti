
#include <TM1637Display.h>
#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  //määritetään lcd-niminen lcd-näyttö ja portit

#define CLK 7   // SCL-pinni
#define DIO 13  // SDA-pinni

TM1637Display display(CLK, DIO);

void Oikein();
void Kosautus();
//bool nappiPainettu(); //esitelläänkö muutkin aliohjelmat vai eikö esitellä yhtään?****************


//määrittää tuplaus ja musiikkipankin muuttujat
// Melodian nuotit:
int melodia[] = {

  175,
  262,
  247,
  262,
  294,
  262,
  247,
  262,
  262,
  262,
  247,
  233,
  233,
  220,
  196,
  175,
  262,
  247,
  262,
  294,
  262,
  247,
  262,
  233,
  233,
  247,
  262,
  233,
  220,
  196,

};

// Melodian kesto
int melodianKesto[] = {

  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  4,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  4,
  8,
  8,
  8,

};

int oikeaValinta[] = {
  // Oikea vastaus

  175,
  220,
  262,
  698,

};

int oikeanKesto[] = {

  16,
  16,
  16,
  4,

};

int vaaraValinta[] = {
  // Väärä vastaus

  262,
  247,
  233,
  220,

};

int vaaranKesto[] = {

  8,
  8,
  8,
  2,

};

//määritetään globaalit muuttujat
int vasenlukittu;
int keskilukittu;
int oikealukittu;
int kierros;                //tarkistaa rullan päivityskierroksen (80 kertaa luupissa)
int luuppi;                 //tarkistaa täyden rullan kierroksen tarpeeton?
int vasenkohta = 0;         //tarkistaa arvotun rullan kohdan, kohdat alustetaan tässä
int keskikohta = 0;         //tarkistaa arvotun rullan kohdan
int oikeakohta = 0;         //tarkistaa arvotun rullan kohdan
int tuplausrullakohta = 0;  //tarkistaa arvotun rullan kohdan
int kaynnista;
int kruunavalinta;
int klaavavalinta;
int tuplaus;  // kertoo onko tuplaus onnistunut vai ei
int voittorivi;
int rahat = 150;     //rahasumma jota seurataan kellonäytöllä
int voitettuSumma;  //voidaan seurata tuplauksissa, mutta ei lisätä rahoihin ennen tuplausohjelman päättämistä


//nappia varten muuttujat
int D = 14, C = 15, B = 16, A = 17;  //määritetään portit;
int nappi = 18;                      //muuttaa suoraan nappiin kuten muissakin?
//int select = 1; nykyään turha?// bool numberPrinted = false; nykyään turha?
unsigned long lastDebounceTime = 50;  //tallentaa millisekunteina viimeisimmän napin painamisen ajankohdan
unsigned long debounceDelay = 500;    // määrittää kuinka kauan ohjelma odottaa, että nappi tasaantuu ennen kuin se havaitsee napin painamisen.
//* tuplaus aliohjelman muuttujat päättyy

//lukitus muuttujat
int voitto = 0;
int lukittiinko = 0;
int edellinenpanos = 0;
int panos = 1;  //alustetaan panos, joka näkyy 7-segment näytöllä
//lukitusmuuttujat päättyy

byte normirullavasen[79];  //luodaan vasemman puoleinen rulla
byte normirullakeski[79];  //luodaan keskimmäinen rulla
byte normirullaoikea[79];  //luodaan oikean puoleinen rulla
byte tuplausrulla[79];     //luodaan tuplausrulla

byte kruuna[]  //asetetaan kruuna symboli
  {
    //Kruuna
    B00000,
    B10101,
    B10101,
    B11111,
    B10001,
    B10001,
    B11111,
    B00000,
  };

byte klaava[]  //asetetaan klaava symboli
  {
    //Klaava
    B00000,
    B00110,
    B01110,
    B00110,
    B00110,
    B01111,
    B01111,
    B00000,
  };

byte lukko[]  //asetetaan lukitus symboli
  {
    //Lukko
    B00000,
    B01110,
    B10001,
    B11111,
    B11011,
    B11111,
    B11111,
    B00000,
  };

byte sydan[]  //asetetaan Sydänsymboli
  {
    //sydän
    B00000,
    B01010,
    B11111,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
  };

byte yksirivi[]  //asetetaan rivisymboli
  {
    //1 rivi
    B00000,
    B00000,
    B11111,
    B10001,
    B11111,
    B00000,
    B00000,
    B00000,

  };

byte risuaita[]  //asetetaan risuaitasymboli
  {
    //#
    B00000,
    B00000,
    B01010,
    B11111,
    B01010,
    B11111,
    B01010,
    B00000,
  };

byte pata[]  //asetetaan patasymboli
  {
    //pata
    B00000,
    B00100,
    B01110,
    B11111,
    B11111,
    B00100,
    B01110,
    B00000,

  };

byte seiska[]  //asetetaan seiskasymboli
  {
    //7
    B00000,
    B11111,
    B10001,
    B11101,
    B01010,
    B10100,
    B11100,
    B00000,
  };

//Aliohjelmat alkaa************

void symbolipankki()  //haetaan puhtaat rullat
{
  byte normirullavasenresetointi[] =  //asetetaan symbolit yleisrullalle palautumispiste
    {
      //sydän
      B00000,
      B01010,
      B11111,
      B11111,
      B11111,
      B01110,
      B00100,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //sydän
      B00000,
      B01010,
      B11111,
      B11111,
      B11111,
      B01110,
      B00100,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,

      //pata
      B00000,
      B00100,
      B01110,
      B11111,
      B11111,
      B00100,
      B01110,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //7
      B00000,
      B11111,
      B10001,
      B11101,
      B01010,
      B10100,
      B11100,
      B00000,

      //pata
      B00000,
      B00100,
      B01110,
      B11111,
      B11111,
      B00100,
      B01110,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,


      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      /*/ruutu  ylimääräisiä merkkejä, mikäli niitä halutaan vaihtaa
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,//

  //2 riviä
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,//

  //risti
  B00000,
  B01110,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000, //

  //3 riviä
  B00000,
  B00000,
  B11111,
  B00000,
  B11111,
  B00000,
  B11111,
  B00000,/*/

    };
  byte normirullakeskiresetointi[] =  //asetetaan palautumispiste
    {
      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //pata
      B00000,
      B00100,
      B01110,
      B11111,
      B11111,
      B00100,
      B01110,
      B00000,

      //7
      B00000,
      B11111,
      B10001,
      B11101,
      B01010,
      B10100,
      B11100,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,
    };
  byte normirullaoikearesetointi[] =  //asetetaan symbolille palautumispiste
    {

      //sydän
      B00000,
      B01010,
      B11111,
      B11111,
      B11111,
      B01110,
      B00100,
      B00000,

      //pata
      B00000,
      B00100,
      B01110,
      B11111,
      B11111,
      B00100,
      B01110,
      B00000,

      //naama
      B00000,
      B11011,
      B11011,
      B00000,
      B10001,
      B01110,
      B00000,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,


      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

      //7
      B00000,
      B11111,
      B10001,
      B11101,
      B01010,
      B10100,
      B11100,
      B00000,

      //pata
      B00000,
      B00100,
      B01110,
      B11111,
      B11111,
      B00100,
      B01110,
      B00000,

      //#
      B00000,
      B00000,
      B01010,
      B11111,
      B01010,
      B11111,
      B01010,
      B00000,

      //1 rivi
      B00000,
      B00000,
      B11111,
      B10001,
      B11111,
      B00000,
      B00000,
      B00000,

    };

  byte tuplausrullaresetointi[] =  //asetetaan rullille palautumispiste
    {
      //Kruuna
      B00000,
      B10101,
      B10101,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,

      //Klaava
      B00000,
      B00110,
      B01110,
      B00110,
      B00110,
      B01111,
      B01111,
      B00000,

      //Kruuna
      B00000,
      B10101,
      B10101,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,

      //Klaava
      B00000,
      B00110,
      B01110,
      B00110,
      B00110,
      B01111,
      B01111,
      B00000,

      //Kruuna
      B00000,
      B10101,
      B10101,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,

      //Klaava
      B00000,
      B00110,
      B01110,
      B00110,
      B00110,
      B01111,
      B01111,
      B00000,

      //Kruuna
      B00000,
      B10101,
      B10101,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,

      //Klaava
      B00000,
      B00110,
      B01110,
      B00110,
      B00110,
      B01111,
      B01111,
      B00000,

      //Kruuna
      B00000,
      B10101,
      B10101,
      B11111,
      B10001,
      B10001,
      B11111,
      B00000,

      //Klaava
      B00000,
      B00110,
      B01110,
      B00110,
      B00110,
      B01111,
      B01111,
      B00000,

    };

  for (int i = 0; i < 80; i++) {  //jos resetoidaan vain aina og asentoon
    if (vasenlukittu == 0) {
      normirullavasen[i] = normirullavasenresetointi[i];
    }
    if (keskilukittu == 0) {
      normirullakeski[i] = normirullakeskiresetointi[i];
    }
    if (vasenlukittu == 0) {
      normirullaoikea[i] = normirullaoikearesetointi[i];
    }
    tuplausrulla[i] = tuplausrullaresetointi[i];
  }

  /* tätä käytettiin hakemaan alkukohta; tarpeeton tällä hetkellä; kun arvontarullat aloitetaan aina alusta
for (int i = vasenkohta*8; i < vasenkohta + 80; i++) {
    normirullavasen[i-vasenkohta*8] = normirullavasenresetointi[i % 79];
}
for (int i = keskikohta*8; i < keskikohta + 80; i++) {
    normirullakeski[i-keskikohta*8] = normirullakeskiresetointi[i % 79];
}
for (int i = oikeakohta*8; i < oikeakohta + 80; i++) {
    normirullaoikea[i-oikeakohta*8] = normirullaoikearesetointi[i % 79];
}
for (int i = tuplausrullakohta*8; i < tuplausrullakohta + 79; i++) {
    tuplausrulla[i-tuplausrullakohta*8] = tuplausrullaresetointi[i % 79];
}*/
  //jos halutaan että pyörintä jatkuu samasta, pitää sitten voittorullien kohtia muuttaa
}

void setChar(byte rivit[], byte rivit2[], byte rivit3[]) {  //luodaan rullasta uniikki merkki jonka näyttö piirtää
  if (vasenlukittu == 0) {
    lcd.createChar(0, rivit);
  }
  if (keskilukittu == 0) {
    lcd.createChar(1, rivit2);
  }
  if (oikealukittu == 0) {
    lcd.createChar(2, rivit3);
  }
}


  int symboliarvonta(int rulla, int vasenlukittu, int keskilukittu, int oikealukittu) {

  //aliohjelman muuttujat
  byte firstChar;
  byte secondChar;
  byte thirdChar;
  int vasenvalmis = 0;
  int keskivalmis = 0;
  int oikeavalmis = 0;  //testaus toimiiko paremmin
  int kierroksetvasen;  //tarviiko enää jos kiertää vain kerran
  int vasensymboli;
  int kierroksetkeski;  //tarviiko enää jos kiertää vain kerran
  int keskisymboli;
  int kierroksetoikea;  //tarviiko enää jos kiertää vain kerran
  int oikeasymboli;
  int rullankohta;
  int melody[] = {
    110
  };

  int noteDurations[] = {
    1
  };
  voitto = 0;
  //arvonnat
  if (vasenlukittu == 0) {
    //kierroksetvasen = random(1,3); // tarviiko enää
    vasensymboli = random(5, 7);
    vasenkohta = vasensymboli - 1;
  } else {
    vasensymboli = vasenkohta + 1;
    vasenvalmis = 1;  // testi
  }
  if (keskilukittu == 0) {
    //kierroksetkeski = random(1,3); // tarviiko enää
    keskisymboli = random(4, 6);
    keskikohta = keskisymboli - 1;
    tuplausrullakohta = keskikohta;
  } else {
    keskisymboli = keskikohta + 1;
    keskivalmis = 1;  // testi
  }
  if (oikealukittu == 0) {
    //kierroksetoikea = random(1,3); // tarviiko enää
    oikeasymboli = random(5, 7);
    oikeakohta = oikeasymboli - 1;
  } else {
    oikeasymboli = oikeakohta + 1;
    oikeavalmis = 1;  // testi
  }
  luuppi = 0;
  kierros = 0 ;

  if (rulla == 2) {
    byte secondChar = tuplausrulla[0];
    setChar(kruuna, tuplausrulla, klaava);  //asetetaan symbolit kohdilleen; muutoin lukittu merkki jää rullan tilalle
    vasenvalmis = 1;
    oikeavalmis = 1;
  }

  //while (vasenlukittu == 0 || keskilukittu == 0 || oikealukittu == 0)
  while (vasenvalmis == 0 || keskivalmis == 0 || oikeavalmis == 0) {
    kierros++;

    /* testataan paljonko rullat jätättää tinkercadissa, 3 rulla jää perään
  if(kierros%80==0){
   delay(10000); 
  }*/



//testattavat arvotut luvut
    //lcd.setCursor (11,0);
    //lcd.print(String(kierroksetvasen));
    //lcd.setCursor (13,0);
    //lcd.print(String(kierroksetkeski));
    //lcd.setCursor (15,0);
    //lcd.print(String(kierroksetoikea));
    //lcd.setCursor (0,0);
    //lcd.print(String(vasensymboli));
    //lcd.setCursor (4,0);
    //lcd.print(String(keskisymboli));
    //lcd.setCursor (8,0);
    //lcd.print(String(oikeasymboli));
    //lcd.setCursor (0,1);
    //lcd.print(String(vasenlukittu));
    //lcd.setCursor (4,1);
    //lcd.print(String(keskilukittu));
    //lcd.setCursor (8,1);
    //lcd.print(String(oikealukittu));
    //lcd.setCursor (11,1);
    //lcd.print(String(luuppi));

    //testi päättyy

    if (rulla == 1) {
      if (vasenvalmis == 0) {
        firstChar = normirullavasen[0];
      }
      if (keskivalmis == 0) {
        secondChar = normirullakeski[0];
      }
      if (oikeavalmis == 0) {
        thirdChar = normirullaoikea[0];
      }

      for (int i = 0; i < 80; i++) {
        /*testataan mikä kierros menossa
    delay(10);
lcd.setCursor (14,1);
lcd.print(String(i));
//testi päättyy */
        if (vasenvalmis == 0) {
          normirullavasen[i] = normirullavasen[i + 1];
        }
        if (keskivalmis == 0) {
          normirullakeski[i] = normirullakeski[i + 1];
        }
        if (oikeavalmis == 0) {
          normirullaoikea[i] = normirullaoikea[i + 1];
        }
      }
      if (vasenvalmis == 0) {
        normirullavasen[79] = firstChar;
      }
      if (keskivalmis == 0) {
        normirullakeski[79] = secondChar;
      }
      if (oikeavalmis == 0) {
        normirullaoikea[79] = thirdChar;
      }

      // Päivitä merkit

      setChar(normirullavasen, normirullakeski, normirullaoikea);  // tarviiko aliohjelmaa vai laitetaanko se 3 riviä vain tähän
                                                                   /*/ Näytä merkki
 // if(vasenlukittu==0){
//  lcd.setCursor(2, 0);
//  lcd.write((byte)0);
 // }
  if(keskilukittu==0){
  // Näytä merkki
  lcd.setCursor(6, 0);
  lcd.write((byte)1);
  }
  if(oikealukittu==0){
  // Näytä merkki
  lcd.setCursor(10, 0);
  lcd.write((byte)2);
  }
  //Odotellaan merkin päivittymistä 
  Vaikuttaa ettei ylläoleva koodi tee mitään*/
                                                                   //delay(100); //korvaa rullaäänellä
      for (int nuotti = 0; nuotti < 1; nuotti++) {

        int noteDuration = 65 / noteDurations[nuotti];

        tone(6, melody[nuotti], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.5;

        delay(pauseBetweenNotes);

        noTone(6);
      }

    } else if (rulla == 2) {
      secondChar = tuplausrulla[0];
      vasenvalmis = 1;
      oikeavalmis = 1;
      keskisymboli = random(6, 8);
      for (int i = 0; i < 80; i++) {

        tuplausrulla[i] = tuplausrulla[i + 1];
      }
      tuplausrulla[79] = secondChar;

      // Päivitä merkki
      setChar(kruuna, tuplausrulla, klaava);

      // Näytä merkki
      lcd.setCursor(2, 0);
      lcd.write((byte)3);

      // Näytä merkki
      lcd.setCursor(6, 0);
      lcd.write((byte)1);

      // Näytä merkki
      lcd.setCursor(10, 0);
      lcd.write((byte)4);
      //Odotellaan merkin päivittymistä
      //delay(150); //korvaa rullaäänellä
      for (int nuotti = 0; nuotti < 1; nuotti++) {

        int noteDuration = 65 / noteDurations[nuotti];

        tone(6, melody[nuotti], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.5;

        delay(pauseBetweenNotes);

        noTone(6);
      }
    }
        rullankohta = kierros + luuppi;  // ei välttämättä enää tarvi

    if (kierros > vasensymboli * 8 ) {  // tarvittaessa lisää mahdollisuus useammalle kierrokselle
      vasenvalmis = 1;
    }
    if (kierros > keskisymboli * 8  ) {  // tarvittaessa lisää mahdollisuus useammalle kierrokselle
      keskivalmis = 1;
    }
    if (kierros > oikeasymboli * 8) {  // tarvittaessa lisää mahdollisuus useammalle kierrokselle
      oikeavalmis = 1;
    }
    if (kierros % 80 == 0) {  // tarviiko enää
      luuppi++;
      if(luuppi>2){ //varalta jos taulukko korruptoituu eikä arvonta pääty 
      break;
      }
      }
    
  }  //while looppi päättyy tähän
    //lcd.setCursor (14,1);
    //lcd.print(String(tuplausrullakohta));
    //////////////////////////////////////////////////////////////////////
    if(rulla==2){
    if(keskisymboli == 1 || keskisymboli == 3 || keskisymboli == 5 || keskisymboli == 7 || keskisymboli == 9 ){
    return 1;
    }else{
    return 2;
    }
    }
    

    //SYDÄN
    if (vasensymboli == 0 ) {
      if (keskisymboli == 2 ) {
        if (oikeasymboli == 0 || oikeasymboli == 2) {
          voittorivi = 1;

          lcd.createChar(7, sydan);
          lcd.setCursor(2, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(6, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(10, 0);
          lcd.write((byte)7);

          voitettuSumma = panos;
          lcd.setCursor(0, 1);
          lcd.print("Voitto " + String(voitettuSumma));
          voitto = 1;
        }
      }
      //1RIVI
    } else if (vasensymboli == 1 || vasensymboli == 5 || vasensymboli == 9 || vasensymboli == 2) {
      if (keskisymboli == 0 || keskisymboli == 3 || keskisymboli == 7 || keskisymboli == 8 || keskisymboli == 6) {
        if (oikeasymboli == 4 || oikeasymboli == 9 || oikeasymboli == 5 || oikeasymboli == 2) {
          voittorivi = 2;

          lcd.createChar(7, yksirivi);
          lcd.setCursor(2, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(6, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(10, 0);
          lcd.write((byte)7);

          voitettuSumma = panos * 2;
          lcd.setCursor(0, 1);
          lcd.print("Voitto " + String(voitettuSumma));
          voitto = 1;
        }
      }
      //RISUAITA
    } else if (vasensymboli == 3 || vasensymboli == 8) {
      if (keskisymboli == 1 || keskisymboli == 9) {
        if (oikeasymboli == 3 || oikeasymboli == 8 || oikeasymboli == 2) {
          voittorivi = 3;
          lcd.createChar(7, risuaita);
          lcd.setCursor(2, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(6, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(10, 0);
          lcd.write((byte)7);
          voitettuSumma = panos * 3;
          lcd.setCursor(0, 1);
          lcd.print("Voitto " + String(voitettuSumma));
          voitto = 1;
        }
      }
      //PATA
    } else if (vasensymboli == 4 || vasensymboli == 7) {
      if (keskisymboli == 4 ) {
        if (oikeasymboli == 1 || oikeasymboli == 7 || oikeasymboli == 2) {
          voittorivi = 4;
          lcd.createChar(7, pata);
          lcd.setCursor(2, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(6, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(10, 0);
          lcd.write((byte)7);
          voitettuSumma = panos * 4;
          lcd.setCursor(0, 1);
          lcd.print("Voitto " + String(voitettuSumma));
          voitto = 1;
        }
      }
      //7
    } else if (vasensymboli == 6) {
      if (keskisymboli == 5) {
        if (oikeasymboli == 6 || oikeasymboli == 2) {
          voittorivi = 5;

          lcd.createChar(7, seiska);
          lcd.setCursor(2, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(6, 0);
          lcd.write((byte)7);

          // Näytä merkki
          lcd.setCursor(10, 0);
          lcd.write((byte)7);
          lcd.setCursor(0, 1);
          voitettuSumma = panos * 5;
          lcd.print("Voitto " + String(voitettuSumma));
          voitto = 1;
        }
      }
    }
  //}  lienee liikaa kun while loppuu jo aiemmin ///////////////////////////////
  if (vasenlukittu == 0 && keskilukittu == 0 && oikealukittu == 0) {
    lukittiinko = 0;
  } else {
    lukittiinko = 1;
  }
  if (voitto == 1) {
    Oikein();
do{
    tuplaus = Tuplaus();
    if (tuplaus == 3){
      break;
    }
}while (tuplaus == 1);
    vasenlukittu = 0;
    keskilukittu = 0;
    oikealukittu = 0;

    lcd.setCursor(2, 1);
    lcd.print(" ");

    lcd.setCursor(6, 1);
    lcd.print(" ");
    
    lcd.setCursor(10, 1);
    lcd.print(" ");

  } 
  //lukittiinko=1;

  delay(200);
   vasenlukittu = 0;
    keskilukittu = 0;
    oikealukittu = 0;
    lcd.setCursor(2, 1);
    lcd.print(" ");

    lcd.setCursor(6, 1);
    lcd.print(" ");
    
    lcd.setCursor(10, 1);
    lcd.print(" ");

    
    return 0;
}
bool nappiPainettu() {  // tarkista napin tila //nimetään uusiksi
                        //menisikö sitten kuitenkin yhdellä nappialiohjelmalla

  int tila = digitalRead(nappi);


  if (tila == LOW && (millis() - lastDebounceTime) > debounceDelay) {  // tarkista onko nappi painettu sekä inputin debounce

    lastDebounceTime = millis();  // alusta debouncen aika
    return true;
  } else {
    return false;
  }
}

bool nappiOnPainettu(void)  // tarkista napin tila //nimetään uusiksi
{
  int tila = digitalRead(19);
  if (tila == HIGH) {
    return false;
  } else {
    return true;
  }
}


bool Lukitus1() {  // tarkista napin tila

  int tila = digitalRead(8);

  if (tila == LOW && (millis() - lastDebounceTime) > debounceDelay)  // tarkista onko nappi painettu sekä inputin debounce
  {
    lastDebounceTime = millis();  // alusta debouncen aika
    return true;
  } else {
    return false;
  }
}

bool Lukitus2() {  // tarkista napin tila

  int tila = digitalRead(9);

  if (tila == LOW && (millis() - lastDebounceTime) > debounceDelay)  // tarkista onko nappi painettu sekä inputin debounce
  {
    lastDebounceTime = millis();  // alusta debouncen aika
    return true;
  } else {
    return false;
  }
}

bool Lukitus3() {  // tarkista napin tila

  int tila = digitalRead(10);

  if (tila == LOW && (millis() - lastDebounceTime) > debounceDelay)  // tarkista onko nappi painettu sekä inputin debounce
  {
    lastDebounceTime = millis();  // alusta debouncen aika
    return true;
  } else {
    return false;
  }
}
/* siirretty voitontarkistus arvontaan; tarpeeton aliohjelma
void VoitonTarkistus(){
  lcd.setCursor (0,3);

  switch (voittorivi)
  {
    case 1:
      voitettuSumma = panos;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 2:
      voitettuSumma = panos * 2;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 3:
      voitettuSumma = panos * 3;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 4:
      voitettuSumma = panos * 4;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 5:
      voitettuSumma = panos * 5;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 6:
      voitettuSumma = panos * 6;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;

    case 7:
      voitettuSumma = panos * 7;
      lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;
    case 8:
      voitettuSumma = panos * 8;
      lcd.print("Voitto " + String(voitettuSumma));  
    voitto=1;
      break;
	
    case 9:
      voitettuSumma = panos * 10;
      lcd.print("Voitto " + String(voitettuSumma)); 
    voitto=1;
      break;

    case 10:
      voitettuSumma = panos * 25;
     lcd.print("Voitto " + String(voitettuSumma));
    voitto=1;
      break;
    case 11:
      lcd.print("Ei voittoa.");
      voitto=0;
      break;

    case 12:
      voitettuSumma = voitettuSumma * 2;
      lcd.print("Voitto " + String(voitettuSumma));
      break;
                      
  }
  delay(1500);
  lcd.setCursor(2,1);
  lcd.print(" ");
}
*/


int Tuplaus() {
  vasenlukittu = 0;
  keskilukittu = 0;
  oikealukittu = 0;
  tuplaus=0;
  kruunavalinta = 0;
  klaavavalinta = 0;
    int melody[] = {
    110
  };

  int noteDurations[] = {
    1
  };
  int kruunavaiklaava;
  //  	lcd.setCursor(0, 1);
  //     lcd.print(String(voittorivi));
  // Näytä merkki
  lcd.setCursor(2, 0);
  lcd.write((byte)3);

  // Näytä merkki
  lcd.setCursor(6, 0);
  lcd.write((byte)5);

  // Näytä merkki
  lcd.setCursor(10, 0);
  lcd.write((byte)4);

  while (kruunavalinta == 0 && klaavavalinta == 0) {
    for (int nuotti = 0; nuotti < 30; nuotti++) {

      // Nuotin keston määritys, 1000 millisekuntia jaetaan melodianKesto osassa määritellyillä nuotin kestoilla.

      int nuotinKesto = 1300 / melodianKesto[nuotti];

      tone(6, melodia[nuotti], nuotinKesto);

      // Tehdään nuottien väliin pieni tauko.

      int taukoNuottienValissa = nuotinKesto * 1.30;

      delay(taukoNuottienValissa);

      // Lopetetaan nuotin soitto:

      noTone(6);

      if (Lukitus1() == true) {
        kruunavalinta = 1;
        klaavavalinta = 0;
        
kruunavaiklaava = symboliarvonta(2,0,0,0);	

if (kruunavaiklaava==1) {
      
      setChar(kruuna, kruuna, klaava);

      // Näytä merkki
      lcd.setCursor(2, 0);
      lcd.write((byte)3);

      // Näytä merkki
      lcd.setCursor(6, 0);
      lcd.write((byte)3);

      // Näytä merkki
      lcd.setCursor(10, 0);
      lcd.write((byte)4);

	Oikein();
        lcd.setCursor(0, 1);
        voitettuSumma=voitettuSumma*2;
      lcd.print("Voitto " +   String(voitettuSumma));
      delay(1500);
      return 1;
      }else {
	Kosautus();
        // Näytä merkki
      lcd.setCursor(6, 0);
      lcd.write((byte)4);
          lcd.setCursor(0, 1);
        voitettuSumma=voitettuSumma*0;
      lcd.print("EI Voittoa ");
      delay(1500);
      return 0;
    } 
      }
      if (Lukitus2() == true) {
        // VoitonTarkistus();
        rahat = rahat + voitettuSumma;  
        tuplaus=0;
        display.showNumberDec(rahat);
        return 3;  //tarkistetaan vielä
      }
      if (Lukitus3() == true) {
        kruunavalinta = 0;
        klaavavalinta = 1;
        
kruunavaiklaava = symboliarvonta(2,0,0,0);	

    if (kruunavaiklaava==2) {
      setChar(kruuna, klaava, klaava);
      // Näytä merkki
      lcd.setCursor(2, 0);
      lcd.write((byte)3);

      // Näytä merkki
      lcd.setCursor(6, 0);
      lcd.write((byte)4);

      // Näytä merkki
      lcd.setCursor(10, 0);
      lcd.write((byte)4);

	    Oikein();
       lcd.setCursor(0, 1);
        voitettuSumma=voitettuSumma*2;
      lcd.print("Voitto " +   String(voitettuSumma));
      delay(1500);
      return 1;
      }else{
     // Näytä merkki
      lcd.setCursor(6, 0);
      lcd.write((byte)3);
	 Kosautus();
             lcd.setCursor(0, 1);
        voitettuSumma=voitettuSumma*0;
      lcd.print("EI Voittoa");
      delay(1500);
   return 0;
    }
        }
      }

    }



  
 //     lcd.setCursor(0, 1);
 //     lcd.print("Voitto " +   String(voitettuSumma));
 //     kruunavaiklaava = random(1, 3);
      lcd.setCursor(14, 1);
      lcd.print(kruunavaiklaava);

    
    
}

void Oikein() {  // fanfaarin tyylinen
  for (int nuotti = 0; nuotti < 4; nuotti++) {

    // Nuotin keston määritys, 1000 millisekuntia jaetaan oikeanKesto osassa määritellyillä nuotin kestoilla.

    int nuotinKesto = 1300 / oikeanKesto[nuotti];

    tone(6, oikeaValinta[nuotti], nuotinKesto);

    // Tehdään nuottien väliin pieni tauko.

    int taukoNuottienValissa = nuotinKesto * 1.30;

    delay(taukoNuottienValissa);

    // Lopetetaan nuotin soitto:

    noTone(6);
  }
}

void Kosautus() {  //kosh-äänen korvike
  for (int nuotti = 0; nuotti < 4; nuotti++) {

    // Nuotin keston määritys, 1000 millisekuntia jaetaan wrongDuration osassa määritellyillä nuotin kestoilla.

    int nuotinKesto = 1400 / vaaranKesto[nuotti];

    tone(6, vaaraValinta[nuotti], nuotinKesto);

    // Tehdään nuottien väliin pieni tauko.

    int taukoNuottienValissa = nuotinKesto * 1.30;

    delay(taukoNuottienValissa);

    // Lopetetaan nuotin soitto:

    noTone(6);
  }
}

void lukitus(int lukitus)  //lukitaan pyydettyä linjaa
{

  if (voitto == 1)  // katotaan tuliko voittoa viimekierroksella jos kyllä lopetetaan aliohjelma
  {
    return;  // Lopetetaan aliohjelma
  }
  if (lukittiinko == 1) {  // katotaan lukittiinko viimekierroksella jos kyllä lopetetaan aliohjelma
    vasenlukittu = 0;
    lcd.setCursor(2, 1);
    lcd.print(" ");
    keskilukittu = 0;
    lcd.setCursor(6, 1);
    lcd.print(" ");
    oikealukittu = 0;
    lcd.setCursor(10, 1);
    lcd.print(" ");
    return;  // Lopetetaan aliohjelma
  }
  if (panos > edellinenpanos && panos > 1)  // katotaan vaihtuiko panos jos kyllä lopetetaan aliohjelma
  {
    return;  // Lopetetaan aliohjelma
  }
  if (lukitus == 1) {
    if (vasenlukittu == 0) {
      vasenlukittu = 1;
    } else {
      vasenlukittu = 0;
      lcd.setCursor(2, 1);
      lcd.print(" ");
    }
  }
  if (lukitus == 2) {
    if (keskilukittu == 0) {
      keskilukittu = 1;
    } else {
      keskilukittu = 0;
      lcd.setCursor(6, 1);
      lcd.print(" ");
    }
  }
  if (lukitus == 3) {
    if (oikealukittu == 0) {
      oikealukittu = 1;
    } else {
      oikealukittu = 0;
      lcd.setCursor(10, 1);
      lcd.print(" ");
    }
  }
}
void Panos() {       //asetetaan panos, se näkyy 7-segmentillä
  if (panos == 6) {  // alusta laskuri mikäli se nousee 5 asti
    panos = 1;
  }

  switch (panos) {  // näytä panos segmenttinäytöllä
    case 1:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      delay(200);
      break;
    case 2:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      delay(200);
      break;
    case 3:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      delay(200);
      break;
    case 4:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      delay(200);
      break;
    case 5:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      delay(200);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));                                   //otetaan arvontaan kanta
  lcd.begin(16, 2);                                            // Asetetaan näytön koko
  symbolipankki();                                             //alustetaan merkit
  setChar(normirullavasen, normirullakeski, normirullaoikea);  // Asetetaan uniikki merkki //kenties tarpeeton

  lcd.setCursor(0, 0);
  lcd.print("TERVETULOA");
  lcd.setCursor(0, 1);
  lcd.print("PELAAMAAN");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TOIVOTTAA");
  lcd.setCursor(0, 1);
  lcd.print("PELIPROJEKTI");
  delay(3000);
  lcd.clear();

  pinMode(19, INPUT_PULLUP);  //nappia varten, muutetaan muiden aliohjelmien mukaan
  pinMode(8, INPUT_PULLUP);   //nappia varten, muutetaan muiden aliohjelmien mukaan
  pinMode(9, INPUT_PULLUP);   //nappia varten, muutetaan muiden aliohjelmien mukaan
  pinMode(10, INPUT_PULLUP);  //nappia varten, muutetaan muiden aliohjelmien mukaan

  pinMode(nappi, INPUT_PULLUP);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  // otetaan vakiomerkit
  lcd.createChar(3, kruuna);
  lcd.createChar(4, klaava);
  lcd.createChar(5, lukko);

  //piirretään arvontaruudukko
  lcd.setCursor(0, 0);
  lcd.print("I");
  lcd.setCursor(4, 0);
  lcd.print("I");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(12, 0);
  lcd.print("I");

  setChar(normirullavasen, normirullakeski, normirullaoikea);  // tarviiko aliohjelmaa vai laitetaanko se 3 riviä vain tähän
  //Näytä merkki
  lcd.setCursor(2, 0);
  lcd.write((byte)0);

  // Näytä merkki
  lcd.setCursor(6, 0);
  lcd.write((byte)1);

  // Näytä merkki
  lcd.setCursor(10, 0);
  lcd.write((byte)2);

  //panosnäytölle 1 alkuun
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);

  display.setBrightness(7);
  display.showNumberDec(rahat);
}

void loop() {
  //   setChar(normirullavasen, normirullakeski, normirullaoikea); // tarviiko aliohjelmaa vai laitetaanko se 3 riviä vain tähän
  //Näytä merkki
  lcd.setCursor(2, 0);
  lcd.write((byte)0);

  // Näytä merkki
  lcd.setCursor(6, 0);
  lcd.write((byte)1);

  // Näytä merkki
  lcd.setCursor(10, 0);
  lcd.write((byte)2);
  if (Lukitus1()) {
    lukitus(1);
  }
  if (Lukitus2()) {
    lukitus(2);
  }
  if (Lukitus3()) {
    lukitus(3);
  }
  if (vasenlukittu == 1) {
    // Näytä lukitus
    lcd.setCursor(2, 1);
    lcd.write((byte)5);
  }
  if (keskilukittu == 1) {
    // Näytä lukitus
    lcd.setCursor(6, 1);
    lcd.write((byte)5);
  }
  if (oikealukittu == 1) {
    // Näytä lukitus
    lcd.setCursor(10, 1);
    lcd.write((byte)5);
  }

  // korvataan käynnistämistoiminnolla
  //voitettuSumma=1; //testiä varten
  //Tuplaus(); //testiä varten

  if (nappiPainettu()) {
    if (vasenlukittu == 0 && keskilukittu == 0 && oikealukittu == 0) {
      panos++;
      Panos();
    }
  }


  if (nappiOnPainettu()) {
    if (rahat >= panos) {  //jos rahat ei riitä panokseen, peli ei ala
      kaynnista = 1;
      edellinenpanos = panos;
      rahat = rahat - panos;
      display.showNumberDec(rahat);
      symboliarvonta(1, vasenlukittu, keskilukittu, oikealukittu);
      //VoitonTarkistus();
    }
  }


  symbolipankki();  //resetoidaan rullat oletuksiin

  //delay(1000);
  kaynnista = 0;
  lcd.setCursor(0, 1);
  lcd.print("            ");
  // lcd.setCursor(0, 1);
  // lcd.print(String(vasenlukittu));
  // lcd.setCursor(15, 1);
  //    lcd.print(String(lukittiinko));
  if (lukittiinko == 1) {
    lcd.setCursor(2, 1);
    lcd.print(" ");

    lcd.setCursor(6, 1);
    lcd.print(" ");
    
    lcd.setCursor(10, 1);
    lcd.print(" ");
    

    vasenlukittu = 0;
    keskilukittu = 0;
    oikealukittu = 0;
    //  lcd.setCursor(0, 1);
    // lcd.print(String(vasenlukittu));
  }
}
