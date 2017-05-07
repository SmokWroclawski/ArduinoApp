#ifndef pms_header
#define pms_header

#include <stdint.h>

struct pms_state {
	uint8_t buffer[100];
	uint8_t buffer_position;
	double pm25;
	double pm10;
};

struct pms_state pms_init();
void pms_recv_byte(struct pms_state * state, uint8_t recv_byte);
void pms_process_frame(struct pms_state * state);

#endif
