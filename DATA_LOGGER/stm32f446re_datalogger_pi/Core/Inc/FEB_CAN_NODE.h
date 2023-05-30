#ifndef INC_FEB_CAN_NODE_H_
#define INC_FEB_CAN_NODE_H_

#include "stdio.h"
#include "string.h"

/*** data logger ***/
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;
uint16_t msg_count = 0;


/*** SETTINGS ***/
#define ID_LENGTH_BITS 11
#define BITS_PER_ID 7
#define BITS_PER_MESSAGE_TYPE 4

typedef uint32_t AddressIdType;
typedef uint8_t FilterArrayLength;

/*** EMERGENCY IDs ***/
#define EMERGENCY_ID 0b0000000
#define EMERGENCY_APPS_EMERGENCY 0b00000000000
#define EMERGENCY_SW_EMERGENCY 0b00000000001

/*** EMERGENCY MESSSAGE BUFFER ***/
#define EMERGENCY_APPS_EMERGENCY_TYPE uint8_t
#define EMERGENCY_SW_EMERGENCY_TYPE uint8_t

typedef struct EMERGENCY_MESSAGE_TYPE {
    EMERGENCY_APPS_EMERGENCY_TYPE apps_emergency;
    EMERGENCY_SW_EMERGENCY_TYPE sw_emergency;
} EMERGENCY_MESSAGE_TYPE;
EMERGENCY_MESSAGE_TYPE EMERGENCY_MESSAGE;

void Store_EMERGENCY_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    char buf[128];
    int buf_len;
    switch (RxId){
        case EMERGENCY_APPS_EMERGENCY:
            memcpy(&(EMERGENCY_MESSAGE.apps_emergency), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,EMERGENCY,APPS_EMERGENCY,%d\n", HAL_GetTick()/1000.0, EMERGENCY_MESSAGE.apps_emergency);
            break;
        case EMERGENCY_SW_EMERGENCY:
            memcpy(&(EMERGENCY_MESSAGE.sw_emergency), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,EMERGENCY,SW_EMERGENCY,%d\n", HAL_GetTick()/1000.0, EMERGENCY_MESSAGE.sw_emergency);
            break;
    }
    HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, buf_len, 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 1000);
}

/*** BMS IDs ***/
#define BMS_ID 0b0000001
#define BMS_TEMPERATURE 0b00000010000
#define BMS_VOLTAGE 0b00000010001

/*** BMS MESSSAGE BUFFER ***/
#define BMS_TEMPERATURE_TYPE float
#define BMS_VOLTAGE_TYPE float

typedef struct BMS_MESSAGE_TYPE {
    BMS_TEMPERATURE_TYPE temperature;
    BMS_VOLTAGE_TYPE voltage;
} BMS_MESSAGE_TYPE;
BMS_MESSAGE_TYPE BMS_MESSAGE;

void Store_BMS_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    char buf[128];
    int buf_len;
    switch (RxId){
        case BMS_TEMPERATURE:
            memcpy(&(BMS_MESSAGE.temperature), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,BMS,TEMPERATURE,%.3f\n", HAL_GetTick()/1000.0, BMS_MESSAGE.temperature);
            break;
        case BMS_VOLTAGE:
            memcpy(&(BMS_MESSAGE.voltage), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,BMS,VOLTAGE,%.3f\n", HAL_GetTick()/1000.0, BMS_MESSAGE.voltage);
            break;
    }
    HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, buf_len, 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 1000);
}

/*** SW IDs ***/
#define SW_ID 0b0000010
#define SW_READY_TO_DRIVE 0b00000100000
#define SW_COOLANT_PUMP 0b00000100001
#define SW_ACUMULATOR_FANS 0b00000100010
#define SW_EXTRA 0b00000100011

/*** SW MESSSAGE BUFFER ***/
#define SW_READY_TO_DRIVE_TYPE uint8_t
#define SW_COOLANT_PUMP_TYPE uint8_t
#define SW_ACUMULATOR_FANS_TYPE uint8_t
#define SW_EXTRA_TYPE uint8_t

typedef struct SW_MESSAGE_TYPE {
    SW_READY_TO_DRIVE_TYPE ready_to_drive;
    SW_COOLANT_PUMP_TYPE coolant_pump;
    SW_ACUMULATOR_FANS_TYPE acumulator_fans;
    SW_EXTRA_TYPE extra;
} SW_MESSAGE_TYPE;
SW_MESSAGE_TYPE SW_MESSAGE;

