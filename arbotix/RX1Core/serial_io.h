// Serial Reader header
#include "data_structures.h"

int process_char(unsigned char ch);
void transmit_queue();
void enqueue_byte(unsigned char);

RC_t transmit_error(char *message);

//void enqueue_byte(unsigned char ch);
RC_t transmit_actuator_state(actuator_state_t *state);
actuator_target_t *decode_actuator_target_message(unsigned char *data,
                                                  int num_actuators,
                                                  int data_size);
actuator_state_t *decode_actuator_state_test(unsigned char *data,
                                                  int num_actuators,
                                                  int data_size);
