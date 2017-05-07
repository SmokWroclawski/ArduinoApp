#include "pms.h"

struct pms_state pms_init() {
	struct pms_state state;

	for (uint8_t i = 0; i < 100; i++)
		state.buffer[i] = '\0';
	state.buffer_position = 0;
	state.pm25 = 0.0;
	state.pm10 = 0.0;

	return state;
}

void pms_recv_byte(struct pms_state * state, uint8_t recv_byte) {
	if ((state->buffer_position == 0 && recv_byte == 0x42)
			|| (state->buffer_position == 1 && recv_byte == 0x4d)) {
		state->buffer_position++;
	} else if (state->buffer_position >= 2 && state->buffer_position < 30) {
		state->buffer[state->buffer_position - 2] = recv_byte;
		state->buffer_position++;
		if (state->buffer_position == 30) {
			pms_process_frame(state);
			state->buffer_position = 0;
		}
	} else {
		state->buffer_position = 0;
	}
}

void pms_process_frame(struct pms_state * state) {
	state->pm25 = state->buffer[4] * 256 + state->buffer[5];
	state->pm10 = state->buffer[6] * 256 + state->buffer[7];
}
