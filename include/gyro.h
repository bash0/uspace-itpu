#ifndef GYRO_H
#define GYRO_H


#define TWI_SPEED	50000       // Speed of TWI
#define ITG3200_WHO_AM_I 	0x00
#define ITG3200_SMPLRT_DIV 	0x15
#define ITG3200_DLPF_FS 	0x16
#define ITG3200_TEMP_OUT_H 	0x1B
#define ITG3200_TEMP_OUT_L 	0x1C
#define ITG3200_GYRO_XOUT_H 0x1D
#define ITG3200_GYRO_XOUT_L 0x1E
#define ITG3200_GYRO_YOUT_H 0x1F
#define ITG3200_GYRO_YOUT_L 0x20
#define ITG3200_GYRO_ZOUT_H 0x21
#define ITG3200_GYRO_ZOUT_L 0x22
#define ITG3200_READ_START	ITG3200_GYRO_XOUT_H
#define ITG3200_PWR_MGM 	0x3E
#define ITG3200_I2C_ADDRESS 0b01101000 //pin 9 is low

#define GYRO_CONVERSION_FACTOR	14.375
#define DEG_TO_RAD  0.017453293
#define RAD_TO_DEG  57.29578

int itg3200_init();

int itg3200_read_raw(int xyz_raw[3]);

int itg3200_read_rad(float xyz_rad[3]);

int itg3200_read_deg(float xyz_deg[3]);

int itg3200_read_rad_calibrated(float xyz_rad_cal[3]);

int itg3200_read_deg_calibrated(float xyz_deg_cal[3]);

int itg3200_calibrate_gyro();

#endif // GYRO_H
