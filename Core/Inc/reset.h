/*
 * reset.h
 *
 *  Created on: 2026. 1. 14.
 *      Author: MS
 */

#ifndef INC_RESET_H_
#define INC_RESET_H_

#include "rtc.h"
#include "hw_def.h"

bool reset_Init(void);
uint32_t reset_GetCount(void);

#endif /* INC_RESET_H_ */
