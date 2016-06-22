#include <ax12.h>
#include "handlers.h"

// Global checksum counter
unsigned int checksum_counter = 0;

// forward declarations
void reset_checksum();
void output_byte(int ch);
void output_checksum();

// void read_register_short_all(int addr_L,
//                              unsigned char *lowbyte,
//                              unsigned char *highbyte);
// void read_register_byte_all(int addr_L, unsigned char *lowbyte);
// void write_message_shorts(unsigned char message_id,
//                            unsigned char *lowbyte,
//                            unsigned char *highbyte);
// void write_message_bytes(unsigned char message_id, unsigned char *lowbyte);

/*************************************************************************
*************************************************************************/
void init_interface(){
  // Init AX12 Interface
  ax12Init(1000000);
}

/*************************************************************************
Query All Registers
*************************************************************************/
void query_all_registers(){
  unsigned short position[SERVO_CNT];
  unsigned short speed[SERVO_CNT];
  unsigned short load[SERVO_CNT];
  unsigned char voltage[SERVO_CNT];
  unsigned char temp[SERVO_CNT];
  unsigned char moving[SERVO_CNT];

  for(int i=0; i<SERVO_CNT; i++){
    position[i] = (unsigned short)ax12GetRegister(i+1, AX_PRESENT_POSITION_L, 2);
    speed[i] = (unsigned short)ax12GetRegister(i+1, AX_PRESENT_SPEED_L, 2);
    load[i] = (unsigned short)ax12GetRegister(i+1, AX_PRESENT_LOAD_L, 2);
    voltage[i] = (unsigned char)ax12GetRegister(i+1, AX_PRESENT_VOLTAGE, 1);
    temp[i] = (unsigned char)ax12GetRegister(i+1, AX_PRESENT_TEMPERATURE, 1);
    moving[i] = (unsigned char)ax12GetRegister(i+1, AX_MOVING, 1);
  }

  // output the mega message
  output_byte(0xff); // Header
  output_byte(0xff); // Header 2
  output_byte(0xB1); // Message ID
  output_byte(SERVO_CNT);  // Servo count
  output_byte(SERVO_CNT * 9); // data size

  for(int i=0; i<SERVO_CNT; i++){
    output_byte(position[i] >> 8);
    output_byte(position[i] & 0xFF);
    output_byte(speed[i] >> 8);
    output_byte(speed[i] & 0xFF);
    output_byte(load[i] >> 8);
    output_byte(load[i] & 0xFF);
    output_byte(voltage[i]);
    output_byte(temp[i]);
    output_byte(moving[i]);
  }

  output_checksum();
}

/*************************************************************************
Query All Current Positions
*************************************************************************/
// int query_all_positions(){
//   unsigned char lowbyte[SERVO_CNT];
//   unsigned char highbyte[SERVO_CNT];
//
//   read_register_short_all(AX_PRESENT_POSITION_L, lowbyte, highbyte);
//   write_message_shorts(0xA0, lowbyte, highbyte);
//   return 0;
// }

/*************************************************************************
Query All Current Speeds
*************************************************************************/
// int query_all_speeds(){
//   unsigned char lowbyte[SERVO_CNT];
//   unsigned char highbyte[SERVO_CNT];
//
//   read_register_short_all(AX_PRESENT_SPEED_L, lowbyte, highbyte);
//   write_message_shorts(0xA1, lowbyte, highbyte);
//   return 0;
// }

/*************************************************************************
*************************************************************************/
// void write_message_shorts(unsigned char message_id,
//                            unsigned char *lowbyte,
//                            unsigned char *highbyte){
//   // Write return message
//   reset_checksum();
//   output_byte(0xff);
//   output_byte(0xff);
//   output_byte((int)message_id);
//   output_byte(2 * SERVO_CNT);
//
//   for(int i=0;i<SERVO_CNT;i++){
//     output_byte((int)highbyte[i]);
//     output_byte((int)lowbyte[i]);
//   }
//   output_checksum();
// }

/*************************************************************************
*************************************************************************/
// void write_message_bytes(unsigned char message_id,
//                          unsigned char *lowbyte){
//   // Write return message
//   reset_checksum();
//   output_byte(0xff);
//   output_byte(0xff);
//   output_byte((int)message_id);
//   output_byte(SERVO_CNT);
//
//   for(int i=0;i<SERVO_CNT;i++){
//     output_byte((int)lowbyte[i]);
//   }
//   output_checksum();
// }

/*************************************************************************
*************************************************************************/
// void read_register_short_all(int addr_L,
//                              unsigned char *lowbyte,
//                              unsigned char *highbyte){
//   for(int i=0;i<SERVO_CNT;i++){
//     lowbyte[i] = ax12GetRegister(i+1, addr_L, 1);
//     highbyte[i] = ax12GetRegister(i+1, addr_L + 1, 1);
//   }
// }

/*************************************************************************
*************************************************************************/
// void read_register_byte_all(int addr_L,
//                             unsigned char *lowbyte){
//   for(int i=0;i<SERVO_CNT;i++){
//     lowbyte[i] = ax12GetRegister(i+1, addr_L, 1);
//   }
// }

/*************************************************************************
*************************************************************************/
void reset_checksum(){
  checksum_counter = 0;
}

/*************************************************************************
*************************************************************************/
void output_byte(int ch){
  checksum_counter += (unsigned char)ch;
  Serial.write(ch);
}

/*************************************************************************
*************************************************************************/
void output_checksum(){
  unsigned char diff = 256 - checksum_counter & 0xFF;
  Serial.write(diff);
}
