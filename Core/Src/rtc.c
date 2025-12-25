/*
 * rtc.c
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#include "rtc.h"

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
void MX_RTC_Init(void)
{
  /* USER CODE BEGIN RTC_Init 0 */
  /* USER CODE END RTC_Init 0 */

  LL_RTC_InitTypeDef RTC_InitStruct =
  { 0 };
  LL_RTC_TimeTypeDef RTC_TimeStruct =
  { 0 };

  LL_PWR_EnableBkUpAccess();
  /* Enable BKP CLK enable for backup registers */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);
  /* Peripheral clock enable */
  LL_RCC_EnableRTC();

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC and set the Time and Date
   */
  RTC_InitStruct.AsynchPrescaler = 0xFFFFFFFFU;
  LL_RTC_Init(RTC, &RTC_InitStruct);
  LL_RTC_SetAsynchPrescaler(RTC, 0xFFFFFFFFU);

  /** Initialize RTC and set the Time and Date
   */
  RTC_TimeStruct.Hours = 0;
  RTC_TimeStruct.Minutes = 0;
  RTC_TimeStruct.Seconds = 0;
  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);
  /* USER CODE BEGIN RTC_Init 2 */
  /* USER CODE END RTC_Init 2 */

}

void RTC_BKP_Set(uint32_t index, uint32_t data)
{
  return LL_RTC_BKP_SetRegister(BKP, index, data);
}

uint32_t RTC_BKP_Get(uint32_t index)
{
  return LL_RTC_BKP_GetRegister(BKP, index);
}
