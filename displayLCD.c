#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "displayLCD.h"

// Função para inicializar o display
void init_display(ssd1306_t *ssd)
{
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(ssd, SSD1306_WIDTH, SSD1306_HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

// Função para exibir mensagem com quebra de linha no display
void display_message(ssd1306_t *ssd, const char *line1, const char *line2)
{
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, line1, 0, 0);  // Primeira linha
    ssd1306_draw_string(ssd, line2, 0, 20); // Segunda linha (20px abaixo)
    ssd1306_send_data(ssd);
}