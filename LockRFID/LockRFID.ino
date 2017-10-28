#include <SPI.h>
#include <MFRC522.h>
// o pino onde a entrada de energia dos leds está plugada
#define LED_VERDE 40 
#define LED_AMARELO 41
#define LED_VERMELHO 42

#define DELAY 5000 // quanto tempo esperar antes de voltar a ler um novo ID
#define N_AUTORIZADOS 2 // numero de tags cadastradas no sistema como tags autorizadas

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
  
String lista_autorizados[N_AUTORIZADOS] = { // esta é a lista de tags autorizados
                              "34 76 B1 EB", // O chaveiro
                              "AC 2A ED 75" // O card
                              };
void setup() {
  Serial.begin(9600); // Inicia a serial
  SPI.begin();    // Inicia  SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522
  
  Serial.println("Aproxime o seu cartão/chaveiro do leitor...");
  Serial.println();

  //Preparando os leds para acender
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);

  checarReceptor();
}
 
void loop() {
  // Emquanto não encontrar novos cartões
  digitalWrite(LED_AMARELO, HIGH); // iniciando leitura
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  
  // Caso não tenha conseguido ler
  if(!mfrc522.PICC_ReadCardSerial()) return;
  
  //Mostra UID na serial
  Serial.print("ID da tag: ");
  String conteudo = ""; // essa string vai receber todo o conteudo da tag RFID
  // este loop recebe todo conteudo do cartão/chaveiro
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  conteudo.toUpperCase(); // apenas para deixar mais apresentável
  conteudo = conteudo.substring(1); // removendo o primeiro caractere que é inútil
  Serial.println(conteudo); // mostrando o que leu

  if (checarAutorizacao(conteudo)) { // Caso esteja na lista de autorizados
    autorizar();
  }
  else { // ID não reconhecido
    negar();
  }
}

/*
  Essa função busca se o ID passado está na lista de autorizados
*/
bool checarAutorizacao(String ID) {
  for(int i = 0; i < N_AUTORIZADOS; i++) // Percorrendo a lista completa
    if(ID == lista_autorizados[i]) // Checando se o ID é igual ao item da lista
      return true; // O ID passado está presente na lista
  // Terminou o loop e não encontrou na lista então deve retornar false
  return false;
}

/*
  Essa função deve ser executada quando o id lido for correto
*/
void autorizar() {
    Serial.println("Acesso permitido !");
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    delay(DELAY); // espera 5 segundos
    digitalWrite(LED_VERDE, LOW);
    Serial.println();
}

/*
  Essa função deve ser executada quando o id lido for INcorreto
*/
void negar() {
   Serial.println("Acesso Negado !");
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    delay(DELAY); // espera 5 segundos
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println();
}

/*
  Essa função checa se o receptor está bem conectado e trava o sistema caso não esteja
*/
void checarReceptor() {

  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println("Falha na conexão com o Receptor");
    Serial.println("Verifique a conexão e reinicie o arduino");

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    while (true); // travar
  }
}

