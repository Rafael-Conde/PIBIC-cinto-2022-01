// Bibliotecas
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// Declarando o módulo MPU6050
MPU6050 mpu6050(Wire);

// Declarando o módulo SIM900 e suas portas de comunicação serial
SoftwareSerial SIM900(7, 8);


// Variáreis globais
#define pinoLED 13
#define buzzer 12
float gyrox, gyroy, gyroz;
float angx, angy; // Não vamos usar angz pois ele descalibra com movimentos bruscos
int taxaDeTransmissao = 19200;

void setup() {
  // Iniciando comunicação serial
  Serial.begin(taxaDeTransmissao);

  // Iniciando a comunicação Arduino <-> MPU6050 e calibrando
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  // Definindo LED e buzzer como saída
  pinMode(pinoLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Ligando o SIM900
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(5000);
  //Serial.println("/n/r SIM900 ligado");
  digitalWrite(buzzer, HIGH);
  delay(1);//wait for 1ms
  digitalWrite(buzzer, LOW);
  delay(1);//wait for 1ms

  // Iniciando a comunicação Arduino <-> SIM900
  SIM900.begin(taxaDeTransmissao);
  // Delay para fazer logon na rede
  delay(2000);
  //Serial.println("Comunicação Arduino <-> SIM900, estabelecida");
  digitalWrite(buzzer, HIGH);
  delay(1);//wait for 1ms
  digitalWrite(buzzer, LOW);
  delay(1);//wait for 1ms

};

void loop() {
  // Atualizando o módulo
  mpu6050.update();

  // Armazenando informações do Gíroscópio, acelerômetro e ângulo
  gyrox = mpu6050.getGyroX();
  gyroy = mpu6050.getGyroY();
  gyroz = mpu6050.getGyroZ();

  angx = mpu6050.getAngleX();
  angy = mpu6050.getAngleY();

  // Exibindo informações no serial
  //Serial.print("gyro. x, y, z:\t");
  Serial.print(gyrox); Serial.print("\t");
  Serial.print(gyroy); Serial.print("\t");
  Serial.print(gyroz); Serial.println("\t");

  Serial.print("ang. x, y:\t");
  Serial.print(angx); Serial.print("\t");
  Serial.print(angy); Serial.println("\t");

  // Chamada da função de detecção de queda
  queda(gyrox, gyroy, gyroz, angx, angy);

};

// Detecção de queda
void queda (float gyrox, float gyroy, float  gyroz, float  angx, float angy) {
  // Variáveis locais
  float gyroxAbs, gyroyAbs, gyrozAbs;

  float MovimentoBrusco = 300;
  int direitax = -20, direitay = 60;
  int esquerdax = 25, esquerday = 60;
  int frente = 120, tras = 40;

  bool aviso1 = false, aviso2 = false ;

  // Detectando movimento brusco
  gyroxAbs = abs(gyrox);
  gyroyAbs = abs(gyroy);
  gyrozAbs = abs(gyroz);

  if ((gyroxAbs > MovimentoBrusco) || (gyroyAbs > MovimentoBrusco) || (gyrozAbs > MovimentoBrusco) ) {
    Serial.println("========================================================================> Movimento brusco ");
    aviso1 = true;
  }
  else {
    aviso1 = false;
  };

  // Detectando inclinação
  if (angx >= frente) {
    Serial.println("========================================================================> frente");
    aviso2 = true;
  }
  else if (angx <= tras) {
    Serial.println("========================================================================> trás");
    aviso2 = true;
  }
  else if ((angx >= esquerdax) && (angy <= esquerday)) {
    Serial.println("========================================================================> esquerda");
    aviso2 = true;
  }
  else if ((angx <= direitax) && (angy <= direitay)) {
    Serial.println("========================================================================> direita");
    aviso2 = true;
  }
  else {
    aviso2 = false;
  };

  // Aviso
  if ((aviso1 == true) && (aviso2 == true)) {
    Serial.println("========================================================================> Perigo");
    digitalWrite(pinoLED, HIGH);
    som();
    //sms();
  }
  else {
    digitalWrite(pinoLED, LOW);
  };
};

// Enviando SMS
void sms() {
  // Comando AT para definir o SIM900 no modo SMS
  SIM900.print("AT+CMGF=1\r");
  delay(100);

  // Configurando para o número do chip inserido no sistema, sempre no formato internacional
  SIM900.println("AT + CMGS = \"+XXXXXXXXXXXX\"");
  delay(100);

  // Conteúdo do SMS
  SIM900.println("");
  delay(100);

  // Finalizando o comando AT com um ^Z, código ASCII 26
  //SIM900.println(char(26));
  SIM900.println((char)26);
  delay(100);
  SIM900.println();
  // Tempo para o módulo enviar o SMS
  delay(5000);
};

// Ativar o buzzer
void som() {
  unsigned char i;
  //output an frequency
  for (i = 0; i < 80; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(1);//wait for 1ms
    digitalWrite(buzzer, LOW);
    delay(1);//wait for 1ms
  }
  //output another frequency
  for (i = 0; i < 100; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(2);//wait for 2ms
    digitalWrite(buzzer, LOW);
    delay(2);//wait for 2ms
  }
}
