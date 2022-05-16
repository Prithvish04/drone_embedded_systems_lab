#include "parse.h"


void print_GUI(Modes mode, DroneMessage* command, Measurement* measure){
	printf("%d ", mode);
	printf("%d ", command->yaw);
	printf("%d ", command->roll);
	printf("%d ", command->pitch);
	printf("%d ", command->lift);
	printf("%d %d %d %d ", measure->mot0, measure->mot1, measure->mot2, measure->mot3);
	printf("%d %d %d ", measure->phi, measure->theta, measure->psi);
	printf("%d %d %d ", measure->gyro0, measure->gyro1, measure->gyro2);
	printf("%d %ld %ld\n", measure->battery, measure->temperature, measure->pressure);
}



void print_commands(DroneMessage* msg){
	printf("%c |", (char)msg->key);
	printf("%d |", msg->event);
	printf("%d |", msg->yaw);
	printf("%d |", msg->roll);
	printf("%d |", msg->pitch);
	printf("%d |\n", msg->lift);
}


void print_measurement(Measurement* msg){
	printf("-------------------------------\n");
	printf("time : %10ld \n", msg->timestamp);
	printf("motor: %6d | %6d | %6d | %6d \n", msg->mot0, msg->mot1, msg->mot2, msg->mot3);
	printf("euler: %6d | %6d | %6d \n", msg->phi, msg->theta, msg->psi);
	printf("imu  : %6d | %6d | %6d \n", msg->gyro0, msg->gyro1, msg->gyro2);
	printf("b,t,p: %6d | %6ld | %6ld \n\n", msg->battery, msg->temperature, msg->pressure);
}


void log_measurement(uint32_t time, int16_t* motor, int16_t* euler, int16_t* imu, uint16_t battery, int32_t temperature, int32_t pressure, Measurement* msg){
	msg->timestamp = time;
	msg->mot0 = motor[0];
	msg->mot1 = motor[1];
	msg->mot2 = motor[2];
	msg->mot3 = motor[3];
	msg->phi = euler[0];
	msg->theta = euler[1];
	msg->psi = euler[2];
	msg->gyro0 = imu[0];
	msg->gyro1 = imu[1];
	msg->gyro2 = imu[2];
	msg->battery = battery;
	msg->temperature = temperature;
	msg->pressure = pressure;
}



void process_key(uint8_t c, DroneMessage* msg){
	if ((c == ' ') || (c == 27))
		msg->event = Panic_Event;
	else if ((c >= '0') && (c <= '8'))
		msg->event = events_array[c -'0'];
	else {
		msg->event = Null;
		msg->key = c;
	}
}


bool process_message(uint8_t c, DroneMessage* msg) {	
	static bool isValid = false;
	static int8_t curIdx = -1;
	if((c == START_BYTE) && !isValid) {
		isValid = true;
		curIdx = -1;
	}
	else if (isValid) {
		curIdx++;
		switch (curIdx) {
		case 0:
			msg->key = c;
			process_key(c, msg);
			break;
		case 1:
			msg->pitch = (int16_t)(c << 8);
			break;
		case 2:
			msg->pitch |= (int16_t)c;
			break;
		case 3:
			msg->roll = (int16_t)c << 8;
			break;
		case 4:
			msg->roll |= (int16_t)c;
			break;
		case 5:
			msg->yaw = (int16_t)c << 8;
			break;
		case 6:
			msg->yaw |= (int16_t)c;
			break;
		case 7:
			msg->lift = (int16_t)c << 8;
			break;
		case 8:
			msg->lift |= (int16_t)c;
			break;
		default:
			if (c == STOP_BYTE) {
				isValid = false;
				return true;
			}
		}
	}
	return false;
}