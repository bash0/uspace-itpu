#ifndef ACC_MAG_H
#define ACC_MAG_H

#include <stdint.h>

/********************************************************
 Name          : lsm303_mag_acc.h
 Author        : Morten Olsen
 Description   :
 **********************************************************/

//------------------   L S M 3 0 3   A C C E L E R O M E T E R  -------------------
#define LSM303_LIN_ACC_ADDR             0x18	// I2C-slave address of accelerometer
#define LSM303_SUB_ADDR_multiple_read 0b10000000 		// setting the 7th bit of the sub-address register to read the most significant bit first (allow reading of multiple bytes)
#define LSM303_CTRL_REG1_A 		0x20 	// Controls power mode, data rate and enables Z-Y-X axes
#define LSM303_CTRL_REG4_A 		0x23
#define LSM303_STATUS_REG_A 	0x27 	// status if new data is available or  has been overwritten un-read data.
#define LSM303_OUT_X_L_A 		0x28 	// linear acceleration data address (Atheel says address is 0xA8)
#define LSM303_OUT_X_H_A 		0x29 	// linear acceleration data address
#define LSM303_OUT_Y_L_A 		0x2A 	// linear acceleration data address
#define LSM303_OUT_Y_H_A 		0x2B 	// linear acceleration data address
#define LSM303_OUT_Z_L_A 		0x2C 	// linear acceleration data address
#define LSM303_OUT_Z_H_A 		0x2D 	// linear acceleration data address


//------------------   L S M 3 0 3   M A G N E T O M E T E R   -------------------
#define LSM303_MAG_SENS_ADDR 	0x1E	// read/write address of the magnetometer
#define LSM303_CRA_REG_M 		0x00
#define LSM303_CRB_REG_M 		0x01
#define LSM303_MR_REG_M 		0x02
#define LSM303_OUT_X_H_M 		0x03	// magnetometer data address
#define LSM303_OUT_X_L_M 		0x04	// magnetometer data address
#define LSM303_OUT_Y_H_M 		0x05	// magnetometer data address
#define LSM303_OUT_Y_L_M 		0x06	// magnetometer data address
#define LSM303_OUT_Z_H_M 		0x07	// magnetometer data address
#define LSM303_OUT_Z_L_M 		0x08	// magnetometer data address
#define LSM303_SR_REG_Mg		0x09 	// Make sure that 2nd bit is zero, otherwise new information cannot be stored!
#define LSM303_IRA_REG_M		0x0A 	// ID value = 0b01001000 =
#define LSM303_IRB_REG_M 		0x0B 	// ID value = 0b00110100
#define LSM303_IRC_REG_M 		0x0C 	// ID value = 0b00110011


int lsm303_init();

int lsm303_read_acc_measurements(float xyz[3]);
int lsm303_read_acc_raw(int16_t xyz[3]);
int lsm303_read_acc_SI(float xyz[3]);

int lsm303_read_mag_measurements(float xyz[3]);
int lsm303_read_magnet_raw(int16_t xyz[3]);
int lsm303_read_magnet_tesla(float xyz[3]);
int lsm303_read_magnet_gauss(float xyz_gauss[3]);
int lsm303_read_magnet_gauss_cal(float xyz[3]);
float lsm303_magnet_angle_rad();

//void lsm303_print_mag_measurements();

//void lsm303_print_acc_measurements();

float lsm303_mag_convert_to_deg_2D(int16_t xy[]);

int lsm303_calibrate_acc();

int lsm303_calibrate_mag();

#endif // ACC_MAG_H
