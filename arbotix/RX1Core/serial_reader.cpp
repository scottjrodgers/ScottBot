/**********************************************************************
  State machine responsible for reading messages from serial port

  Note:  Non-thread-safe
**********************************************************************/
#include "serial_reader.h"
#include "handlers.h"

// State Variables
int state = 0;
int counter = 0;
int data_size = 0;
int message_id = 0;
int num_actuators = 0;

// read buffer
unsigned char serial_data[1024];

// function to process one character
int process_char(unsigned char ch){
  switch(ch){
    case 0:
        if(ch == 0xFF){
          state = 1;
        }
        break;
    case 1:
        if(ch & 0xF0 == 0xF0){
          state = 2;
          message_id = ch & 0x0F;
        } else {
          state = 0;
        }
        break;
    case 2:
        num_actuators = (int)ch;
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
        update_targets(serial_data, num_actuators, data_size);
      }
      state = 0;
      break;
  }
}