void Store_SW_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    char buf[128];
    int buf_len;
    switch (RxId){
        case SW_READY_TO_DRIVE:
            memcpy(&(SW_MESSAGE.ready_to_drive), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,SW,READY_TO_DRIVE,%d\n", HAL_GetTick()/1000.0, SW_MESSAGE.ready_to_drive);
            break;
        case SW_COOLANT_PUMP:
            memcpy(&(SW_MESSAGE.coolant_pump), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,SW,COOLANT_PUMP,%d\n", HAL_GetTick()/1000.0, SW_MESSAGE.coolant_pump);
            break;
        case SW_ACUMULATOR_FANS:
            memcpy(&(SW_MESSAGE.acumulator_fans), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,SW,ACUMULATOR_FANS,%d\n", HAL_GetTick()/1000.0, SW_MESSAGE.acumulator_fans);
            break;
        case SW_EXTRA:
            memcpy(&(SW_MESSAGE.extra), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,SW,EXTRA,%d\n", HAL_GetTick()/1000.0, SW_MESSAGE.extra);
            break;
    }
    HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, buf_len, 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 1000);
}

/*** APPS IDs ***/
#define APPS_ID 0b0000011
#define APPS_ACCELERATOR1_PEDAL 0b00000110000
#define APPS_ACCELERATOR2_PEDAL 0b00000110001
#define APPS_BRAKE_PEDAL 0b00000110010
#define APPS_TORQUE 0b00000110011

/*** APPS MESSSAGE BUFFER ***/
#define APPS_ACCELERATOR1_PEDAL_TYPE float
#define APPS_ACCELERATOR2_PEDAL_TYPE float
#define APPS_BRAKE_PEDAL_TYPE float
#define APPS_TORQUE_TYPE float

typedef struct APPS_MESSAGE_TYPE {
    APPS_ACCELERATOR1_PEDAL_TYPE accelerator1_pedal;
    APPS_ACCELERATOR2_PEDAL_TYPE accelerator2_pedal;
    APPS_BRAKE_PEDAL_TYPE brake_pedal;
    APPS_TORQUE_TYPE torque;
} APPS_MESSAGE_TYPE;
APPS_MESSAGE_TYPE APPS_MESSAGE;

void Store_APPS_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    char buf[128];
    int buf_len;
    switch (RxId){
        case APPS_ACCELERATOR1_PEDAL:
            memcpy(&(APPS_MESSAGE.accelerator1_pedal), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,APPS,ACCELERATOR1_PEDAL,%.3f\n", HAL_GetTick()/1000.0, APPS_MESSAGE.accelerator1_pedal);
            break;
        case APPS_ACCELERATOR2_PEDAL:
            memcpy(&(APPS_MESSAGE.accelerator2_pedal), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,APPS,ACCELERATOR2_PEDAL,%.3f\n", HAL_GetTick()/1000.0, APPS_MESSAGE.accelerator2_pedal);
            break;
        case APPS_BRAKE_PEDAL:
            memcpy(&(APPS_MESSAGE.brake_pedal), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,APPS,BRAKE_PEDAL,%.3f\n", HAL_GetTick()/1000.0, APPS_MESSAGE.brake_pedal);
            break;
        case APPS_TORQUE:
            memcpy(&(APPS_MESSAGE.torque), RxData, data_length);
            buf_len = sprintf(buf, "%.3f,APPS,TORQUE,%.3f\n", HAL_GetTick()/1000.0, APPS_MESSAGE.torque);
            break;
    }
    HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, buf_len, 1000);
    HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 1000);
}

/*** RMS IDs ***/
#define RMS_ID 0b0000100


/*** IVPDB IDs ***/
#define IVPDB_ID 0b0000101


/*** RX Arrays ***/
const AddressIdType BMS_RX_ID[] = {SW_ID};
const FilterArrayLength BMS_RX_NUM = 1;

const AddressIdType APPS_RX_ID[] = {BMS_ID, SW_ID};
const FilterArrayLength APPS_RX_NUM = 2;

const AddressIdType RMS_RX_ID[] = {APPS_ID};
const FilterArrayLength RMS_RX_NUM = 1;

const AddressIdType IVPDB_RX_ID[] = {SW_ID};
const FilterArrayLength IVPDB_RX_NUM = 1;

const AddressIdType* assign_filter_array(AddressIdType NODE_ID) {
    switch(NODE_ID) {
        case BMS_ID:
            return BMS_RX_ID;
            break;
        case APPS_ID:
            return APPS_RX_ID;
            break;
        case RMS_ID:
            return RMS_RX_ID;
            break;
        case IVPDB_ID:
            return IVPDB_RX_ID;
            break;
    }
    return 0;
}

FilterArrayLength assign_filter_array_legnth(AddressIdType NODE_ID) {
    switch(NODE_ID) {
        case BMS_ID:
            return BMS_RX_NUM;
            break;
        case APPS_ID:
            return APPS_RX_NUM;
            break;
        case RMS_ID:
            return RMS_RX_NUM;
            break;
        case IVPDB_ID:
            return IVPDB_RX_NUM;
            break;
    }
    return 0;
}

void store_msg(CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[]) {
    switch(pHeader->StdId >> BITS_PER_MESSAGE_TYPE) {
        case EMERGENCY_ID:
            Store_EMERGENCY_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        case BMS_ID:
            Store_BMS_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        case SW_ID:
            Store_SW_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        case APPS_ID:
            Store_APPS_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        default:
        	char buf[128];
			int buf_len;
			switch(pHeader->StdId) {
				case 0x0C0: //inverter command
					float torque = 0.0;
					memcpy(&(torque), RxData, 2);
					buf_len = sprintf(buf, "%.3f,IVT,TORQUE,%.3f\n", HAL_GetTick()/1000.0, torque);
				default:
					return;
			}
			HAL_SPI_Transmit(&hspi2, (uint8_t *)buf, buf_len, 1000);
			HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 1000);
    }
}

#endif /* INC_FEB_CAN_H_ */
