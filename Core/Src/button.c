/*
 * button.c
 *
 *  Created on: 2025. 12. 25.
 *      Author: MS
 */

#include "button.h"

// todo: 전체 논리 점검.

// --- 설정 상수 ---
#define DEBOUNCE_DELAY_MS    50   // 디바운싱 시간
#define LONG_PRESS_TIME_MS   1000 // 롱 프레스 기준 시간

// --- 내부 관리 변수 ---
static Button_t *_btn_table = 0;
static uint8_t _btn_count = 0;
static Button_ReadPinFn_t _read_pin_fn = 0; // 외부 핀 읽기 함수
static volatile uint32_t *_time_ms_ptr = 0; // 외부 시간 변수 포인터

// --- 초기화 및 등록 함수 ---

void Button_Init(Button_t* buttons, uint8_t count)
{
  _btn_table = buttons;
  _btn_count = count;

  for(int i = 0; i < count; i++)
  {
    _btn_table[i].state = BTN_STATE_RELEASED;
    _btn_table[i].event = BTN_EVENT_NONE;
    _btn_table[i].pressed_duration = 0;
    // 초기 하드웨어 상태 가정 (필요시 여기서 한 번 읽을 수도 있음)
    _btn_table[i]._last_hardware_state = false;
  }
}

void Button_RegisterReadFn(Button_ReadPinFn_t readFn)
{
  _read_pin_fn = readFn;
}

void Button_RegisterTimeVar(volatile uint32_t* timeVarMs)
{
  _time_ms_ptr = timeVarMs;
}

// --- 핵심 로직 ---

void Button_Process(void)
{
  if( !_btn_table || !_read_pin_fn || !_time_ms_ptr ) return; // 안전장치

  uint32_t current_time = *_time_ms_ptr;

  for(int i = 0; i < _btn_count; i++)
  {
    Button_t *btn = &_btn_table[i];

    // 1. 하드웨어 상태 읽기 (Active Low 고려)
    uint8_t raw_level = _read_pin_fn(btn->port, btn->pin);
    bool is_active = btn->active_low ? (raw_level == 0) : (raw_level == 1);

    // 2. 디바운싱 처리
    if( is_active != btn->_last_hardware_state )
    {
      btn->_last_debounce_time = current_time;
      btn->_last_hardware_state = is_active;
    }

    if( (current_time - btn->_last_debounce_time) > DEBOUNCE_DELAY_MS )
    {

      // 3. 상태 변경 감지
      if( is_active != (bool)btn->state )
      {

        if( is_active )
        {
          // 눌림 시작 (PRESS)
          btn->state = BTN_STATE_PRESSED;
          btn->press_start_time = current_time;
          btn->event = BTN_EVENT_PRESSED;
          btn->is_handled = false;
        }
        else
        {
          // 떼어짐 (RELEASE)
          btn->state = BTN_STATE_RELEASED;
          btn->event = BTN_EVENT_RELEASED;
          btn->is_handled = false;
        }
      }
    }

    // 4. 눌려있는 동안 시간 계산 및 롱 프레스 판정
    if( btn->state == BTN_STATE_PRESSED )
    {
      btn->pressed_duration = current_time - btn->press_start_time;

      // 롱 프레스 이벤트 발생 (이전에 처리되지 않은 경우에만)
      if( btn->pressed_duration >= LONG_PRESS_TIME_MS && btn->event != BTN_EVENT_LONG_PRESSED )
      {

        btn->event = BTN_EVENT_LONG_PRESSED;
        btn->is_handled = false;
      }
    }
    else
    {
      btn->pressed_duration = 0;
    }
  }
}
