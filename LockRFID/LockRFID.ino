#include <SPI.h>
#include <MFRC522.h>
// o pino onde a entrada de energia dos leds está plugada
#define LED_VERDE 40
#define LED_AMARELO 41
#define LED_VERMELHO 42

#define DELAY 5000 // quanto tempo esperar antes de voltar a ler um novo ID
#define DEBUG_MODE 1 // quando ativo simula cards sendo lidos a todo momento

// Os pinos SDA e RST do receptor
#define SS_PIN 53
#define RST_PIN 9
/*
  Os pinos abaixo obrigatoriamente tem que estar nessas posições no caso do Arduino MEGA
  SCK -> 52
  MOSI  -> 51
  MISO  -> 50
*/
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Criando a instancia da classe que controla o receptor
 
void setup() {
  Serial.begin(9600); // Inicia a serial
  SPI.begin();    // Inicia  SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522

  //Preparando os leds para acender
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  testar_leds();
}
 
void loop() {
  checarReceptor();
  String conteudo = ""; // essa string vai receber todo o conteudo da tag RFID
  
  if(!DEBUG_MODE) {
    // Emquanto não encontrar novos cartões
    digitalWrite(LED_AMARELO, HIGH); // iniciando leitura
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    
    // Caso não tenha conseguido ler
    if(!mfrc522.PICC_ReadCardSerial()) return;
    
    // este loop recebe todo conteudo do cartão/chaveiro
    for (byte i = 0; i < mfrc522.uid.size; i++) {
       conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    conteudo.toUpperCase(); // apenas para deixar mais apresentável
    conteudo = conteudo.substring(1); // removendo o primeiro caractere que é inútil
  }
  else {
    delay(1000);
    conteudo = "XX XX XX XX";
  }
  if (checarAutorizacao(conteudo)) // Caso esteja na lista de autorizados
    autorizar();
  else // ID não reconhecido
    negar();
}

/*
  Essa função busca se o ID passado está na lista de autorizados
*/
bool checarAutorizacao(String ID) {
  char leitura; // Variavel que receberá os valores enviados pelo programa em python
  
  Serial.println(ID); // enviado a tag lida

  while(Serial.available() <= 0); // aguardando por tempo indeterminado
  leitura = Serial.read();  // lendo a resposta
 
   if(leitura == 'S') return true;
 
  return false;
}

/*
  Essa função deve ser executada quando o id lido for correto
*/
void autorizar() {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    delay(DELAY);
    digitalWrite(LED_VERDE, LOW);
}

/*
  Essa função deve ser executada quando o id lido for INcorreto
*/
void negar() {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    delay(DELAY);
    digitalWrite(LED_VERMELHO, LOW);
}

/*
  Essa função checa se o receptor está bem conectado e trava o sistema caso não esteja
*/
void checarReceptor() {

  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  
  if ((v == 0x00) || (v == 0xFF)) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    while ((v == 0x00) || (v == 0xFF)) { // travar
      digitalWrite(LED_VERMELHO, HIGH);
      delay(500);
      digitalWrite(LED_VERMELHO, LOW);
      delay(500);
      v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg); // checar de novo
    }
  }
}
/*
  Esta função é apenas para debug e saber se os leds estão be conectados
*/
void testar_leds() {
   digitalWrite(LED_VERDE, LOW);
   digitalWrite(LED_AMARELO, LOW);
   digitalWrite(LED_VERMELHO, LOW);

   digitalWrite(LED_VERDE, HIGH);
   delay(500);
   digitalWrite(LED_AMARELO, HIGH);
   delay(500);
   digitalWrite(LED_VERMELHO, HIGH);
   delay(500);

   digitalWrite(LED_VERDE, LOW);
   digitalWrite(LED_AMARELO, LOW);
   digitalWrite(LED_VERMELHO, LOW);
}

