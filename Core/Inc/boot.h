/*
 * boot.h
 *
 *  Created on: 2025. 12. 19.
 *      Author: MS
 */

#ifndef INC_BOOT_H_
#define INC_BOOT_H_

#include "rtc.h"
#include "hw_def.h"

bool boot_Init(void);
void boot_JumpToFw(void);
bool boot_CheckFw(void);

#endif /* INC_BOOT_H_ */
