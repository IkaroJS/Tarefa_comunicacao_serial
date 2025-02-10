# Projeto de Controle de LEDs, Botões e Display SSD1306 com Comunicação Serial

Este projeto foi desenvolvido como parte de uma tarefa prática para consolidar os conceitos sobre o uso de interfaces de comunicação serial no microcontrolador RP2040, especificamente utilizando a placa de desenvolvimento BitDogLab. O objetivo é aplicar o conhecimento adquirido sobre comunicação serial, manipulação de LEDs (comuns e endereçáveis WS2812), controle de botões com interrupções e debouncing, além de trabalhar com um display SSD1306 via I2C.

## Objetivos

- **Compreensão de UART e I2C:** Aplicar os conceitos de comunicação serial em microcontroladores, especificamente utilizando UART e I2C.
- **Controle de LEDs:** Manipular LEDs comuns e LEDs endereçáveis WS2812.
- **Uso de Botões e Interrupções:** Implementar o uso de botões com interrupções, incluindo o tratamento de debounce.
- **Exibição no Display SSD1306:** Exibir caracteres digitados via Serial Monitor no display SSD1306.
- **Desenvolvimento de um Projeto Funcional:** Combinar o hardware e o software de maneira funcional, interligando todos os componentes.

## Componentes Utilizados

- **Matriz 5x5 de LEDs WS2812:** Conectada à GPIO 7, utilizada para exibir números de 0 a 9.
- **LED RGB:** Conectado às GPIOs 11, 12 e 13, utilizado para indicar o estado dos botões.
- **Botão A:** Conectado à GPIO 5, utilizado para controlar o LED verde.
- **Botão B:** Conectado à GPIO 6, utilizado para controlar o LED azul.
- **Display SSD1306:** Conectado via I2C nas GPIOs 14 (SDA) e 15 (SCL), utilizado para exibir mensagens e caracteres.

## Funcionalidades Implementadas

### 1. **Modificação da Biblioteca `font.h`**

A biblioteca `font.h` foi modificada para incluir caracteres minúsculos, garantindo a exibição correta de caracteres em maiúsculas e minúsculas no display SSD1306.

### 2. **Entrada de Caracteres via PC (Serial Monitor)**

O projeto permite que caracteres sejam enviados via Serial Monitor no VS Code e exibidos no display SSD1306. Além disso, se um número entre 0 e 9 for digitado, um símbolo correspondente será exibido na matriz 5x5 WS2812.

### 3. **Interação com o Botão A**

- **Função:** Pressionar o botão A alterna o estado do LED RGB verde (ligado/desligado).
- **Exibição:** Uma mensagem informativa sobre o estado do LED é exibida no display SSD1306 e enviada ao Serial Monitor.

### 4. **Interação com o Botão B**

- **Função:** Pressionar o botão B alterna o estado do LED RGB azul (ligado/desligado).
- **Exibição:** Uma mensagem informativa sobre o estado do LED é exibida no display SSD1306 e enviada ao Serial Monitor.

## Implementação de Interrupções e Debouncing

- **Botões A e B:** O código utiliza interrupções (IRQ) para detectar o pressionamento dos botões. O tratamento de debouncing foi implementado via software para garantir que múltiplos acionamentos rápidos não causem falhas de leitura.
  
## Comunicação Serial via UART

- O projeto utiliza a UART para enviar mensagens informativas ao Serial Monitor, registrando as operações com os botões e exibindo as ações no display.

## Organização do Código

O código foi estruturado de forma modular, com funções específicas para cada funcionalidade:

- **`init_pins()`**: Configuração dos pinos para LEDs e botões.
- **`gpio_callback()`**: Função de interrupção para o controle dos botões.
- **`init_display()`**: Inicialização e configuração do display SSD1306.
- **`enviar_uart()`**: Envio de mensagens pela UART.
- **`desenhar_numero()`**: Exibição de números na matriz de LEDs WS2812.

