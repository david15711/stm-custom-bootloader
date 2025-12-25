/*
 * boot.c
 *
 *  Created on: 2025. 12. 19.
 *      Author: MS
 *      //[출처] MCU 펌웨어 만들기 - Bootloader|작성자 바람
 */

#include "boot.h"

/***
 * TODO: add
 *    boot_ShowVersion()
 *    boot_ShowBoardVersion()
 *    boot_EraseFlashFw
 *    boot_WriteFlashFw
 */

static uint32_t reset_count = 0;

bool boot_Init(void)
{
  // 만약 Reset 핀이 눌렸다면
  if( RCC->CSR == RCC_CSR_PINRSTF )
  {
    RTC_BKP_Set(1, RTC_BKP_Get(1) + 1);
    delay(500);
    reset_count = RTC_BKP_Get(1);
  }
  RTC_BKP_Set(1, 0);

  if( reset_count != 2 )
  {
    if ( boot_CheckFw() )
    {
      boot_JumpToFw();
    }
  }

  return true;
}

void boot_JumpToFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_FW_ADDR_START + 4);

  LL_RCC_DeInit();

  for (int i=0; i<8; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    __DSB();
    __ISB();
  }
  SysTick->CTRL = 0;

  __set_MSP(0x8004000);
  (*jump_func)();
}

bool boot_CheckFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_FW_ADDR_START + 4);

  return ((uint32_t)(*jump_func) != 0xFFFFFFFF);
}

