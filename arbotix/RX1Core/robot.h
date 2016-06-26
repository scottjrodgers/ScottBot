/************************************************
*  Key Robot Definitions
************************************************/
#ifndef ROBOT_DEF
#define ROBOT_DEF

// Total number of AX12 Actuators
#define NUM_ACTUATORS 18

// Number of legs
#define NUM_LEGS 6
#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define REAR_LEFT 2
#define REAR_RIGHT 3
#define MID_LEFT 4
#define MID_RIGHT 5

// defines for indexs
#define COXA 0
#define FEMUR 1
#define TIBIA 2

// Define minimums and maximums for different motors
#define COXA_MIN 212
#define COXA_MAX 812
#define FEMUR_MIN 212
#define FEMUR_MAX 812
#define TIBIA_MIN 212
#define TIBIA_MAX 812

// Bare minimum / maximum
#define ACTUATOR_MIN 50
#define ACTUATOR_MAX 970
#define SPEED_MIN -1000
#define SPEED_MAX 1000

// Define home position
#define COXA_HOME 512
#define FEMUR_HOME 600
#define TIBIA_HOME 300



#endif
