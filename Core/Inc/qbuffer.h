/*
 * qbuffer.h
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#ifndef INC_QBUFFER_H_
#define INC_QBUFFER_H_

#include "def.h"
#include <string.h>

typedef struct
{
  uint32_t in;
  uint32_t out;
  uint32_t size;
  uint32_t len;

  uint8_t *p_buf;
} qbuffer_t;

bool qbufferCreate(qbuffer_t* p_node, uint8_t* p_buf, uint32_t length);
uint32_t qbufferWrite(qbuffer_t* p_node, uint8_t* p_data, uint32_t length);
uint32_t qbufferRead(qbuffer_t* p_node, uint8_t* p_data, uint32_t length);
uint32_t qbufferAvailable(qbuffer_t* p_node);
uint32_t qbufferSize(qbuffer_t* p_node);
uint32_t qbufferEmpty(qbuffer_t* p_node);
uint32_t qbufferFull(qbuffer_t* p_node);
uint8_t qbufferCheck(qbuffer_t* p_node, uint32_t index);
void qbufferFlush(qbuffer_t* p_node);

#endif /* INC_QBUFFER_H_ */
