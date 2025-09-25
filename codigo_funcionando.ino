// Arquivo: Arduino_Escravo_Final.ino

#include <Servo.h>

#define PINO_SERVO 6
#define PINO_LDR_ESQUERDO A0
#define PINO_LDR_DIREITO A1

Servo meuServo;
int posicaoServoAtual = 90;

void setup() {
  Serial.begin(9600);
  meuServo.attach(PINO_SERVO);
  meuServo.write(posicaoServoAtual);
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando.startsWith("P")) {
      int novaPosicao = comando.substring(1).toInt();
      posicaoServoAtual = constrain(novaPosicao, 0, 180);
      meuServo.write(posicaoServoAtual);
    }
  }

  int leituraLDR_L = analogRead(PINO_LDR_ESQUERDO);
  int leituraLDR_R = analogRead(PINO_LDR_DIREITO);

  Serial.print("LDR_L:");
  Serial.print(leituraLDR_L);
  Serial.print(",LDR_R:");
  Serial.print(leituraLDR_R);
  Serial.print(",POS:");
  Serial.println(posicaoServoAtual);

  delay(100);
}