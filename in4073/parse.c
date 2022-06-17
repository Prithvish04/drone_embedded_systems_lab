#include "parse.h"


void print_GUI(Modes mode, DroneMessage* command, Measurement* measure){
	printf("gui ");
	printf("%d ", mode);
	printf("%d ", command->yaw_offset);
	printf("%d ", command->roll_offset);
	printf("%d ", command->pitch_offset);
	printf("%d ", command->lift_offset);
	printf("%d %d %d %d ", measure->mot0, measure->mot1, measure->mot2, measure->mot3);
	printf("%d %d %d ", measure->phi, measure->theta, measure->psi);
	printf("%d %d %d ", command->P, command->P1, command->P2);
	printf("%d %ld %ld \n", measure->battery, measure->temperature, measure->pressure);
}


void print_commands(DroneMessage* msg){
	printf("debug ");
	printf("%d ", msg->key);
	printf("%d ", msg->event);
	printf("%d ", msg->yaw);
	printf("%d ", msg->roll);
	printf("%d ", msg->pitch);
	printf("%d \n", msg->lift);
}

void print_debug(char* msg) {
	printf("debug %s \n", msg);
}


void log_measurement(uint32_t time, uint16_t* motor, int16_t* euler, int16_t* imu, uint16_t battery, int32_t temperature, int32_t pressure, Measurement* msg){
	msg->timestamp = time;
	msg->mot0 = motor[0];
	msg->mot1 = motor[1];
	msg->mot2 = motor[2];
	msg->mot3 = motor[3];
	msg->phi = euler[0];
	msg->theta = euler[1];
	msg->psi = euler[2];
	msg->sp = imu[0];
	msg->sq = imu[1];
	msg->sr = imu[2];
	msg->sax = imu[3];
	msg->say = imu[4];
	msg->saz = imu[5];
	msg->battery = battery;
	msg->temperature = temperature;
	msg->pressure = pressure;
}



void process_key(uint8_t c, DroneMessage* msg){
	static const uint8_t offset = 5, k_offset = 1, keyboard_lim = 50;

	if ((c == ' ') || (c == 27))
		msg->event = Panic_Event;
	else if ((c >= '0') && (c <= '8'))
		msg->event = events_array[c -'0'];
	else {
		msg->event = Null;
	}
	switch (msg->key){
    case LIFT_UP_KEY:
        msg->lift_offset = (msg->lift_offset + offset > keyboard_lim) ? keyboard_lim : msg->lift_offset + offset;
        break;
    case LIFT_DOWN_KEY:
        msg->lift_offset = (msg->lift_offset - offset > -keyboard_lim) ? msg->lift_offset - offset : -keyboard_lim;
        break;
    case ROLL_UP_KEY:
        msg->roll_offset = (msg->roll_offset + offset > keyboard_lim) ? keyboard_lim : msg->roll_offset + offset;
        break;
    case ROLL_DOWN_KEY:
        msg->roll_offset = (msg->roll_offset - offset > -keyboard_lim) ? msg->roll_offset - offset : -keyboard_lim;
        break;
    case PITCH_UP_KEY:
        msg->pitch_offset = (msg->pitch_offset + offset > keyboard_lim) ? keyboard_lim : msg->pitch_offset + offset;
        break;
    case PITCH_DOWN_KEY:
        msg->pitch_offset = (msg->pitch_offset - offset > -keyboard_lim) ? msg->pitch_offset - offset : -keyboard_lim;
        break;
    case YAW_UP_KEY:
        msg->yaw_offset = (msg->yaw_offset + offset > keyboard_lim) ? keyboard_lim : msg->yaw_offset + offset;
        break;
    case YAW_DOWN_KEY:
        msg->yaw_offset = (msg->yaw_offset - offset > -keyboard_lim) ? msg->yaw_offset - offset : -keyboard_lim;
        break;
    case P_UP_KEY: 
        msg->P += k_offset; 
        break;
    case P_DOWN_KEY:
        msg->P = (msg->P - k_offset < 0) ? 0 : msg->P - k_offset;
        break;
    case P1_UP_KEY: 
        msg->P1 += k_offset;  
        break;        
    case P1_DOWN_KEY:
        msg->P1 = (msg->P1 - k_offset < 0) ? 0 : msg->P1 - k_offset;
        break;
    case P2_UP_KEY: 
        msg->P2 += k_offset; 
        break;        
    case P2_DOWN_KEY:
        msg->P2 = (msg->P2 - k_offset < 0) ? 0 : msg->P2 - k_offset;
        break;
    default:
        break;
    }
    msg->key = 0xFF;  
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