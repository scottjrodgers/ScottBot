/**********************************************************************
  State machine responsible for reading messages from serial port

  Note:  Non-thread-safe
**********************************************************************/
#include <Arduino.h>
#include "serial_io.h"
#include "rx1_api.h"
#include "actuators.h"

// State Variables
int state = 0;
int counter = 0;
int data_size = 0;
int message_id = 0;
int num_actuators = 0;

/**********************************************************************
**  Function to process one character
**********************************************************************/
unsigned char serial_data[1024];  // read buffer
int in_checksum = 0;
int process_char(unsigned char ch){
  switch(ch){
    case 0:
        if(ch == 0xFF){
          in_checksum = ch;
          state = 1;
        }
        break;
    case 1:
        if(ch & 0xF0 == 0xF0){
          state = 2;
          message_id = ch & 0x0F;
          in_checksum += ch;
        } else {
          state = 0;
        }
        break;
    case 2:
        num_actuators = (int)ch;
        in_checksum += ch;
        state = 3;
        break;
    case 3:
        data_size = (int)ch;
        counter = 0;
        state = 4;
        break;
    case 4:
        serial_data[counter] = ch;
        counter ++;
        if(counter >= data_size){
          state = 5;
        }
        break;
    case 5:
      // check if checksum matches ch
      // if so, call the appropriate function to process this completed
      // message and its data
      if(true && message_id == 1){
        actuator_target_t *target = decode_actuator_target_message(serial_data, num_actuators, data_size);
        RC_t rc = update_targets(target);
      }
      state = 0;
      break;
  }
}


/*******************************************************************
** Transmit any data in the queue
*******************************************************************/
// Transmit_queue data
unsigned char output_data[1024];
int transmit_idx = 0;
int write_idx = 0;
void transmit_queue(){
  //if(Serial.availableForWrite() > 0){
    if(transmit_idx != write_idx){
      Serial.write(output_data[transmit_idx]);
      transmit_idx ++;
      if(transmit_idx > 1024){
        transmit_idx = 0;
      }
    }
  //}
}

// enqueue_byte
void enqueue_byte(unsigned char ch){
  output_data[write_idx] = ch;
  write_idx ++;
  if(write_idx > 1024){
    write_idx = 0;
  }
}

/******************************************************************
*******************************************************************/
// Global checksum counter
unsigned int checksum_counter = 0;
void reset_checksum(){
  checksum_counter = 0;
}

/******************************************************************
*******************************************************************/
void output_byte(int ch){
  checksum_counter += (unsigned char)ch;
  enqueue_byte(ch);
}

/*******************************************************************
*******************************************************************/
void output_checksum(){
  enqueue_byte(checksum_counter & 0xFF);
  checksum_counter = 0;
}

/*******************************************************************
* Take actuator_state message and transmit it to Serial Port
*******************************************************************/
RC_t transmit_actuator_state(actuator_state_t *state){
  // output the mega message
  reset_checksum();
  output_byte(0xff); // Header
  output_byte(0xf2); // Header + Message 2
  output_byte(NUM_ACTUATORS);  // Servo count
  output_byte(NUM_ACTUATORS * 9); // data size

  for(int i=1; i<=NUM_ACTUATORS; i++){
    int position = state->position[i];
    int speed = state->speed[i] + 1024;  // now it's centered on 1024 = 0
    int load = state->load[i] + 1024;    // centered on 1024 = 0
    int voltage = state->voltage[i];
    int temp = state->temperature[i];
    char moving = state->moving[i];
    output_byte(position >> 8);
    output_byte(position & 0xFF);
    output_byte(speed >> 8);
    output_byte(speed & 0xFF);
    output_byte(load >> 8);
    output_byte(load & 0xFF);
    output_byte(voltage);
    output_byte(temp);
    output_byte(moving);
  }
  output_checksum();

  return SUCCESS;
}

/*******************************************************************
* Decode the byte stream for a actuator_target into a message struct
*******************************************************************/
actuator_target_t *decode_actuator_target_message(unsigned char *data,
                                                  int num_actuators,
                                                  int data_size){
  actuator_target_t *tgt = new_actuator_target();
  if(data_size != num_actuators * 4) return NULL;

  int p = 0;
  for(int i=1; i <= NUM_ACTUATORS; i++)
  {
    tgt->position[i] = data[p] << 8 + data[p+1];
    p += 2;
    tgt->position[i] = data[p] << 8 + data[p+1];
    p += 2;
  }
  return tgt;
}



/*******************************************************************
**
*******************************************************************/
