//
// SSD1306Z LCD Driver: 0.96" OLED LY096BG30
//

#ifdef __cplusplus
extern "C" {
#endif // def __cplusplus
#define LCD_I2C_SLA        0x3c
#define LCD_I2C_PORT       I2C1

#define LCD_Xmax 128
#define LCD_Ymax 64
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

extern void Init_LCD(void);
extern void clear_LCD(void);
extern void clear_LCD_line(int line);
extern void print_C(uint8_t Line, uint8_t Col, char ascii);
extern void printC_5x7(int16_t x, int16_t y, unsigned char ascii_code);
extern void print_Line(uint8_t line, char text[]);
extern void printS(int16_t x, int16_t y, char text[]);
extern void printS_5x7(int16_t x, int16_t y, char text[]);

extern void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor);
extern void draw_Bmp8x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp120x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp8x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x24(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x40(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp64x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_LCD(unsigned char *buffer);

#ifdef __cplusplus
} // extern "C"
#endif // def __cplusplus
