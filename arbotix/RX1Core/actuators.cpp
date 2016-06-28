#include <ax12.h>
#include "robot.h"
#include "actuators.h"
#include "data_structures.h"
#include "rx1_api.h"

// These are the ID's that are known to the AX12s.
const char leg_actuator_ids[6][3] = {{1,3,5}, // front left leg (reversed)
                                     {2,4,6}, // front right leg
                                     {7,9,11}, // rear left leg (reversed)
                                     {8,10,12}, // rear right leg
                                     {13,15,17}, // middle left leg (reversed)
                                     {14,16,18}}; // middle right leg

/*************************************************************************
  Initialize the serial interface with the AX12 actuators
*************************************************************************/
void init_interface(){
  // Init AX12 Interface
  ax12Init(1000000);
}

/*************************************************************************
  Given an actuator and its value, reverse the set of actuators on the left
*************************************************************************/
// This is transforming it from the AX12 Bus to numbers for us to use
actuator_state_t *transform_actuator_state(actuator_state_t *state){
  for(int n = 1; n <= 18; n++){
    if(n % 2 == 1){
      state->position[n] = 1023 - state->position[n];
    }
  }
  return state;
}

// This is transforming actuator targets from our numbers to AX12 values
actuator_target_t *transform_actuator_target(actuator_target_t *tgt){

  // check bounds
  int pos;
  for(int i=0;i<6;i++){
    // COXA
    pos = tgt->position[leg_actuator_ids[i][COXA]];
    if(pos < COXA_MIN) pos = COXA_MIN;
    if(pos > COXA_MAX) pos = COXA_MAX;
    tgt->position[leg_actuator_ids[i][COXA]] = pos;

    // FEMUR
    pos = tgt->position[leg_actuator_ids[i][FEMUR]];
    if(pos < FEMUR_MIN) pos = FEMUR_MIN;
    if(pos > FEMUR_MAX) pos = FEMUR_MAX;
    tgt->position[leg_actuator_ids[i][FEMUR]] = pos;

    // TIBIA
    pos = tgt->position[leg_actuator_ids[i][TIBIA]];
    if(pos < TIBIA_MIN) pos = TIBIA_MIN;
    if(pos > TIBIA_MAX) pos = TIBIA_MAX;
    tgt->position[leg_actuator_ids[i][TIBIA]] = pos;
  }

  // reverse the left side
  for(int n = 1; n <= 18; n++){
    if(n % 2 == 1){
      tgt->position[n] = 1023 - tgt->position[n];
    }
  }
  return tgt;
}

/*************************************************************************
Update target positions and velocities
*************************************************************************/
RC_t sync_write(int* data, int addr){
  //unsigned char *data, int num_actuators, int data_size){
  int value;
  int length = 4 + (NUM_ACTUATORS * 3);   // 3 = id + pos(2byte)
  int checksum = 254 + length + AX_SYNC_WRITE + 2 + addr;

  setTXall();
  ax12write(0xFF);
  ax12write(0xFF);
  ax12write(0xFE);
  ax12write(length);
  ax12write(AX_SYNC_WRITE);
  ax12write(addr);
  ax12write(2); // 2 bytes each
  for(int i=1; i <= NUM_ACTUATORS; i++)
  {
    value = data[i];

    ax12write(i);
    checksum += (i); // ids are 1 - N
    ax12write(value & 0xff);
    ax12write(value >> 8);
    checksum += (value & 0xff) + (value >> 8);
  }
  ax12write(0xff - (checksum % 256));
  setRX(0);

  return SUCCESS;
}

/*************************************************************************
Update target positions and velocities
*************************************************************************/
RC_t update_targets(actuator_target_t *tgt_in){
  // Reverse the direction of the actuators on right hand side so all are same
  actuator_target_t *tgt = transform_actuator_target(tgt_in);

  RC_t rc1 = sync_write(tgt->speed, AX_GOAL_SPEED_L);
  delay(2);
  RC_t rc2 = sync_write(tgt->position, AX_GOAL_POSITION_L);
  return SUCCESS;
}


/*************************************************************************
Query All Registers
*************************************************************************/
actuator_state_t *query_all_registers(){

  actuator_state_t *state = new_actuator_state();

  for(int i=0; i<SERVO_CNT; i++){
    state->position[i] = ax12GetRegister(i+1, AX_PRESENT_POSITION_L, 2);
    state->speed[i] = ax12GetRegister(i+1, AX_PRESENT_SPEED_L, 2);
    state->load[i] = ax12GetRegister(i+1, AX_PRESENT_LOAD_L, 2);
    state->voltage[i] = ax12GetRegister(i+1, AX_PRESENT_VOLTAGE, 1);
    state->temperature[i] = ax12GetRegister(i+1, AX_PRESENT_TEMPERATURE, 1);
    state->moving[i] = ax12GetRegister(i+1, AX_MOVING, 1);
  }
  return transform_actuator_state(state);
}
