/*
 * boot.c
 *
 *  Created on: 2025. 12. 19.
 *      Author: MS
 *      //[출처] MCU 펌웨어 만들기 - Bootloader|작성자 바람
 */

#include "boot.h"
#include "reset.h"

/***
 * TODO: add
 *    boot_ShowVersion()
 *    boot_ShowBoardVersion()
 *    boot_EraseFlashFw //
 *    boot_WriteFlashFw // ymodem will use.
 */

bool boot_Init(void)
{
  if( reset_Init() )
  {
    if( boot_CheckFw() )
    {
      boot_JumpToFw();
    }
  }
  return true;
}

//__attribute__((optimize("O0")))
void boot_JumpToFw(void)
{
  uint32_t stack_pointer = *(__IO uint32_t*)FLASH_FW_ADDR_START;
  uint32_t jump_address = *(__IO uint32_t*)(FLASH_FW_ADDR_START + 4);
//  void (*app_jump)(void) = (void (*)(void))jump_address;

  LL_RCC_DeInit();
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  for(int i = 0; i < 8; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
    __DSB();
    __ISB();
  }

//  __set_MSP(stack_pointer);
//  app_jump();
  __asm volatile (
      "msr msp, %0 \n"  // (1) 스택 포인터 설정
      "bx %1       \n"// (2) 해당 주소로 점프
      :
      : "r" (stack_pointer), "r" (jump_address)// (3) 입력 파라미터
      : "memory"// (4) 클로버(Clobber) 목록
  );
}

bool boot_CheckFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_FW_ADDR_START + 4);

  return ((uint32_t)(*jump_func) != 0xFFFFFFFF);
}
