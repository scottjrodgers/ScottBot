#include <ax12.h>
#include "handlers.h"
#include "serial_io.h"

// Global checksum counter
unsigned int checksum_counter = 0;

// forward declarations
void reset_checksum();
void output_byte(int ch);
void output_checksum();

/*************************************************************************
*************************************************************************/
void init_interface(){
  // Init AX12 Interface
  ax12Init(1000000);
}

/* write pose out to servos using sync write. */
// void BioloidController::writePose(){
//     int temp;
//     int length = 4 + (poseSize * 3);   // 3 = id + pos(2byte)
//     int checksum = 254 + length + AX_SYNC_WRITE + 2 + AX_GOAL_POSITION_L;
//     setTXall();
//     ax12write(0xFF);
//     ax12write(0xFF);
//     ax12write(0xFE);
//     ax12write(length);
//     ax12write(AX_SYNC_WRITE);
//     ax12write(AX_GOAL_POSITION_L);
//     ax12write(2);
//     for(int i=0; i<poseSize; i++)
//     {
//         temp = pose_[i] >> BIOLOID_SHIFT;
//         checksum += (temp&0xff) + (temp>>8) + id_[i];
//         ax12write(id_[i]);
//         ax12write(temp&0xff);
//         ax12write(temp>>8);
//     }
//     ax12write(0xff - (checksum % 256));
//     setRX(0);
// }

/*************************************************************************
Update target positions and velocities
*************************************************************************/
void update_targets(unsigned char *data, int num_actuators, int data_size){
  if(data_size == num_actuators * 4){
    int pos_value;
    int sped_value;
    int length = 4 + (poseSize * 5);   // 5 = id + pos(2byte) + speed(2byte)
    int checksum = 254 + length + AX_SYNC_WRITE + 2 + AX_GOAL_POSITION_L;
    int p = 0;
    setTXall();
    ax12write(0xFF);
    ax12write(0xFF);
    ax12write(0xFE);
    ax12write(length);
    ax12write(AX_SYNC_WRITE);
    ax12write(AX_GOAL_POSITION_L);
    ax12write(4); // 2 for position + 2 for speed
    for(int i=0; i < poseSize; i++)
    {
      pos_value = data[p] << 8 + data[p+1];
      p += 2;
      speed_value = data[p] << 8 + data[p+1];
      p += 2;
      ax12write(id_[i]);
      checksum += (i+1); // ids are 1 - N
      ax12write(pos_value & 0xff);
      ax12write(pos_value >> 8);
      checksum += (pos_value & 0xff) + (pos_value >> 8) + id_[i];
      ax12write(speed_value & 0xff);
      ax12write(speed_value >> 8);
      checksum += (speed_value & 0xff) + (speed_value >> 8) + id_[i];
    }
    ax12write(0xff - (checksum % 256));
    setRX(0);
  }
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
  output_byte(0xf2); // Header + Message 2
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

/*************************************************************************
*************************************************************************/
void reset_checksum(){
  checksum_counter = 0;
}

/*************************************************************************
*************************************************************************/
void output_byte(int ch){
  checksum_counter += (unsigned char)ch;
  enqueue_byte(ch);
}

/*************************************************************************
*************************************************************************/
void output_checksum(){
  enqueue_byte(checksum_counter & 0xFF);
  checksum_counter = 0;
}
