/******************************************************************
Interface to ArbotX-M for RX-1 Robot by Scott Rodgers
******************************************************************/

#define SERVO_CNT 18

void init_interface();
void query_all_registers();
void update_targets(unsigned char *serial_data, int num_actuators, int data_size);

// int query_all_positions();
// int query_all_speeds();
