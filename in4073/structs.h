
#ifndef STRUCTS_H__
#define STRUCTS_H__


#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef enum {Safe_Mode,
              Panic_Mode,
              Manual_Mode,
              Calibration_Mode,
              Yaw_Mode,
              Full_Mode,
              Raw_Mode,
              Height_Mode,
              Wireless_Mode,
              __last_Mode} Modes;

typedef enum {Safe_Event,
              Panic_Event,
              Manual_Event,
              Calibration_Event,
              Yaw_Event,
              Full_Event,
              Raw_Event,
              Height_Event,
              Wireless_Event,
              Null,
              __last_Event} Events;

typedef struct DroneMessage { Events event;
							  uint8_t key;
							  int16_t pitch;
							  int16_t roll;
							  int16_t yaw;
							  int16_t lift;
							  
							  // manual mode params
							  int16_t lift_offset;
							  int16_t roll_offset;
							  int16_t yaw_offset;
							  int16_t pitch_offset;

							  // gains
							  uint16_t P;
							  uint16_t P1;
							  uint16_t P2; 

							} DroneMessage;

typedef struct Measurement { uint32_t timestamp;
							 int16_t mot0;
							 int16_t mot1;
							 int16_t mot2;
							 int16_t mot3;
							
							 int16_t phi;
							 int16_t theta;
							 int16_t psi;

							 int16_t sp;
							 int16_t sq;
							 int16_t sr;

							 int16_t sax;
							 int16_t say;
							 int16_t saz;
							 
							 // calibration mode params
							 int16_t phi_offset;
							 int16_t theta_offset;
							 int16_t psi_offset;
							
							 int16_t sp_offset;
							 int16_t sq_offset;
							 int16_t sr_offset;

							 int16_t sax_offset;
							 int16_t say_offset;
							 int16_t saz_offset;

							 uint16_t battery;
							 int32_t temperature;
							 int32_t pressure;
							} Measurement;

#endif