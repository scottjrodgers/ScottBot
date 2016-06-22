/**********************************************************************
  State machine responsible for reading messages from serial port

  Note:  Non-thread-safe
**********************************************************************/

// State Variables
int state = 0;
int counter = 0;
int data_size = 0;
int message_id = 0;

// read buffer
unsigned char data[1024];

// function to process one character
int process(unsigned char ch){
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
        data_size = (int)ch;
        counter = 0;
        state = 3;
        break;
    case 3:
        data[counter] = ch;
        counter ++;
        if(counter >= data_size){
          state = 4;
        }
        break;
    case 4:
  }
}
