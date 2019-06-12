/*
  Theremin accionado por sensor de luz. 
  Las frecuencias son discretas siguiendo la escala cromática,
  diatónica o pentatónica (se cambia pulsando el botón 2).
  Al inicio o al pulsar el botón 1 se realiza una calibración
  de los valores más alto y más bajo de la luz.

  Trabajo realizado para el módulo de Electrónica para Instalaciones
  del Máster en Arte Sonoro de la Universidad de Barcelona
  Profesora: Lina Bautista
  mayo de 2019

  Autor: Carlos Arturo Guerra Parra
  Software de dominio público
*/

// variable to hold sensor value
int sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;
// número de semitono obtenido del sensor
int semitono = 0;
// nota más baja
int fundamental = 400;
// variables para determinar el pitch
int pitch, pitch_old = 0;
// LED pin
const int ledPin = 13;
// Button 1, para calibrar
const int button1Pin = 9; 
// estado del button1
int button1State = 0;
// Button 2, para cambiar de escala
const int button2Pin = 7; 
// estado del button1
int button2State = 0;
// Buzzer pin
const int buzzerPin = 8;
// Escala en uso
int escala = 0;


/// FUNCIONES ********************

// Calibra el máximo y el mínimo del sensor de luz (sensorHigh y sensorLow)
void calibrate ();
// Devuelve el grado de la escala más cercano
int cromaticToEscala (int semitono, int escala = 1);  // escala == 0: cromática (sin conversión)
                                                      // escala == 1: diatónica -- por defecto
                                                      // escala == 2: pentatónica
// retorna en cada llamada un número entre 0 y 2 consecutivamente (para cambiar de escala)
int siguienteEscala (int anterior);
// hace parpadear el led 13 tantas veces como se indique
void ledParpadeo(int num_parpadeos);



/// IMPLEMENTACIÓN ****************

void setup() {
  // Make the LED pin an output and turn it on
  pinMode(ledPin, OUTPUT);

  // initialize the pushbutton pins as inputs:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  calibrate();
}

void loop() {
  // comprobar si se pulsa botón 1 (para calibrar)
  button1State = digitalRead(button1Pin);
  if (button1State == HIGH) calibrate();

  // comprobar si se pulsa botón 2 (para cambiar de escala)
  button2State = digitalRead(button2Pin);
  if (button2State == HIGH) {
    noTone(buzzerPin);
    delay(500); // para evitar rebotes y el tiempo excesivo de pulsación
    escala = siguienteEscala(escala); 
    ledParpadeo(escala+1);}
  
  //read the input from A0 and store it in a variable
  sensorValue = analogRead(A0);
  // map the sensor values to 2 octaves (0-35 semitonos)
  semitono = map(sensorValue, sensorLow, sensorHigh, 0, 35); // configurado para 36 semitonos (3 octavas)
  semitono = cromaticToEscala(semitono, escala);
  pitch = pow(pow(2, (1/12.0)),semitono)*fundamental;

  // play the tone
  if (pitch != pitch_old) {
    noTone(buzzerPin);
    tone(buzzerPin, pitch);
    pitch_old = pitch;
  }
  delay(100);
}

void calibrate () {
  noTone(buzzerPin);
  digitalWrite(ledPin, HIGH);
  sensorLow = 1023;
  sensorHigh = 0;
  // calibrate for the first five seconds after program runs
  unsigned long time = millis();
  while (millis()-time < 5000) {
    // record the maximum sensor value
    sensorValue = analogRead(A0);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    // record the minimum sensor value
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  // turn the LED off, signaling the end of the calibration period
  digitalWrite(ledPin, LOW);
}


static int cromaticToEscala (int semitono, int escala) {
  int toCromatica[] = {0,1,2,3,4,5,6,7,8,9,10,11}; // cromática (sin conversión)
  int toDiatonica[] = {0,0,2,2,4,5,5,7,7,9,9,11};  // diátonica
  int toPentatonica[] = {0,0,2,2,4,4,4,7,7,9,9,9};   // pentatónica
  int *conversor;
  switch (escala) {
    case 0:
      conversor = &toCromatica[0];
      break;
    case 1:
      conversor = &toDiatonica[0];
      break;
    case 2:
      conversor = &toPentatonica[0];
      break;
    default:
      conversor = &toCromatica[0];
      break;
  }
  int nota_cromatica = semitono%12;
  int nota_diatonica = 0;
  nota_diatonica = conversor[nota_cromatica];
  return (semitono/12)*12+nota_diatonica;
}


int siguienteEscala(int anterior) {
  int contador = anterior;
  contador++;
  if (contador > 2) contador = 0;
  return contador;
}


void ledParpadeo (int num_parpadeos) {
  for (int i=0; i<num_parpadeos; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
