/*
 * iwdg.c
 *
 *  Created on: 2025. 12. 22.
 *      Author: MS
 */

#include "iwdg.h"

/**
 * @brief IWDG Initialization Function
 * @param None
 * @retval None
 */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  LL_IWDG_Enable(IWDG);
  LL_IWDG_EnableWriteAccess(IWDG);
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);
  LL_IWDG_SetReloadCounter(IWDG, 4095);
  while( LL_IWDG_IsReady(IWDG) != 1 )
  {
  }

  LL_IWDG_ReloadCounter(IWDG);
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

void IWDG_Feed(void)
{
  LL_IWDG_ReloadCounter(IWDG);
}
