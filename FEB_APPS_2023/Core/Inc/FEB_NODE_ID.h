#include <string.h>

/*** SETTINGS ***/
#define ID_LENGTH_BITS 11
#define BITS_PER_ID 7
#define BITS_PER_MESSAGE_TYPE 4

typedef uint32_t AddressIdType;
typedef uint8_t FilterArrayLength;

/*** BMS IDs ***/
#define BMS_ID 0b0000001
#define BMS_TEMPERATURE 0b00000010001
#define BMS_VOLTAGE 0b00000010010

/*** BMS MESSSAGE BUFFER ***/
#define BMS_TEMPERATURE_TYPE float
#define BMS_VOLTAGE_TYPE float

typedef struct BMS_MESSAGE_TYPE {
    BMS_TEMPERATURE_TYPE temperature;
    BMS_VOLTAGE_TYPE voltage;
} BMS_MESSAGE_TYPE;
BMS_MESSAGE_TYPE BMS_MESSAGE;

void Store_BMS_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    switch (RxId){
        case BMS_TEMPERATURE:
            memcpy(&(BMS_MESSAGE.temperature), RxData, data_length);
            break;
        case BMS_VOLTAGE:
            memcpy(&(BMS_MESSAGE.voltage), RxData, data_length);
            break;
    }
}

/*** SM IDs ***/
#define SM_ID 0b0000010
#define SM_COMMAND_1 0b00000100001
#define SM_TORQUE 0b00000100010

/*** SM MESSSAGE BUFFER ***/
#define SM_COMMAND_1_TYPE uint8_t
#define SM_TORQUE_TYPE uint16_t

typedef struct SM_MESSAGE_TYPE {
    SM_COMMAND_1_TYPE command_1;
    SM_TORQUE_TYPE torque;
} SM_MESSAGE_TYPE;
SM_MESSAGE_TYPE SM_MESSAGE;

void Store_SM_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    switch (RxId){
        case SM_COMMAND_1:
            memcpy(&(SM_MESSAGE.command_1), RxData, data_length);
            break;
        case SM_TORQUE:
            memcpy(&(SM_MESSAGE.torque), RxData, data_length);
            break;
    }
}

/*** APPS IDs ***/
#define APPS_ID 0b0000011
#define APPS_ACCELERATOR1_PEDAL 0b00000110001
#define APPS_ACCELERATOR2_PEDAL 0b00000110010
#define APPS_BRAKE_PEDAL 0b00000110011

/*** APPS MESSSAGE BUFFER ***/
#define APPS_ACCELERATOR1_PEDAL_TYPE uint16_t
#define APPS_ACCELERATOR2_PEDAL_TYPE uint16_t
#define APPS_BRAKE_PEDAL_TYPE uint16_t

typedef struct APPS_MESSAGE_TYPE {
    APPS_ACCELERATOR1_PEDAL_TYPE accelerator1_pedal;
    APPS_ACCELERATOR2_PEDAL_TYPE accelerator2_pedal;
    APPS_BRAKE_PEDAL_TYPE brake_pedal;
} APPS_MESSAGE_TYPE;
APPS_MESSAGE_TYPE APPS_MESSAGE;

void Store_APPS_Msg(AddressIdType RxId, uint8_t *RxData, uint32_t data_length) {
    switch (RxId){
        case APPS_ACCELERATOR1_PEDAL:
            memcpy(&(APPS_MESSAGE.accelerator1_pedal), RxData, data_length);
            break;
        case APPS_ACCELERATOR2_PEDAL:
            memcpy(&(APPS_MESSAGE.accelerator2_pedal), RxData, data_length);
            break;
        case APPS_BRAKE_PEDAL:
            memcpy(&(APPS_MESSAGE.brake_pedal), RxData, data_length);
            break;
    }
}

/*** RMS IDs ***/
#define RMS_ID 0b0000100


/*** RX Arrays ***/
const AddressIdType BMS_RX_ID[] = {SM_ID};
const FilterArrayLength BMS_RX_NUM = 1;

const AddressIdType SM_RX_ID[] = {BMS_ID, APPS_ID};
const FilterArrayLength SM_RX_NUM = 2;

const AddressIdType APPS_RX_ID[] = {SM_ID};
const FilterArrayLength APPS_RX_NUM = 1;

const AddressIdType RMS_RX_ID[] = {SM_ID};
const FilterArrayLength RMS_RX_NUM = 1;

const AddressIdType* assign_filter_array(AddressIdType NODE_ID) {
    switch(NODE_ID) {
        case BMS_ID:
            return BMS_RX_ID;
            break;
        case SM_ID:
            return SM_RX_ID;
            break;
        case APPS_ID:
            return APPS_RX_ID;
            break;
        case RMS_ID:
            return RMS_RX_ID;
            break;
    }
    return 0;
}

FilterArrayLength assign_filter_array_legnth(AddressIdType NODE_ID) {
    switch(NODE_ID) {
        case BMS_ID:
            return BMS_RX_NUM;
            break;
        case SM_ID:
            return SM_RX_NUM;
            break;
        case APPS_ID:
            return APPS_RX_NUM;
            break;
        case RMS_ID:
            return RMS_RX_NUM;
            break;
    }
    return 0;
}

void store_msg(CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[]) {
    switch(pHeader->StdId >> BITS_PER_MESSAGE_TYPE) {
        case BMS_ID:
            Store_BMS_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        case SM_ID:
            Store_SM_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
        case APPS_ID:
            Store_APPS_Msg(pHeader->StdId, RxData, pHeader->DLC);
            break;
    }
}
