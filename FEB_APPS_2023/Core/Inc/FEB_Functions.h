
#include "stm32f4xx_hal.h"

float FEB_ADC_sampleChannel(ADC_HandleTypeDef *ADCx, uint32_t channel) {
  ADC_ChannelConfTypeDef adc_config = {0};

  adc_config.Channel = channel;
  adc_config.Rank = 1;
  adc_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;

  HAL_ADC_ConfigChannel(ADCx, &adc_config); // probably need error checking here
  HAL_ADC_Start(ADCx);
  HAL_ADC_PollForConversion(ADCx, 100);

  return (float)HAL_ADC_GetValue(ADCx) * 3.3 / 4096.;
}


