/*
 * reset.c
 *
 *  Created on: 2026. 1. 14.
 *      Author: MS
 */

#include "reset.h"

static uint32_t reset_count = 0;

bool reset_Init(void)
{
  // 만약 Reset 핀이 눌렸다면
  if( (RCC->CSR & RCC_CSR_PINRSTF_Msk) == RCC_CSR_PINRSTF )
  {
    RTC_BKP_Set(1, RTC_BKP_Get(1) + 1);
    delay(500);
    reset_count = RTC_BKP_Get(1);
  }
  RTC_BKP_Set(1, 0);

  if( reset_count != 2 )
  {
    return true;
  }
  return false;
}

uint32_t reset_GetCount(void)
{
  return reset_count;
}
