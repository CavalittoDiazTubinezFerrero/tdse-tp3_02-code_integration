#include "display.h"
#include <stdbool.h>

// ================== Definiciones =====================
#define DISPLAY_IR_CLEAR_DISPLAY   0x01
#define DISPLAY_IR_ENTRY_MODE_SET  0x04
#define DISPLAY_IR_DISPLAY_CONTROL 0x08
#define DISPLAY_IR_FUNCTION_SET    0x20
#define DISPLAY_IR_SET_DDRAM_ADDR  0x80

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0x02
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0x00

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0x04
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF   0x00
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF    0x00

#define DISPLAY_IR_FUNCTION_SET_4BITS    0x00
#define DISPLAY_IR_FUNCTION_SET_2LINES   0x08
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0x00

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0x00
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 0x40
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 0x14
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 0x54

#define DISPLAY_RS_INSTRUCTION 0
#define DISPLAY_RS_DATA        1

// ================== Mapeo de pines Nucleo =====================
#define LCD_D4_PORT GPIOB
#define LCD_D4_PIN  GPIO_PIN_5
#define LCD_D5_PORT GPIOB
#define LCD_D5_PIN  GPIO_PIN_4
#define LCD_D6_PORT GPIOB
#define LCD_D6_PIN  GPIO_PIN_10
#define LCD_D7_PORT GPIOA
#define LCD_D7_PIN  GPIO_PIN_8
#define LCD_RS_PORT GPIOA
#define LCD_RS_PIN  GPIO_PIN_9
#define LCD_EN_PORT GPIOC
#define LCD_EN_PIN  GPIO_PIN_7

// ================== Macros HAL =====================
#define LCD_RS(state) HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_EN(state) HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_D4(state) HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_D5(state) HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_D6(state) HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD_D7(state) HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (state)?GPIO_PIN_SET:GPIO_PIN_RESET)

// ================== Funciones privadas =====================
static void lcdPulseEnable(void);
static void lcdSendNibble(uint8_t nibble);
static void lcdSendByte(uint8_t rs, uint8_t data);

// ================== Implementación =====================
void displayInit(displayConnection_t connection)
{
    (void)connection; // solo 4 bits soportado

    HAL_Delay(50);

    // Inicialización especial modo 4 bits (según HD44780 datasheet)
    LCD_RS(0);
    lcdSendNibble(0x03);
    HAL_Delay(5);
    lcdSendNibble(0x03);
    HAL_Delay(1);
    lcdSendNibble(0x03);
    HAL_Delay(1);
    lcdSendNibble(0x02); // pasar a 4 bits

    // Configuración: 2 líneas, 5x8 puntos
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_FUNCTION_SET | DISPLAY_IR_FUNCTION_SET_4BITS | DISPLAY_IR_FUNCTION_SET_2LINES | DISPLAY_IR_FUNCTION_SET_5x8DOTS);
    // Display OFF
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_DISPLAY_CONTROL);
    // Clear
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_CLEAR_DISPLAY);
    HAL_Delay(2);
    // Entry mode
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_ENTRY_MODE_SET | DISPLAY_IR_ENTRY_MODE_SET_INCREMENT | DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT);
    // Display ON
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_DISPLAY_CONTROL | DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON);
}

void displayCharPositionWrite(uint8_t x, uint8_t y)
{
    uint8_t addr = 0;
    switch(y) {
        case 0: addr = DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS + x; break;
        case 1: addr = DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS + x; break;
        case 2: addr = DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS + x; break;
        case 3: addr = DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS + x; break;
    }
    lcdSendByte(DISPLAY_RS_INSTRUCTION, DISPLAY_IR_SET_DDRAM_ADDR | addr);
}

void displayStringWrite(const char *str)
{
    while(*str) {
        lcdSendByte(DISPLAY_RS_DATA, (uint8_t)*str++);
    }
}

// ================== Funciones internas =====================
static void lcdPulseEnable(void)
{
    LCD_EN(1);
    HAL_Delay(1);
    LCD_EN(0);
    HAL_Delay(1);
}

static void lcdSendNibble(uint8_t nibble)
{
    LCD_D4((nibble >> 0) & 0x01);
    LCD_D5((nibble >> 1) & 0x01);
    LCD_D6((nibble >> 2) & 0x01);
    LCD_D7((nibble >> 3) & 0x01);
    lcdPulseEnable();
}

static void lcdSendByte(uint8_t rs, uint8_t data)
{
    LCD_RS(rs);
    lcdSendNibble(data >> 4);
    lcdSendNibble(data & 0x0F);
    HAL_Delay(1);
}
