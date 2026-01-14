/*
 * cli.c
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#include "cli.h"
#include "uart.h"
#include "boot.h"
//#include "ymodem.h"

// --- [Type Definitions] ---
typedef void (*CliCmdFunc)(int argc, char** argv);

#define NICKNAME (const char*[])

typedef struct
{
  const char *command;      // 명령어 문자열
  CliCmdFunc handler;       // 핸들러 함수 포인터
  const char *help;         // 도움말
  const char *usage;
  const char **nickname;
} CLI_Command_t;

static void cmd_help(int argc, char** argv);
static void cmd_reset(int argc, char** argv);
static void cmd_clear(int argc, char** argv);
static void cmd_download(int argc, char** argv);;
static void cmd_boot(int argc, char** argv);

#ifdef DEBUG
static void cmd_LED(int argc, char** argv);
#endif

static const CLI_Command_t cli_commands[] =
#ifdef DEBUG
    {
    { "help", cmd_help, "Show all commands.",
        "Usage: "_ANSI(FG_CYAN, BG_BLACK)"help"ANSI_RESET" "_ANSI(FG_YELLOW, BG_BLACK)"[command]"ANSI_RESET"\r\n"
        "\tWithout arg: List all commands.\r\n"
        "\tWith arg: Show usage of the command.", NICKNAME
        {
        "?", NULL } },
    { "reset", cmd_reset, "System reset.", "Usage: "_ANSI(FG_CYAN, BG_BLACK)"reset"ANSI_RESET, NULL },
    { "clear", cmd_clear, "Clear terminal screen.", "Usage: "_ANSI(FG_CYAN, BG_BLACK)"clear"ANSI_RESET, NICKNAME
    {
    "clr", NULL } },
    { "download", cmd_download, "Download new firmware using ymodem protocol.", NULL, NULL },
    { "LED", cmd_LED, "Toggle LED.", NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL } };
#else
    {
    { "help", cmd_help, "Show all commands.", NULL, NULL },
    { "reset", cmd_reset, "System reset.", NULL, NULL },
    { "clear", cmd_clear, "Clear terminal screen.", NULL, NULL },
    { "download", cmd_download, "Download new firmware using ymodem protocol.", NULL, NULL },
    { "boot", cmd_boot, "Boot firmware.", NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL } };
#endif
bool cli_Init(void)
{
  return uart_isOpen(0);
}

#ifdef DEBUG
static float safe_atof(const char* str)
{
  float result = 0.0f;
  float fraction = 0.0f;
  int sign = 1;
  int divisor = 1;
  bool seen_digit = false;
  bool seen_dot = false;

  // Skip leading spaces
  while( *str == ' ' )
    str++;

  // Handle sign
  if( *str == '-' )
  {
    sign = -1;
    str++;
  }
  else if( *str == '+' )
  {
    str++;
  }

  // Parse digits and dot
  while( *str )
  {
    if( 0x2F < *str && *str < 0x3A )
    {
      seen_digit = true;
      if( !seen_dot )
      {
        result = result * 10.0f + (*str - '0');
      }
      else
      {
        fraction = fraction * 10.0f + (*str - '0');
        divisor *= 10;
      }
    }
    else if( *str == '.' )
    {
      if( seen_dot ) return 0.0f; // multiple dots → invalid
      seen_dot = true;
    }
    else if( *str == ' ' )
    {
      break; // allow trailing space
    }
    else
    {
      return -9999.9f; // invalid character
    }
    str++;
  }

  // Skip trailing spaces
  while( *str == ' ' )
    str++;

  // If there's anything left, it's invalid
  if( *str != '\0' || !seen_digit ) return -9999.9f;

  result += fraction / divisor;
  return result * sign;
}
#endif

static char* trimleft(char* str)
{
  int spos = 0;
  int i;

  for(i = 0; i < (int)strlen(str); i++)
  {
    if( str[i] != ' ' && str[i] != '\t' )
    {
      spos = i;
      break;
    }
  }

  if( spos == 0 ) return str;

  return &str[spos];
}

static char* trimright(char* str)
{
  int i;

  for(i = strlen(str) - 1; i >= 0; i--)
  {
    if( str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r' ) str[i] = 0x00;
    else break;
  }
  return str;
}

static int ArgsParsing(char* cmdline, const char* delim, char** argv, bool trim)
{
  char *tok;
  int argc = 0;

  argv[argc] = NULL;
  for(tok = strtok(cmdline, delim); tok; tok = strtok(NULL, delim))
  {
    if( trim ) argv[argc++] = trimright(trimleft(tok));
    else argv[argc++] = tok;
  }

  return argc;
}
#ifdef DEBUG
static int cli_parse_args(char* cmdline, char** argv, int max_args)
{
  int argc = 0;
  // 공백, 탭, 개행문자 등을 모두 구분자로 처리
  char *tok = strtok(cmdline, " \t\r\n");

  while( tok != NULL && argc < max_args )
  {
    argv[argc++] = tok;
    tok = strtok(NULL, " \t\r\n");
  }

  return argc;
}
#endif

#ifdef DEBUG
static void cmd_help(int argc, char** argv)
{
  const CLI_Command_t *pCmd = cli_commands;

  // 1. "help" 만 입력했을 때: 요약 리스트 출력
  if( argc == 1 )
  {
    uart_printf("\r\n"_ANSI(FG_WHITE,BG_BLUE)
    "------------------------- Command List -------------------------"
    ANSI_RESET"\r\n");
    while( pCmd->command != NULL )
    {
      uart_printf(_ANSI(FG_CYAN, BG_BLACK)"  %-10s "ANSI_RESET": "_ANSI(FG_YELLOW, BG_BLACK)"%s"ANSI_RESET"\r\n", pCmd->command, pCmd->help);
      pCmd++;
    }
    uart_printf("Type 'help <command>' for details.\r\n");
    uart_printf(_ANSI(FG_WHITE,BG_BLUE)
    "----------------------------------------------------------------"
    ANSI_RESET"\r\n");
  }
  // 2. "help <명령어>" 입력했을 때: 상세 설명 출력
  else
  {
    bool found = false;

    // 테이블을 뒤져서 argv[1]과 일치하는 명령어 찾기
    while( pCmd->command != NULL )
    {
      if( strcmp(pCmd->command, argv[1]) == 0 )
      {
        found = true;
        uart_printf("\r\n"_ANSI(FG_WHITE,BG_BLUE)"[ Help for '%s' ]"ANSI_RESET"\r\n", pCmd->command);

        if( pCmd->usage != NULL )
        {
          uart_printf("%s\r\n", pCmd->usage);
        }
        else
        {
          // 상세 설명이 NULL이면 요약 설명이라도 출력
          uart_printf(_ANSI(FG_CYAN,BG_BLACK)"%s"ANSI_RESET" (No usageed help available)\r\n", pCmd->command);
        }
        break;
      }
      pCmd++;
    }

    if( !found )
    {
      uart_printf("Unknown: '%s'\r\n", argv[1]);
    }
  }
}
#else
static void cmd_help(int argc, char** argv)
{
  const CLI_Command_t *pCmd = cli_commands;

  // 1. "help" 만 입력했을 때: 요약 리스트 출력
  if( argc == 1 )
  {

    while( pCmd->command != NULL )
    {
      uart_printf(_DEF_UART2, "%-10s: %s\r\n", pCmd->command, pCmd->help);
      pCmd++;
    }
    uart_printf(_DEF_UART2, "Type 'help <command>' for details.\r\n");

  }
  // 2. "help <명령어>" 입력했을 때: 상세 설명 출력
  else
  {
    bool found = false;

    // 테이블을 뒤져서 argv[1]과 일치하는 명령어 찾기
    while( pCmd->command != NULL )
    {
      if( strcmp(pCmd->command, argv[1]) == 0 )
      {
        found = true;
        uart_printf(_DEF_UART2, "\r\n[Help for '%s']\r\n", pCmd->command);

        if( pCmd->usage != NULL )
        {
          uart_printf(_DEF_UART2, "%s\r\n", pCmd->usage);
        }
        else
        {
          // 상세 설명이 NULL이면 요약 설명이라도 출력
          uart_printf(_DEF_UART2, _DEF_UART2, "%-10s: %s\r\n", pCmd->command, pCmd->help);
        }
        break;
      }
      pCmd++;
    }

    if( !found )
    {
      uart_printf(_DEF_UART2, "Unknown: '%s'\r\n", argv[1]);
    }
  }
}
#endif

static void cmd_reset(int argc, char** argv)
{
  uart_printf(_DEF_UART2, "Reset...\r\n");
  NVIC_SystemReset();
}

static void cmd_clear(int argc, char** argv)
{
  uart_printf(_DEF_UART2, ANSI_RESET"\x1B[0;0f\x1B[J");
}

static void cmd_download(int argc, char** argv)
{
  ;
}

static void cmd_boot(int argc, char** argv)
{
  if( boot_CheckFw() )
  {
    boot_JumpToFw();
  }
  else uart_printf(_DEF_UART2, "Firmware not ready.\r\n");
}

#ifdef DEBUG
static void cmd_LED(int argc, char** argv)
{
  LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}
#endif

static void cli_process_command(char* cmd_buffer)
{
  char *argv[10]; // 최대 인자 개수 제한
  int argc = ArgsParsing(cmd_buffer, " \t\r\n", argv, 1);

  if( argc == 0 ) return; // 빈 명령어

  const CLI_Command_t *pCmd = cli_commands;
  bool found = false;

  while( pCmd->command != NULL )
  {
    bool is_match = false;

    if( strcmp(argv[0], pCmd->command) == 0 )
    {
      is_match = true;
    }
    else if( pCmd->nickname != NULL ) // 닉네임 비교 (메인 명령어가 아닐 때만 확인)
    {
      const char **pNick = pCmd->nickname;
      while( *pNick != NULL )
      {
        if( strcmp(argv[0], *pNick) == 0 )
        {
          is_match = true;
          break; // 일치하는 닉네임을 찾았으므로 내부 루프 탈출
        }
        pNick++;
      }
    }

    // 3. 일치하는 경우 핸들러 실행
    if( is_match )
    {
      if( pCmd->handler )
      {
        pCmd->handler(argc, argv);
      }
      found = true;
      break; // 명령어를 찾았으므로 전체 검색 루프 탈출
    }

    pCmd++; // 다음 명령어 구조체로 이동
  }

  if( !found )
  {
    uart_printf(_DEF_UART2, "Unknown: '%s'\r\n", argv[0]);
  }
}

void cli_run(void)
{
  static uint8_t idx = 0;
  static char buffer[BUFSIZE] =
  { 0 };

  char c = uart_ReadChar(_DEF_UART2);

  if( c > 0 )
  {
    switch( c )
    {
      case '\r': // Enter Key (CR or LF)
//      case '\n':
        if( idx > 0 )
        {
          buffer[idx] = '\0'; // Null termination
          uart_printf(_DEF_UART2, "\r\n"); // 줄바꿈
          cli_process_command(buffer);
//          uart_printf("in %s, buffer: %s\r\n", __func__, buffer); // dbg
          idx = 0; // 버퍼 초기화
        }
        else
        {
          uart_printf(_DEF_UART2, "\r\n"); // 빈 명령어일 경우 프롬프트만 갱신
        }
        break;

      case '\b':   // Backspace (Standard)
        if( idx > 0 )
        {
          idx--;
          buffer[idx] = '\0';
          uart_printf(_DEF_UART2, "\b \b"); // 커서 뒤로 -> 공백 출력(지우기) -> 커서 다시 뒤로
        }
        break;

      case 0x7F: // Delete (Putty 등 일부 터미널)
      case 0x1B: // ESC Key (Escape Sequence 시작)
        uart_printf(_DEF_UART2, ANSI_RESET"\x1B[K");
        memset(buffer, 0, sizeof(buffer));
        idx = 0;
        break;

      default: // 일반 문자 처리
        if( idx < (BUFSIZE - 1) )
        {
          // 출력 가능한 ASCII 문자만 버퍼링 (제어 문자 제외)
          if( c >= 0x20 && c < 0x7E )
          {
            buffer[idx++] = (char)c;
            uart_WriteChar(_DEF_UART2, c); // Local Echo
          }
        }
        break;
    } // END case c
  } // END if c > 0
}
