#include <Arduino.h>
#include <TimedAction.h>
#include <ax12.h>
#include "robot.h"
#include "rx1_api.h"
#include "actuators.h"
#include "serial_io.h"

unsigned char data[256];
unsigned char ch;
unsigned char prev = 0;

// read all the AX12a registers
void read_actuator_status(){
  actuator_state_t *status = query_all_registers();
  if(status != NULL){
    RC_t rc = transmit_actuator_state(status);
  }
}

// read LIDAR
void read_LIDAR(){
  // TODO: Read LIDAR distance from I2C
}

// Set up the timed functions
TimedAction statusThread = TimedAction(49, read_actuator_status);
//TimedAction lidarThread = TimedAction(101, read_LIDAR);

// Initialize everything before we begin the main loop
void setup(){
  pinMode(0,OUTPUT);

  // TODO: set up I2C for LIDAR

  // setup serial
  Serial.begin(115200);  // 115200


  // Initialize the interface to AX12a Servos
  init_interface();

  // wait, then check the voltage (LiPO safety)
  delay (500);
  float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  if (voltage < 10.0)
    while(1);

  for(int i=1;i<=18;i++){
    ax12SetRegister(i, AX_TORQUE_ENABLE, 1);
    delay(2);
    ax12SetRegister2(i, AX_GOAL_SPEED_L, 100);
    delay(2);
    ax12SetRegister2(i, AX_TORQUE_LIMIT_L, 100);
    delay(2);
  }
  //
  // actuator_target_t *target = new_actuator_target();
  // for(int i=1;i<=18;i++){
  //   target->speed[i] = 100;
  // }
  // for(int i=0;i<6;i++){
  //   target->position[leg_actuator_ids[i][0]] = 512; // COXA
  //   target->position[leg_actuator_ids[i][1]] = 700; // FEMUR
  //   target->position[leg_actuator_ids[i][2]] = 180; // TIBIA
  // }
  //
  // RC_t rc = write_actuator_target(target);
}

// The main loop for the ArbotiX-M Robocontroller
void loop(){
  // check on threads
  statusThread.check();
  // lidarThread.check();

  // Transmit from queue as needed
  transmit_queue();

  // Read in next command
  if(Serial.available() > 0){
    ch = (unsigned char)Serial.read();
    //enqueue_byte(ch);
    process_char(ch);
  }
}
