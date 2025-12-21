# 🎙️ Controle de LED por Comando de Voz com ESP32

Sistema completo para controlar um LED usando comandos de voz através de uma aplicação web que se comunica com o ESP32 via **MQTT na nuvem**!

🌍 **Controle seu LED de qualquer lugar do mundo!**

## 📁 Estrutura do Projeto

```
├── esp32_led_mqtt/              # 🆕 VERSÃO MQTT (Recomendada)
│   └── esp32_led_mqtt.ino       # Código ESP32 com MQTT
├── web_app_mqtt/                # 🆕 VERSÃO MQTT (Recomendada)
│   └── index.html               # Aplicação web com MQTT
├── esp32_led_control/           # Versão HTTP (apenas rede local)
│   └── esp32_led_control.ino    
├── web_app/                     # Versão HTTP (apenas rede local)
│   └── index.html               
└── README.md                    
```

## 🚀 Qual versão usar?

| Versão | Quando usar |
|--------|-------------|
| **MQTT** (Recomendada) | Controlar de qualquer lugar (internet) |
| HTTP | Apenas na mesma rede local |

## 🔧 Requisitos

### Hardware
- ESP32 (qualquer modelo)
- LED (pode usar o LED embutido no GPIO2)
- Cabo USB para programação

### Software
- Arduino IDE (com suporte ao ESP32)
- **Biblioteca PubSubClient** (para MQTT)
- Navegador Google Chrome (para reconhecimento de voz)

---

## 📦 Instalação - Versão MQTT (Recomendada)

### 1. Instalar a Biblioteca PubSubClient

1. Na Arduino IDE, vá em `Sketch > Incluir Biblioteca > Gerenciar Bibliotecas`
2. Pesquise por **"PubSubClient"**
3. Instale a biblioteca de **Nick O'Leary**

### 2. Configurar o ESP32

1. **Instale o suporte ao ESP32 na Arduino IDE:**
   - Vá em `Arquivo > Preferências`
   - Em "URLs Adicionais de Gerenciadores de Placas", adicione:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Vá em `Ferramentas > Placa > Gerenciador de Placas`
   - Procure por "esp32" e instale

2. **Abra o arquivo `esp32_led_mqtt/esp32_led_mqtt.ino`**

3. **Configure suas credenciais WiFi:**
   ```cpp
   const char* ssid = "SEU_WIFI_AQUI";         // Nome da sua rede WiFi
   const char* password = "SUA_SENHA_AQUI";    // Senha da sua rede WiFi
   ```

4. **Selecione a placa ESP32:**
   - `Ferramentas > Placa > ESP32 Arduino > ESP32 Dev Module`

5. **Faça o upload do código para o ESP32**

6. **Abra o Monitor Serial (115200 baud)** para ver se conectou ao MQTT

### 3. Usar a Aplicação Web

1. **Acesse:** https://voz-nine.vercel.app (ou publique `web_app_mqtt/index.html`)

2. **Aguarde conectar** ao broker MQTT (indicador verde)

3. **Clique em "Iniciar Reconhecimento de Voz"**

4. **Fale os comandos!**

## 🎤 Comandos de Voz Disponíveis

| Para Ligar | Para Desligar |
|------------|---------------|
| "Ligar LED" | "Desligar LED" |
| "Ligar luz" | "Desligar luz" |
| "Acender" | "Apagar" |
| "Liga" | "Desliga" |

## 📡 Como funciona o MQTT

```
┌─────────────┐       ┌──────────────────┐       ┌─────────────┐
│  Aplicação  │──────>│  Broker MQTT     │<──────│   ESP32     │
│    Web      │       │  (HiveMQ Cloud)  │       │   + LED     │
│  (Vercel)   │<──────│  broker.hivemq   │──────>│             │
└─────────────┘       └──────────────────┘       └─────────────┘
     🎤 Voz              ☁️ Nuvem                  💡 LED
```

### Tópicos MQTT utilizados:
| Tópico | Direção | Descrição |
|--------|---------|-----------|
| `labmaker/led/comando` | Web → ESP32 | Envia comandos (ligar/desligar) |
| `labmaker/led/estado` | ESP32 → Web | Retorna estado atual (on/off) |

## 🔌 Esquema de Conexão (LED Externo)

Se quiser usar um LED externo em vez do LED embutido:

```
ESP32 GPIO2 ----[Resistor 220Ω]----[LED (+)]----[LED (-)]---- GND
```

Para usar outro GPIO, altere a linha no código:
```cpp
const int LED_PIN = 2;  // Altere para o GPIO desejado
```

## ⚠️ Solução de Problemas

### O reconhecimento de voz não funciona
- Use o navegador **Google Chrome**
- Permita o acesso ao microfone quando solicitado
- Verifique se está usando **HTTPS** (Vercel já usa)

### ESP32 não conecta ao MQTT
- Verifique se o WiFi está correto
- Verifique no Monitor Serial se conectou ao broker
- O broker `broker.hivemq.com` é gratuito e público

### LED não acende
- Verifique se está usando o GPIO correto
- Teste com o LED embutido (GPIO2) primeiro
- Verifique a polaridade do LED externo

### Aplicação web não conecta
- Aguarde alguns segundos para conectar ao broker
- Verifique se o indicador ficou verde
- Tente recarregar a página

## 📱 Acesso pelo Celular

A aplicação web também funciona em celulares:
1. Abra o Chrome no celular
2. Acesse o arquivo HTML (pode hospedar em um servidor local)
3. O reconhecimento de voz funciona normalmente

## 🚀 Melhorias Futuras

- [ ] Adicionar mais dispositivos (múltiplos LEDs)
- [ ] Controle de intensidade (PWM)
- [ ] Histórico de comandos
- [ ] Integração com assistentes virtuais
- [ ] App mobile nativo

## 📄 Licença

Este projeto é livre para uso educacional e pessoal.

---

Desenvolvido para o **GO LabMaker - IFG Câmpus Goiânia Oeste** 🎓
