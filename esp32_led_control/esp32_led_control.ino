/*
 * Controle de LED por Comando de Voz via WiFi
 * ESP32 - Servidor Web para receber comandos
 * 
 * Este código cria um servidor web no ESP32 que recebe
 * comandos para ligar e desligar um LED via requisições HTTP
 */

#include <WiFi.h>
#include <WebServer.h>

// ============ CONFIGURAÇÕES - ALTERE AQUI ============
const char* ssid = "Lidia_2G";         // Nome da sua rede WiFi
const char* password = "vfn4i83123";     // Senha da sua rede WiFi
// =====================================================

// Pino do LED (LED embutido do ESP32 ou LED externo)
const int LED_PIN = 2;  // GPIO2 é o LED embutido na maioria dos ESP32

// Cria o servidor web na porta 80
WebServer server(80);

// Variável para armazenar o estado do LED
bool ledState = false;

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);
  Serial.println("\n\n=== Controle de LED por Voz ===");
  
  // Configura o pino do LED como saída
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Conecta ao WiFi
  connectToWiFi();
  
  // Configura as rotas do servidor
  setupServerRoutes();
  
  // Inicia o servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado!");
  Serial.println("\n>>> Use o IP acima na aplicação web <<<\n");
}

void loop() {
  // Processa as requisições do servidor
  server.handleClient();
}

void connectToWiFi() {
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  
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
  }
}

void setupServerRoutes() {
  // Rota principal - retorna o estado do LED
  server.on("/", HTTP_GET, []() {
    sendCORSHeaders();
    String html = "<h1>ESP32 LED Control</h1>";
    html += "<p>LED está: " + String(ledState ? "LIGADO" : "DESLIGADO") + "</p>";
    html += "<p>Use a aplicação web para controlar por voz!</p>";
    server.send(200, "text/html", html);
  });
  
  // Rota para ligar o LED
  server.on("/ligar", HTTP_GET, []() {
    sendCORSHeaders();
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    Serial.println(">>> LED LIGADO <<<");
    server.send(200, "application/json", "{\"status\":\"success\",\"led\":\"on\",\"message\":\"LED ligado com sucesso!\"}");
  });
  
  // Rota para desligar o LED
  server.on("/desligar", HTTP_GET, []() {
    sendCORSHeaders();
    digitalWrite(LED_PIN, LOW);
    ledState = false;
    Serial.println(">>> LED DESLIGADO <<<");
    server.send(200, "application/json", "{\"status\":\"success\",\"led\":\"off\",\"message\":\"LED desligado com sucesso!\"}");
  });
  
  // Rota para verificar o estado do LED
  server.on("/estado", HTTP_GET, []() {
    sendCORSHeaders();
    String state = ledState ? "on" : "off";
    server.send(200, "application/json", "{\"status\":\"success\",\"led\":\"" + state + "\"}");
  });
  
  // Rota para toggle (inverter estado)
  server.on("/toggle", HTTP_GET, []() {
    sendCORSHeaders();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    String state = ledState ? "on" : "off";
    Serial.println(">>> LED TOGGLE: " + String(ledState ? "LIGADO" : "DESLIGADO") + " <<<");
    server.send(200, "application/json", "{\"status\":\"success\",\"led\":\"" + state + "\"}");
  });
  
  // Handler para requisições OPTIONS (CORS preflight)
  server.on("/ligar", HTTP_OPTIONS, handleCORS);
  server.on("/desligar", HTTP_OPTIONS, handleCORS);
  server.on("/estado", HTTP_OPTIONS, handleCORS);
  server.on("/toggle", HTTP_OPTIONS, handleCORS);
}

void sendCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleCORS() {
  sendCORSHeaders();
  server.send(204);
}
