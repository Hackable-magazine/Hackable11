#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

// macro pour s'y retrouver dans les broches
#define BIT0    7
#define BIT1    6
#define BIT2    5
#define BIT3    4
#define LATCH_G 3
#define LATCH_D 2
#define MALED   8

// Fonction envoyant un état bas durant 1ms
void latch(int digit) {
  digitalWrite(digit, LOW);
  delay(1);
  digitalWrite(digit, HIGH);
  delay(1);  
}

// Fonction envoyant les deux valeurs à l'afficheur
void setdigits(int val) {
  if(val < 10) {
    // en dessous de 10, le chiffre de gauche est toujours 0
    digitalWrite(BIT0, LOW);
    digitalWrite(BIT1, LOW);
    digitalWrite(BIT2, LOW);
    digitalWrite(BIT3, LOW);
    latch(LATCH_G); // validation de la valeur
  } else {
    // sinon on traite les dizaines
    // On divise la valeur par 10
    // puis on test chacun des 4 bits
    digitalWrite(BIT0, (val/10) & B00000001 ? HIGH : LOW);      
    digitalWrite(BIT1, (val/10) & B00000010 ? HIGH : LOW);      
    digitalWrite(BIT2, (val/10) & B00000100 ? HIGH : LOW);      
    digitalWrite(BIT3, (val/10) & B00001000 ? HIGH : LOW);
    latch(LATCH_G); // validation de la valeur du chiffre de gauche
  }
  // exemple avec 54
  // 54 / 10 = 5
  // 5 * 10 = 50
  // 54 - 50 = 4
  // 54 - ((54/10) * 10) = 4
  digitalWrite(BIT0, (val - ((val/10) * 10)) & B00000001 ? HIGH : LOW);      
  digitalWrite(BIT1, (val - ((val/10) * 10)) & B00000010 ? HIGH : LOW);      
  digitalWrite(BIT2, (val - ((val/10) * 10)) & B00000100 ? HIGH : LOW);      
  digitalWrite(BIT3, (val - ((val/10) * 10)) & B00001000 ? HIGH : LOW);
  latch(LATCH_D); // validation de la valeur du chiffre de droite
}

void erreur(int pause) {
  while(1) {
    digitalWrite(MALED, HIGH);
    delay(pause);
    digitalWrite(MALED, LOW);
    delay(pause);
  }
}

void setup() {
  // Tous les ports en sortie
  pinMode(BIT0, OUTPUT);
  pinMode(BIT1, OUTPUT);
  pinMode(BIT2, OUTPUT);
  pinMode(BIT3, OUTPUT);
  pinMode(LATCH_G, OUTPUT);
  pinMode(LATCH_D, OUTPUT);
  pinMode(MALED, OUTPUT);

  for(int i=0; i < 10; i++) {
    setdigits(i*11);  
    delay(420);
  }
  setdigits(0);
  delay(500);
}

// Tableau de structures contenant date/heure complètes
tmElements_t hackable[5] = {
  {0,0,0,2,2+1,2,2016-1970},  // hackable11 (fake)
  {0,0,0,3,29+1,3,2016-1970}, // hackable12
  {0,0,0,2,6+1,6,2016-1970},  // hackable13
  {0,0,0,2,8+1,8,2016-1970},  // hackable14
  {0,0,0,2,3+1,10,2016-1970}  // hackable15
};

void loop() {
  // Structure pour contenir la date/heure depuis le DS1307
  tmElements_t tm;
  // Différence entre dates
  long diff;

  // Peut-on lire la date/heure ?
  if (RTC.read(tm)) {
    // oui !
    // On parcours le tableau de date
    for(int i=0; i<5; i++) {
      // calcul de nombre de jours restants
      diff = makeTime(hackable[i]) - makeTime(tm);
      // la date est dans le futur ? Oui, on arrête là
      if(diff>0) break;
    }
    // On divise le résultat par une macro qui est dans Time.h
    // et est 60*60*24 (60 secondes par minute, 60 minutes par heures,
    // et 24 heures par jour
    // Et on envoi cela à l'afficheur via notre fonction
    setdigits(diff / SECS_PER_DAY);
  } else {
    // la date n'a pas été lue
    if (RTC.chipPresent()) {
      // le module est détectée mais la date n'est pas valide
      // l'heure n'a sans doute pas été réglée
      erreur(250);
    } else {
      // le module n'est pas détecté
      erreur(50);
    }
  }
  // on boucle toutes les 5 secondes
  delay(5000);
}

