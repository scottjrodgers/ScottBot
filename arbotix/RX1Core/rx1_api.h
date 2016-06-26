#ifndef RX1API_H
#define RX1API_H

#include "robot.h"
#include "data_structures.h"

actuator_state_t *read_actuator_state();
RC_t write_actuator_target(actuator_target_t *target);
actuator_state_t *new_actuator_state();
actuator_target_t *new_actuator_target();
kinematic_state_t* new_kinematic_target();
RC_t write_kinematic_target(kinematic_state_t *target);
actuator_target_t *IK_transform(kinematic_state_t *target);
kinematic_state_t *FK_transform(actuator_state_t *state);

#endif
