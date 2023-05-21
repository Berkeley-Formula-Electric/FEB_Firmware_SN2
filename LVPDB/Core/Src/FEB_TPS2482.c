#include "FEB_TPS2482.h"

void FEB_TPS2482_SETUP(I2C_HandleTypeDef hi2c, uint8_t DEV_ADDR, uint8_t CONFIG[], uint8_t CAL_REG[], uint8_t ALERT[], uint8_t LIMIT[]) {
	HAL_I2C_Mem_Write(&hi2c, DEV_ADDR, 0x00 << 1, 1, CONFIG, 16, HAL_MAX_DELAY); // configure
	HAL_I2C_Mem_Write(&hi2c, DEV_ADDR, 0x05 << 1, 1, CAL_REG, 16, HAL_MAX_DELAY); // calibrate
	HAL_I2C_Mem_Write(&hi2c, DEV_ADDR, 0x06 << 1, 1, ALERT, 16, HAL_MAX_DELAY); // set alert
	HAL_I2C_Mem_Write(&hi2c, DEV_ADDR, 0x07 << 1, 1, LIMIT, 16, HAL_MAX_DELAY); // set limit
}

void FEB_TPS2482_shutdownIfError(I2C_HandleTypeDef * hi2c, uint8_t DEV_ADDR, GPIO_TypeDef * EN, uint16_t EN_NUM, GPIO_TypeDef * AL, uint16_t AL_NUM,
		GPIO_TypeDef * PG, uint16_t PG_NUM, float VMAX, float VMIN, float IMAX, float IMIN, float PMAX, float PMIN) {

	// pull EN low if
		// 1. alert pin pulled low
		// 2. PG goes low while EN is high
		// 3. shunt voltage, bus voltage, power, or current out of bounds

	// 1.
	if (HAL_GPIO_ReadPin(AL, AL_NUM) == GPIO_PIN_RESET) {
		HAL_GPIO_WritePin(EN, EN_NUM, GPIO_PIN_RESET);
	}
	// 2.
	if ((HAL_GPIO_ReadPin(EN, EN_NUM) == GPIO_PIN_SET) && (HAL_GPIO_ReadPin(PG, PG_NUM) == GPIO_PIN_RESET)) {
		HAL_GPIO_WritePin(EN, EN_NUM, GPIO_PIN_RESET);
	}
	// 3.
	FEB_TPS2482_pullLowIfOutOfBounds(hi2c, DEV_ADDR, EN, EN_NUM, VMAX, VMIN, 0x01); // shunt voltage
	FEB_TPS2482_pullLowIfOutOfBounds(hi2c, DEV_ADDR, EN, EN_NUM, VMAX, VMIN, 0x02); // bus voltage
	FEB_TPS2482_pullLowIfOutOfBounds(hi2c, DEV_ADDR, EN, EN_NUM, PMAX, PMIN, 0x03); // power
	FEB_TPS2482_pullLowIfOutOfBounds(hi2c, DEV_ADDR, EN, EN_NUM, IMAX, IMIN, 0x04); // current

}

void FEB_TPS2482_pullLowIfOutOfBounds(I2C_HandleTypeDef * hi2c, uint8_t DEV_ADDR, GPIO_TypeDef * EN, uint16_t EN_NUM, float MAX, float MIN,
		uint8_t REG) {
	uint8_t buf[12];
	buf[0] = REG;
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(hi2c, DEV_ADDR, buf, 1, 100);
	if (ret != HAL_OK) {
		HAL_GPIO_WritePin(EN, EN_NUM, GPIO_PIN_RESET); // pull EN low
	} else {
		ret = HAL_I2C_Master_Receive(hi2c, DEV_ADDR, buf, 2, 100);
		if (ret != HAL_OK) {
			HAL_GPIO_WritePin(EN, EN_NUM, GPIO_PIN_RESET); // pull EN low
		} else {
			int val = ((int16_t)buf[0] << 4) | (buf[1] >> 4); // combine the 2 bytes
			val = val - 1;
			val |= 0xF000; // subtract 1 and take complement
			float parsed = val * 0.0000025;// convert to decimal and multiply by 2.5uV
			if (parsed > MAX || parsed < MIN) {
				HAL_GPIO_WritePin(EN, EN_NUM, GPIO_PIN_RESET);
			}
		}
	}
}


