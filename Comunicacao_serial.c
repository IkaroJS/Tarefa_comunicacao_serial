#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/i2c.h"
#include "inc/font.h"
#include "inc/ssd1306.h"
#include "inc/ws2812.pio.h"

#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11
#define BUTTON_A 5
#define BUTTON_B 6
#define MATRIZ_LED 7

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define DEBOUNCE_TIME 20 // Tempo de debounce em ms
#define NUM_PIXELS 25

#define MAX_CHARS 20        // Quantidade máxima de caracteres exibidos
char buffer[MAX_CHARS + 1]; // +1 para o caractere nulo '\0'
int buffer_index = 0;

// Números de 0 a 9 usando a matriz 5x5 de LEDs
const int numeros[10][NUM_PIXELS] = {

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0, // 0
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0, // 1
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0, // 2
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 0, 0, // 3
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0, // 4
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0, // 5
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0, // 6
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0},

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 0, 0, 0, // 7
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0, // 8
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0, // 9
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0},
};

// rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// rotina para acionar a matrix de leds - ws2812b
void desenho_pio(double *numeros, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    int ordem_fisica[NUM_PIXELS] =
        {
            24, 23, 22, 21, 20,
            15, 16, 17, 18, 19,
            14, 13, 12, 11, 10,
            5, 6, 7, 8, 9,
            4, 3, 2, 1, 0};

    for (int16_t i = 0; i < NUM_PIXELS; i++)
    {
        int indice_fisico = ordem_fisica[i];

        valor_led = matrix_rgb(numeros[indice_fisico], r = 0.0, g = 0.0); // Define a cor do LED

        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

void init_display(ssd1306_t *ssd)
{
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

// Função para atualizar o estado de um LED e também display
void atualizar_led_e_display(ssd1306_t *ssd, int led_pin, const char *led_name)
{
    static bool led_state_green = false; // Estado do LED verde
    static bool led_state_blue = false;  // Estado do LED azul

    bool led_state = false;

    // Alterna o estado do LED
    if (led_pin == LED_GREEN)
    {
        led_state_green = !led_state_green;
        led_state = led_state_green;
        gpio_put(LED_GREEN, led_state);
    }
    else if (led_pin == LED_BLUE)
    {
        led_state_blue = !led_state_blue;
        led_state = led_state_blue;
        gpio_put(LED_BLUE, led_state);
    }

    // Exibe a mensagem no terminal
    printf("Botão %s pressionado. LED %s %s\n", led_name, led_name, led_state ? "Ligado" : "Desligado");

    // Atualiza informações da tela
    char message[32];
    sprintf(message, "LED %s", led_name); // Exibir nome do LED
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, message, 10, 10); // Primeira linha com o nome do LED

    // Exibe o estado do LED (Ligado ou Desligado)
    sprintf(message, "%s", led_state ? "Ligado" : "Desligado");
    ssd1306_draw_string(ssd, message, 10, 20); // Segunda linha com o estado do LED

    ssd1306_send_data(ssd); // Atualiza o display
}

void desenhar_numero(int num, PIO pio, uint sm)
{
    uint32_t valor_led;
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        valor_led = numeros[num][i] ? matrix_rgb(1.0, 0.0, 0.0) : matrix_rgb(0.0, 0.0, 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

volatile bool button_a_pressionado = false;
volatile bool button_b_pressionado = false;

// Função para interrupções do botão
void gpio_callback(uint gpio, uint32_t events) {
    sleep_ms(DEBOUNCE_TIME);
    if (gpio == BUTTON_A) {
        button_a_pressionado = !button_a_pressionado;
        gpio_put(LED_GREEN, button_a_pressionado);
        printf("Botão A pressionado\n");
    }
    if (gpio == BUTTON_B) {
        button_b_pressionado = !button_b_pressionado;
        gpio_put(LED_BLUE, button_b_pressionado);
        printf("Botão B pressionado\n");
    }
}


// Inicialização dos pinos
void init_pins()
{
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, false);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, false);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, false);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

// Inicializaçã
void on_uart_rx()
{
    // Inicializa a UART0 com baud rate de 115200
    uart_init(uart0, 115200);

    // Configura pino 0 como TX - Transmissor
    gpio_set_function(0, GPIO_FUNC_UART);

    // Configura pino 1 como RX - Receptor
    gpio_set_function(1, GPIO_FUNC_UART);

    // Habilita o FIFO para evitar sobrecarga de buffer
    uart_set_fifo_enabled(uart0, true);

    // Configura a interrupção para a UART0
    // Define a função de callback para a interrupção de recepção
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);

    // Habilita a interrupção de recepção de dados (RX) na UART
    uart_set_irq_enables(uart0, true, false);

    // Habilita a interrupção na UART0
    irq_set_enabled(UART0_IRQ, true);

    // Enquanto houver dados para ler na UART
    while (uart_is_readable(uart0))
    {
        // Lê o caractere recebido
        char c = uart_getc(uart0);
        // Envia o caractere de volta (echo) para verificação
        uart_putc(uart0, c);
    }
}

int main()
{
    stdio_init_all();
    init_pins();

    PIO pio = pio0;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 1.0, g = 0.0;

    bool ok = set_sys_clock_khz(128000, false);
    if (ok)
    {
        printf("Iniciando comunicação...\n");
    }

    // Configuração da PIO para os LEDs
    uint offset = pio_add_program(pio, &ws2812_program);
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, MATRIZ_LED, 800000, false);

    ssd1306_t ssd; // Declara a variável ssd como armazenamento de informações do display

    init_display(&ssd);

    printf("Digite algum valor para ser exibido no display:\n");

    char input = 0;

    while (true)
    {
        if (uart_is_readable(uart0))
        {
            input = uart_getc(uart0); // Agora estamos usando a mesma variável 'input' fora do bloco

            // Se for backspace e o buffer não estiver vazio
            if (input == '\b' && buffer_index > 0)
            {
                buffer_index--;              // Remove o último caractere
                buffer[buffer_index] = '\0'; // Atualiza a string
            }
            // Se não for backspace e houver espaço no buffer
            else if (input >= 32 && input <= 126 && buffer_index < MAX_CHARS)
            {
                buffer[buffer_index++] = input; // Adiciona caractere ao buffer
                buffer[buffer_index] = '\0';    // Mantém a string terminada
            }

            // Atualiza o display para mostrar o caractere digitado
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, &input, 10, 10); // Exibe o caractere atual
            ssd1306_send_data(&ssd);

            printf("Texto digitado: %c\n", input); // Exibe o caractere digitado
        }

        // Se o caractere for um número, aciona a matriz de LEDs 5x5
        if (input >= '0' && input <= '9')
        {
            int num = input - '0';         // Converte o caractere para o número correspondente
            desenhar_numero(num, pio, sm); // Atualiza a matriz de LEDs com o número
        }

        // Verifica se o botão A foi pressionado
        if (!gpio_get(BUTTON_A))
        {
            atualizar_led_e_display(&ssd, LED_GREEN, "Verde"); // Exibe mensagem para o botão A
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, "Botão A pressionado", 10, 10); // Mensagem no display
            ssd1306_send_data(&ssd);
        }

        // Verifica se o botão B foi pressionado
        if (!gpio_get(BUTTON_B))
        {
            atualizar_led_e_display(&ssd, LED_BLUE, "Azul"); // Exibe mensagem para o botão B
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, "Botao B Pressionado", 10, 10); // Mensagem no display
            ssd1306_send_data(&ssd);
        }
    }
}