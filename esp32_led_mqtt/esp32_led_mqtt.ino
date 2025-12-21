/*
 * Controle de LED por Comando de Voz via MQTT
 * ESP32 - Cliente MQTT para receber comandos da nuvem
 * 
 * Este código conecta o ESP32 a um broker MQTT na nuvem (HiveMQ)
 * permitindo controle do LED de qualquer lugar do mundo!
 */

#include <WiFi.h>
#include <PubSubClient.h>

// ============ CONFIGURAÇÕES WiFi - ALTERE AQUI ============
const char* ssid = "Lidia_2G";              // Nome da sua rede WiFi
const char* password = "vfn4i83123";         // Senha da sua rede WiFi
// ==========================================================

// ============ CONFIGURAÇÕES MQTT - HiveMQ (Gratuito) ============
const char* mqtt_server = "broker.hivemq.com";  // Broker público gratuito
const int mqtt_port = 1883;
const char* mqtt_topic_comando = "labmaker/led/comando";   // Tópico para receber comandos
const char* mqtt_topic_estado = "labmaker/led/estado";     // Tópico para enviar estado
const char* mqtt_client_id = "ESP32_LabMaker_001";         // ID único do dispositivo

// IMPORTANTE: Para um projeto real, use um ID único! 
// Você pode mudar "labmaker" para algo único seu, ex: "davitaveira/led/comando"
// ================================================================

// Pino do LED (LED embutido do ESP32 ou LED externo)
const int LED_PIN = 2;  // GPIO2 é o LED embutido na maioria dos ESP32

// Objetos WiFi e MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variável para armazenar o estado do LED
bool ledState = false;

// Variáveis para reconexão
unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 5000;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("  Controle de LED por Voz via MQTT");
  Serial.println("========================================\n");
  
  // Configura o pino do LED como saída
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Conecta ao WiFi
  connectToWiFi();
  
  // Configura o servidor MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  // Conecta ao broker MQTT
  connectToMQTT();
  
  Serial.println("\n>>> Sistema pronto! Aguardando comandos... <<<\n");
}

void loop() {
  // Mantém a conexão MQTT ativa
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      Serial.println("MQTT desconectado. Tentando reconectar...");
      connectToMQTT();
    }
  }
  
  // Processa mensagens MQTT
  mqttClient.loop();
}

// ============ FUNÇÕES DE CONEXÃO ============

void connectToWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ Falha ao conectar ao WiFi!");
    Serial.println("Verifique o SSID e a senha.");
    Serial.println("Reiniciando em 5 segundos...");
    delay(5000);
    ESP.restart();
  }
}

void connectToMQTT() {
  Serial.print("Conectando ao broker MQTT: ");
  Serial.println(mqtt_server);
  
  if (mqttClient.connect(mqtt_client_id)) {
    Serial.println("✓ Conectado ao broker MQTT!");
    
    // Inscreve-se no tópico de comandos
    mqttClient.subscribe(mqtt_topic_comando);
    Serial.print("Inscrito no tópico: ");
    Serial.println(mqtt_topic_comando);
    
    // Publica estado atual
    publishState();
    
  } else {
    Serial.print("✗ Falha na conexão MQTT. Código: ");
    Serial.println(mqttClient.state());
    Serial.println("Códigos: -4=timeout, -3=connection lost, -2=connect failed");
    Serial.println("         -1=disconnected, 0=connected");
  }
}

// ============ CALLBACK MQTT - RECEBE COMANDOS ============

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Converte payload para string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println("\n----- Mensagem Recebida -----");
  Serial.print("Tópico: ");
  Serial.println(topic);
  Serial.print("Comando: ");
  Serial.println(message);
  Serial.println("-----------------------------");
  
  // Processa o comando
  if (message == "ligar" || message == "on" || message == "1") {
    ligarLED();
  } 
  else if (message == "desligar" || message == "off" || message == "0") {
    desligarLED();
  }
  else if (message == "toggle") {
    if (ledState) {
      desligarLED();
    } else {
      ligarLED();
    }
  }
  else if (message == "estado" || message == "status") {
    publishState();
  }
  else {
    Serial.println("Comando não reconhecido!");
  }
}

// ============ FUNÇÕES DO LED ============

void ligarLED() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  Serial.println(">>> LED LIGADO <<<");
  publishState();
}

void desligarLED() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println(">>> LED DESLIGADO <<<");
  publishState();
}

void publishState() {
  String state = ledState ? "on" : "off";
  mqttClient.publish(mqtt_topic_estado, state.c_str(), true);  // retained = true
  Serial.print("Estado publicado: ");
  Serial.println(state);
}
