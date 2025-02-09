#ifndef DISPLAY_H
#define DISPLAY_H

#include "ssd1306.h"
#include <stdbool.h>

// Definições do I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Tamanho do display
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Declarações das funções
void init_display(ssd1306_t *ssd);
void display_message(ssd1306_t *ssd, const char *line1, const char *line2);

#endif // DISPLAY_OLED_H