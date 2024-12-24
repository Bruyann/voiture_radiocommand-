// Définir le pin 21
const int pin = 21;

// Définir les broches pour le pont en H
const int motorPin1 = 18;  // Moteur A, direction 1
const int motorPin2 = 19;  // Moteur A, direction 2
const int motorPin3 = 22;  // Moteur B, direction 1
const int motorPin4 = 23;  // Moteur B, direction 2

void setup() {
  // Initialiser le port série (optionnel, pour débogage)
  Serial.begin(115200);

  // Configurer le pin 21 comme sortie
  pinMode(pin, OUTPUT);

  // Configurer les broches des moteurs comme sorties
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

    // Passer le pin 21 à l'état logique 1 (HIGH)
  digitalWrite(pin, HIGH);
  Serial.println("Pin 21 HIGH"); // Affiche dans le terminal série
  delay(1000); // Attendre 1 seconde

  // Passer le pin 21 à l'état logique 0 (LOW)
  digitalWrite(pin, LOW);
  Serial.println("Pin 21 LOW"); // Affiche dans le terminal série
  delay(1000); // Attendre 1 seconde


}

void loop() {


    // Contrôle du moteur A dans les deux directions
  digitalWrite(motorPin1, HIGH); // Activer direction 1
  digitalWrite(motorPin2, LOW);
  delay(2000);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH); // Changer la direction
  delay(2000);

  // Contrôle du moteur B dans les deux directions
  digitalWrite(motorPin3, HIGH); // Activer direction 1
  digitalWrite(motorPin4, LOW);
  delay(2000);

  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH); // Changer la direction
  delay(2000);
}
