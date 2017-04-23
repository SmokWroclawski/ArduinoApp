#include <Arduino.h>
#include <Wire.h>

typedef struct bmp_state {
  int16_t AC1, AC2, AC3, _B1, B2, MB, MC, MD;
  uint16_t AC4, AC5, AC6;
  int32_t UT, UP, B5;
} bmp_state;

bmp_state bmp_init();
uint8_t bmp_read_data(uint8_t reg);
void bmp_write_data(uint8_t reg, uint8_t value);
void bmp_read_compensation_data(bmp_state * state);
void bmp_read_temp_and_pressure(bmp_state * state);
double bmp_get_temperature(bmp_state * state);
double bmp_get_pressure(bmp_state * state);
double bmp_get_altitude(double p, double p0);
