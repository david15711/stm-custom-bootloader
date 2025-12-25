/*
 * cli.h
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_

#include <uart.h>
#include "hw_def.h"
#include "string.h"

#define BUFSIZE 64

#define ANSI_RESET      "\x1B[0m"
#define ANSI_BOLD       "\x1B[1m"

#define FG_BLACK      "30"
#define FG_RED        "31"
#define FG_GREEN      "32"
#define FG_YELLOW     "33"
#define FG_BLUE       "34"
#define FG_MAGENTA    "35"
#define FG_CYAN       "36"
#define FG_WHITE      "37"
#define BG_BLACK      "40"
#define BG_BLUE       "44"

#define _ANSI(x,y) "\x1B["x";"y"m"

bool cli_Init(void);
void cli_run(void);

#endif /* INC_CLI_H_ */
