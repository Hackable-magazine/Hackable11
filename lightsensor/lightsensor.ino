#define LED1_P A0
#define LED1_N A1
#define LED2_P A2
#define LED2_N A3

#define PAUSE 20

void setup() {
  // Le convertisseur analogique/numérique
  // utilise les 1,1 volts comme référence
  analogReference(INTERNAL);

  // moniteur série
  Serial.begin(115200);

  // broches led 1 en sortie
  pinMode(LED1_N, OUTPUT);
  pinMode(LED1_P, OUTPUT);

  // broches led 2 en sortie
  pinMode(LED2_N, OUTPUT);
  pinMode(LED2_P, OUTPUT);
}

void loop() {
  int val=0;

  // alumage de la led 2
  pinMode(LED2_N, OUTPUT);
  digitalWrite(LED2_N, LOW);
  digitalWrite(LED2_P, HIGH);

  // On "charge" la led
  digitalWrite(LED1_N, HIGH);
  digitalWrite(LED1_P, LOW);
  // cathode de la led en entrée
  pinMode(LED1_N, INPUT);
  // désactivation de la résistance de rappel
  digitalWrite(LED1_N, LOW);
  // on attend un peu
  delay(PAUSE);
  // mesure de la tension sur la cathode
  val=analogRead(LED1_N);

  // On inverse les rôle des leds
  // et on fait de même
  pinMode(LED1_N, OUTPUT);
  digitalWrite(LED1_N, LOW);
  digitalWrite(LED1_P, HIGH);
  digitalWrite(LED2_N, HIGH);
  digitalWrite(LED2_P, LOW);
  pinMode(LED2_N, INPUT);
  digitalWrite(LED2_N, LOW);
  delay(PAUSE); 
  val+=analogRead(LED2_N);

  // val contient la somme les valeurs lues
  // du convertisseur pour led 1 et led 2
  Serial.println(val);
}

