/*
 * Dashboard IoT - Controle por Voz via MQTT
 * ESP32 + LED + Sensor DHT11 (Temperatura e Umidade)
 * 
 * Este código conecta o ESP32 a um broker MQTT na nuvem (HiveMQ)
 * permitindo controle e monitoramento de qualquer lugar do mundo!
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ============ CONFIGURAÇÕES WiFi - ALTERE AQUI ============
const char* ssid = "Lidia_2G";              // Nome da sua rede WiFi
const char* password = "vfn4i83123";         // Senha da sua rede WiFi
// ==========================================================

// ============ CONFIGURAÇÕES MQTT - HiveMQ (Gratuito) ============
const char* mqtt_server = "broker.hivemq.com";  // Broker público gratuito
const int mqtt_port = 1883;

// Tópicos MQTT
const char* mqtt_topic_comando = "labmaker/led/comando";       // Comandos do LED
const char* mqtt_topic_led_estado = "labmaker/led/estado";     // Estado do LED
const char* mqtt_topic_temperatura = "labmaker/sensor/temperatura";  // Temperatura
const char* mqtt_topic_umidade = "labmaker/sensor/umidade";    // Umidade
const char* mqtt_topic_sensores = "labmaker/sensor/dados";     // Dados combinados (JSON)

const char* mqtt_client_id = "ESP32_LabMaker_001";             // ID único do dispositivo
// ================================================================

// ============ CONFIGURAÇÕES DOS PINOS ============
const int LED_PIN = 2;      // GPIO2 - LED embutido
const int DHT_PIN = 4;      // GPIO4 - Sensor DHT11
// =================================================

// Configuração do DHT11
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Objetos WiFi e MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variáveis de estado
bool ledState = false;
float temperatura = 0;
float umidade = 0;

// Variáveis para temporização
unsigned long lastReconnectAttempt = 0;
unsigned long lastSensorRead = 0;
const long reconnectInterval = 5000;
const long sensorInterval = 5000;  // Ler sensor a cada 5 segundos

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("  Dashboard IoT - ESP32 + LED + DHT11");
  Serial.println("========================================\n");
  
  // Configura o pino do LED como saída
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Inicializa o sensor DHT11
  dht.begin();
  Serial.println("✓ Sensor DHT11 inicializado (GPIO4)");
  
  // Conecta ao WiFi
  connectToWiFi();
  
  // Configura o servidor MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  // Conecta ao broker MQTT
  connectToMQTT();
  
  // Primeira leitura do sensor
  readSensor();
  
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
  
  // Lê o sensor periodicamente
  unsigned long now = millis();
  if (now - lastSensorRead > sensorInterval) {
    lastSensorRead = now;
    readSensor();
  }
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
    
    // Publica estado atual do LED
    publishLedState();
    
    // Publica dados do sensor
    publishSensorData();
    
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
    publishLedState();
    publishSensorData();
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
  publishLedState();
}

void desligarLED() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println(">>> LED DESLIGADO <<<");
  publishLedState();
}

void publishLedState() {
  String state = ledState ? "on" : "off";
  mqttClient.publish(mqtt_topic_led_estado, state.c_str(), true);  // retained = true
  Serial.print("Estado do LED publicado: ");
  Serial.println(state);
}

// ============ FUNÇÕES DO SENSOR DHT11 ============

void readSensor() {
  // Lê temperatura e umidade
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();
  
  // Verifica se a leitura é válida
  if (isnan(newTemp) || isnan(newHum)) {
    Serial.println("✗ Erro ao ler o sensor DHT11!");
    return;
  }
  
  temperatura = newTemp;
  umidade = newHum;
  
  // Mostra no Monitor Serial
  Serial.println("\n----- Leitura do Sensor -----");
  Serial.print("🌡️  Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("💧 Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");
  Serial.println("-----------------------------");
  
  // Publica via MQTT
  publishSensorData();
}

void publishSensorData() {
  if (mqttClient.connected()) {
    // Publica temperatura
    char tempStr[10];
    dtostrf(temperatura, 4, 1, tempStr);
    mqttClient.publish(mqtt_topic_temperatura, tempStr, true);
    
    // Publica umidade
    char humStr[10];
    dtostrf(umidade, 4, 1, humStr);
    mqttClient.publish(mqtt_topic_umidade, humStr, true);
    
    // Publica dados combinados em JSON
    char jsonBuffer[100];
    snprintf(jsonBuffer, sizeof(jsonBuffer), 
             "{\"temperatura\":%.1f,\"umidade\":%.1f,\"led\":\"%s\"}", 
             temperatura, umidade, ledState ? "on" : "off");
    mqttClient.publish(mqtt_topic_sensores, jsonBuffer, true);
    
    Serial.println("✓ Dados do sensor publicados via MQTT");
  }
}
