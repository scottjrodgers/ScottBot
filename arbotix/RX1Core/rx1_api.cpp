// RX1 Api functions

#include "data_structures.h"

// Define some fixed data structures for our use in the API
// Okay because we're single threaded and saves time dynamically
// allocating memory (plus prevents memory leaks)
actuator_state_t Actuator_State;
actuator_target_t Actuator_Target;
kinematic_state_t Kinematic_State;
kinematic_state_t Kinematic_Target;


// read_actuator_state
actuator_state_t *read_actuator_state(){
  return &Actuator_State;
}

// write_actuator_target
RC_t write_actuator_target(actuator_target_t *target){
  return FAILURE;
}

// get new, clean actuator_target_t
actuator_target_t *new_actuator_target(){
  for(int i = 0; i < NUM_ACTUATORS + 1; i++){
    Actuator_Target.position[i] = 512;
    Actuator_Target.speed[i] = 0;
  }
  return &Actuator_Target;
}

// get new, clean kinematic_target
kinematic_target_t *new_actuator_target(){
  Kinematic_Target.position.x = 0;
  Kinematic_Target.position.y = 0;
  Kinematic_Target.position.z = 0;
  Kinematic_Target.position.dx = 0;
  Kinematic_Target.position.dy = 0;
  Kinematic_Target.position.dz = 0;
  Kinematic_Target.up.x = 0;
  Kinematic_Target.up.y = 0;
  Kinematic_Target.up.z = 0;
  Kinematic_Target.fwd.x = 0;
  Kinematic_Target.fwd.y = 0;
  Kinematic_Target.fwd.z = 0;
  for(int i = 0; i < NUM_LEGS; i++){
    Kinematic_Target.legs[i].x = 0;
    Kinematic_Target.legs[i].y = 0;
    Kinematic_Target.legs[i].z = 0;
    Kinematic_Target.legs[i].dx = 0;
    Kinematic_Target.legs[i].dy = 0;
    Kinematic_Target.legs[i].dz = 0;
    Kinematic_Target.fixed[i] = 0;
  }
  return &Kinematic_Target;
}

// write_kinematic_target
RC_t write_kinematic_target(kinematic_state_t *target){
  return FAILURE;
}

// transform from a kinematic pose to an actuator_target
actuator_target_t *IK_transform(kinematic_state_t *target){
  return &Actuator_Target;
}

// transform from an actuator status to a kinematic pose
kinematic_state_t *FK_transform(actuator_state_t *state){
  return &Kinematic_State;
}
