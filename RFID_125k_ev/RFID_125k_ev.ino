// RFID_UART.ino

#include <SoftwareSerial.h>

// quelques leds
#define LROUGE 5
#define LVERTE 4
#define LJAUNE 3

#define PAUSE 5000

SoftwareSerial sSerial(8,7);

// Après une lecture, on ne traite plus
// les message pendant PAUSE millisecondes
int pret = 1;
unsigned long previousMillis = 0;

// liste d'IDs ok
const unsigned long long auth[] = {
  0x0000002C00827F4A,
  0x0000030405060708,
  0x000000bbccddeeff,
  0x000000ff00042154,
  0x0000003344556677};

//  ff00042154

void setup() {
  // moniteur en 115200
	Serial.begin(115200);
	Serial.println("**** CONTROLE D'ACCES RFID ****");
  // softserial en 9600
  sSerial.begin(9600);
  pinMode(LROUGE, OUTPUT);
  pinMode(LJAUNE, OUTPUT);
  pinMode(LVERTE, OUTPUT);
}

// fonction pour obtenir un octet depuis
// deux caractère hexa
byte hex2int(char haut, char bas) {
  byte ret=0;

  // version opérateur ternaire
  ret |= haut < 58 ? (haut-48)<<4 : (haut-55)<<4;
  ret |= bas < 58 ? bas-48 : bas-55;

  // version "if"
  /*
  if(haut<58)
    ret |= (haut-48)<<4;
  else
    ret |= (haut-55)<<4;

  if(bas<58)
    ret |= (bas-48);
  else
    ret |= (bas-55);
  */

  return ret;
}

// fonction de validation de l'ID
// retourne "vrai" si ID dans la liste
bool valide(unsigned long long ID) {
  // on parcours la liste (enum)
  for(int i=0; i<(sizeof(auth)/sizeof(*auth)); i++) {
      // on retourne avec vrai si on trouve
      if(auth[i] == ID){ return true; }
    }
  // on a pas trouvé on retourne avec faux
  return false;
}

void loop() {
  // nbr de caractère reçu du lecteur RFID
  int sret=0;
  char cardmsg[14] = {0};
  byte cardID[5] = {0};
  int i=0;
  // valeur numérique sur 64 bits
  // (attention long long sur AVR == slow slow)
  unsigned long long numID = 0;

  unsigned long currentMillis = millis();

  // des trucs à lire ?
  if(sSerial.available()) {
    // on récupère les caractères dans cardmsg
    // sret contient le nombre de caractères reçus
    sret = sSerial.readBytes(cardmsg, 14);
  }

  // Une lecture a déja eu lieu
  if(!pret) {
    // cela fait-il plus de PAUSE millisecondes ?
    if(currentMillis - previousMillis >= PAUSE) {
      // on enregistre le moment
      previousMillis = currentMillis;
      digitalWrite(LJAUNE, LOW);
      digitalWrite(LROUGE, LOW);
      digitalWrite(LVERTE, LOW);
      // et on est prêt à gérer les données lues
      pret=1;
    } else {
      return;
    }
  }
  
  // A-t-on reçu 14 caractères ?
  if(sret == 14) {
    for(int i=0; i<5; i++) {
      // on compose un tableau de 5 valeurs 8 bits
      // a partir des caractères alpha-hexa
      cardID[i] = hex2int(cardmsg[i*2+1],cardmsg[i*2+2]);
      // ainsi qu'une variable numérique
      numID=(numID<<8)|cardID[i];
    }

    // on affiche le résultat numérique
    for(int i=0; i<5; i++) {
      Serial.print(cardID[i],HEX);
      Serial.print(" ");
    }

    // ainsi que la partie du message du lecteur
    // avec l'ID de la carte lue pour comparaison
    Serial.print(": ");
    for(int i=1; i<11; i++) {
      Serial.print(cardmsg[i]);
    }

    // message original complet
    Serial.print("     ");
    for(int i=0; i<14; i++) {
      Serial.print("[");
      if(cardmsg[i]<0x10)
        Serial.print('0');        
      Serial.print(cardmsg[i],HEX);
      Serial.print("]");
    }
    

    // vérification de la somme de contrôle qui
    // accompagne l'ID
    Serial.print("  Checksum (0x");
    // Le checksum est également en alpha-hexa
    Serial.print(hex2int(cardmsg[11],cardmsg[12]),HEX);
    // Le checksum est un XOR des 5 valeurs de l'ID entre elles
    if(hex2int(cardmsg[11],cardmsg[12]) == cardID[0]^cardID[1]^cardID[2]^cardID[3]^cardID[4]) {
      Serial.print(") OK");
    } else {
      Serial.print(") BAD !!");
      return;
    }
    
    Serial.println("");

    // validation de l'accès
    // la valeur long long est-elle dans notre liste ?
    if(valide(numID)) {
      // ok, ID valide, on laisse passer, activons un relais, etc
      Serial.println("ACCES AUTORISE");
      digitalWrite(LVERTE, HIGH);
    } else {
      // l'ID n'est pas dans la liste, on active les tourelles laser, etc...
      Serial.println("EXTERMINATE ! EXTERMINATE ! EXTERMINAAAAATE ! ");
      digitalWrite(LROUGE, HIGH);
    }
      
    digitalWrite(LJAUNE, HIGH);
    pret=0;
  }
}

