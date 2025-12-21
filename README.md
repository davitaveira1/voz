# 🎙️ Controle de LED por Comando de Voz com ESP32

Sistema completo para controlar um LED usando comandos de voz através de uma aplicação web que se comunica com o ESP32 via WiFi.

## 📁 Estrutura do Projeto

```
├── esp32_led_control/
│   └── esp32_led_control.ino    # Código do ESP32
├── web_app/
│   └── index.html               # Aplicação web com reconhecimento de voz
└── README.md                    # Este arquivo
```

## 🔧 Requisitos

### Hardware
- ESP32 (qualquer modelo)
- LED (pode usar o LED embutido no GPIO2)
- Cabo USB para programação

### Software
- Arduino IDE (com suporte ao ESP32)
- Navegador Google Chrome (para reconhecimento de voz)
- Computador e ESP32 na mesma rede WiFi

## 📦 Instalação

### 1. Configurar o ESP32

1. **Instale o suporte ao ESP32 na Arduino IDE:**
   - Vá em `Arquivo > Preferências`
   - Em "URLs Adicionais de Gerenciadores de Placas", adicione:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Vá em `Ferramentas > Placa > Gerenciador de Placas`
   - Procure por "esp32" e instale

2. **Abra o arquivo `esp32_led_control/esp32_led_control.ino`**

3. **Configure suas credenciais WiFi:**
   ```cpp
   const char* ssid = "SEU_WIFI_AQUI";         // Nome da sua rede WiFi
   const char* password = "SUA_SENHA_AQUI";    // Senha da sua rede WiFi
   ```

4. **Selecione a placa ESP32:**
   - `Ferramentas > Placa > ESP32 Arduino > ESP32 Dev Module`

5. **Faça o upload do código para o ESP32**

6. **Abra o Monitor Serial (115200 baud)** para ver o IP do ESP32

### 2. Usar a Aplicação Web

1. **Abra o arquivo `web_app/index.html`** no Google Chrome

2. **Digite o IP do ESP32** mostrado no Monitor Serial

3. **Clique em "Iniciar Reconhecimento de Voz"**

4. **Fale os comandos!**

## 🎤 Comandos de Voz Disponíveis

| Para Ligar | Para Desligar |
|------------|---------------|
| "Ligar LED" | "Desligar LED" |
| "Ligar luz" | "Desligar luz" |
| "Acender" | "Apagar" |
| "Liga" | "Desliga" |

## 🌐 API do ESP32

O ESP32 expõe as seguintes rotas HTTP:

| Rota | Método | Descrição |
|------|--------|-----------|
| `/` | GET | Página inicial com status |
| `/ligar` | GET | Liga o LED |
| `/desligar` | GET | Desliga o LED |
| `/estado` | GET | Retorna o estado atual do LED |
| `/toggle` | GET | Inverte o estado do LED |

### Exemplo de Resposta JSON

```json
{
  "status": "success",
  "led": "on",
  "message": "LED ligado com sucesso!"
}
```

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
- Verifique se está usando HTTPS ou localhost

### Não consigo conectar ao ESP32
- Verifique se o ESP32 está na mesma rede WiFi
- Confirme o IP no Monitor Serial
- Desative temporariamente o firewall para teste

### CORS Error
- O código do ESP32 já inclui headers CORS
- Se ainda houver problemas, tente abrir o HTML diretamente pelo Chrome

### LED não acende
- Verifique se está usando o GPIO correto
- Teste com o LED embutido (GPIO2) primeiro
- Verifique a polaridade do LED externo

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
