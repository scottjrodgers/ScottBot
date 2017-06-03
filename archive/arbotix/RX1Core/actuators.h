/******************************************************************
Interface to ArbotX-M for RX-1 Robot by Scott Rodgers
******************************************************************/
#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "data_structures.h"

#define SERVO_CNT 18

void init_interface();
actuator_state_t *query_all_registers();
RC_t update_targets(actuator_target_t *tgt);

extern const char leg_actuator_ids[6][3];

#endif