## Requisitos

- **Hardware:**
  - Placa de desenvolvimento BitDogLab com RP2040.
  - Matriz 5x5 de LEDs WS2812.
  - LEDs RGB.
  - Botões A e B.
  - Display SSD1306.

- **Software:**
  - Raspberry Pi Pico SDK.
  - VS Code com extensões para desenvolvimento em C.

## Como Compilar e Carregar o Código

### 1. **Configuração do Ambiente de Desenvolvimento**

Antes de compilar e carregar o código no RP2040, você precisa configurar seu ambiente de desenvolvimento. Siga os passos abaixo:

1. **Instalar o Raspberry Pi Pico SDK:**
   - Baixe e instale o **Raspberry Pi Pico SDK** no seu computador. Você pode seguir as instruções oficiais [aqui](https://www.raspberrypi.org/documentation/rp2040/getting-started/).

2. **Instalar o CMake:**
   - O **CMake** é necessário para compilar o código. Instale-o em seu sistema, se ainda não o tiver. As instruções de instalação podem ser encontradas [aqui](https://cmake.org/install/).

3. **Configurar o VS Code:**
   - Instale o [VS Code](https://code.visualstudio.com/) e a extensão de **C/C++**.
   - Instale também a extensão **CMake Tools** para facilitar a compilação e o gerenciamento do projeto.

4. **Baixar o Repositório:**
   - Clone este repositório para o seu diretório de trabalho usando o comando:
     ```bash
     git clone https://github.com/IkaroJS/Tarefa_comunicacao_serial.git
     ```

5. **Configurar o ambiente CMake:**
   - Abra o VS Code na pasta do projeto.
   - O arquivo `CMakeLists.txt` já está configurado para o Raspberry Pi Pico SDK. No entanto, você pode precisar configurar o caminho do SDK nas configurações do CMake, caso o VS Code não detecte automaticamente.

### 2. **Compilando o Código**

Para compilar o código, siga os seguintes passos:

1. **Abra o terminal integrado do VS Code:**
   - No VS Code, vá até o menu **Terminal** e selecione **Novo Terminal**.

2. **Criar a pasta de build:**
   - No terminal, crie uma pasta de build dentro do diretório do projeto:
     ```bash
     mkdir build
     cd build
     ```

3. **Gerar os arquivos de compilação:**
   - No terminal dentro da pasta `build`, execute o comando CMake para gerar os arquivos de compilação:
     ```bash
     cmake ..
     ```

4. **Compilar o código:**
   - Após a geração dos arquivos de compilação, compile o código com o comando:
     ```bash
     make
     ```

   Se tudo estiver configurado corretamente, o processo de compilação será concluído com sucesso, e o arquivo executável para o RP2040 será gerado.

### 3. **Carregando o Código na Placa**

1. **Conectar a placa RP2040 ao computador:**
   - Conecte a sua placa BitDogLab (RP2040) ao computador usando um cabo USB.

2. **Colocar a placa em modo de bootloader:**
   - Pressione e segure o botão **BOOTSEL** enquanto conecta a placa ao computador via USB. Isso coloca o RP2040 em modo de bootloader.

3. **Carregar o código:**
   - Após colocar a placa em modo de bootloader, copie o arquivo compilado `seu_projeto.uf2` para o dispositivo de armazenamento que aparecerá no seu computador.
   - O código será automaticamente carregado na placa e executado.

4. **Abrir o Serial Monitor:**
   - No VS Code, abra o **Serial Monitor** para visualizar a saída da UART e interagir com o projeto.

## Demonstração

[Assistir Vídeo](https://drive.google.com/drive/folders/1bPRw2jQelPTr3Zoyh9oZ9bElKSvOwdBI?usp=drive_link)

## Conclusão

Este projeto consolidou os conceitos de **UART, I2C, interrupções, debounce e controle de LEDs** em um microcontrolador RP2040..