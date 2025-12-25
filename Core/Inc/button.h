/*
 * button.h
 *
 *  Created on: 2025. 12. 25.
 *      Author: MS
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "hw_def.h"

// 버튼의 현재 물리적/논리적 상태
typedef enum
{
  BTN_STATE_RELEASED = 0, BTN_STATE_PRESSED = 1
} ButtonState_t;

// 버튼의 변동 사항 (이벤트)
typedef enum
{
  BTN_EVENT_NONE = 0, BTN_EVENT_PRESSED,      // 눌린 순간 (Rising/Falling edge depending on circuit)
  BTN_EVENT_RELEASED,     // 떼어진 순간
  BTN_EVENT_LONG_PRESSED, // 길게 눌림 감지
  BTN_EVENT_CLICK         // 짧게 눌렀다 뗌 (옵션)
} ButtonEvent_t;

// --- 구조체 정의 (Button Table Type) ---

typedef struct
{
  // 1. 식별 및 하드웨어 설정
  char *name;             // 버튼 이름 (디버깅용)
  void *port;             // GPIO 포트 (MCU에 따라 GPIO_TypeDef* 등으로 캐스팅)
  uint16_t pin;           // GPIO 핀 번호
  bool active_low;        // Active Low 여부 (GND 연결 시 true)

  // 2. 내부 상태 및 시간 (런타임 변수)
  ButtonState_t state;    // 현재 버튼 상태
  uint32_t press_start_time; // 눌리기 시작한 시간 (ms)
  uint32_t pressed_duration; // 눌려있는 지속 시간 (ms)

  // 3. 변동 (이벤트 결과)
  ButtonEvent_t event;    // 상위 레이어에서 확인할 이벤트
  bool is_handled;        // 이벤트 처리 여부 플래그

  // 4. 디바운싱용 내부 변수 (Private)
  uint32_t _last_debounce_time;
  bool _last_hardware_state;

} Button_t;

// --- 함수 타입 정의 (외부 의존성) ---

// 하드웨어 핀 상태를 읽어오는 함수 포인터 타입 (1: High, 0: Low 반환)
typedef uint8_t (*Button_ReadPinFn_t)(void* port, uint16_t pin);

// --- 공개 함수 프로토타입 ---

// 1. 초기화 및 외부 의존성 등록
void Button_Init(Button_t* buttons, uint8_t count);
void Button_RegisterReadFn(Button_ReadPinFn_t readFn);
void Button_RegisterTimeVar(volatile uint32_t* timeVarMs);

// 2. 주기적 처리 (메인 루프나 타이머 인터럽트에서 호출)
void Button_Process(void);

#endif /* INC_BUTTON_H_ */
