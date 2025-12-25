/*
 * qbuffer.c
 *
 *  Created on: 2025. 12. 18.
 *      Author: MS
 */

#include "qbuffer.h"

bool qbufferCreate(qbuffer_t* p_node, uint8_t* p_buf, uint32_t length)
{
  if( p_node == NULL || p_buf == NULL || length == 0 )
  {
    return false;
  }

  p_node->in = 0;
  p_node->out = 0;
  p_node->len = length;
  p_node->size = 0;
  p_node->p_buf = p_buf;

  return true;
}

uint32_t qbufferWrite(qbuffer_t* p_node, uint8_t* p_data, uint32_t length)
{
  if( p_node == NULL || p_data == NULL || p_node->p_buf == NULL || length == 0 || p_node->len == p_node->size )
  {
    return 0;
  }

  uint32_t available = p_node->len - p_node->size;
  if( length > available )
  {
    length = available;
  }

  if( p_node->in + length > p_node->len ) // 쓰는 도중 배열 끝에 도달시, 배열 처음부터 이어서 (두 청크로 나눠서)
  {
    uint32_t first_chunk = p_node->len - p_node->in; // 현재 위치부터 버퍼 끝까지
    uint32_t second_chunk = length - first_chunk;    // 앞에서부터 쓸 나머지 길이

    // p_buf[in] 부터 끝까지 복사
    memcpy(&p_node->p_buf[p_node->in], p_data, first_chunk);
    // p_buf[0] 부터 나머지 복사
    memcpy(&p_node->p_buf[0], p_data + first_chunk, second_chunk);
  }
  else // 경우 2: 데이터가 연속적인 공간에 들어가는 경우
  {
    memcpy(&p_node->p_buf[p_node->in], p_data, length);
  }

  p_node->in = (p_node->in + length) % p_node->len;
  p_node->size += length;

  return length;
}

uint32_t qbufferRead(qbuffer_t* p_node, uint8_t* p_data, uint32_t length)
{
  // 1. 포인터 유효성 검사 (Crash 방지) 및 읽을 수 있는 실제 길이 계산
  if( p_node == NULL || p_data == NULL || p_node->p_buf == NULL || length == 0 || p_node->size == 0 )
  {
    return 0;
  }

  if( length > p_node->size )
  {
    length = p_node->size;
  }

  // 경우 1: 읽을 데이터가 버퍼 끝을 넘어 앞쪽으로 이어지는 경우
  if( p_node->out + length > p_node->len )
  {
    uint32_t first_chunk = p_node->len - p_node->out; // 버퍼 끝까지의 길이
    uint32_t second_chunk = length - first_chunk;     // 앞에서부터 읽을 나머지 길이

    memcpy(p_data, &p_node->p_buf[p_node->out], first_chunk);
    memcpy(p_data + first_chunk, &p_node->p_buf[0], second_chunk);
  }
  else  // 경우 2: 데이터가 연속적으로 있는 경우
  {
    memcpy(p_data, &p_node->p_buf[p_node->out], length);
  }

  p_node->out = (p_node->out + length) % p_node->len;
  p_node->size -= length;

  return length;
}

uint32_t qbufferAvailable(qbuffer_t* p_node)
{
  return (p_node->len - p_node->size);
}

uint32_t qbufferSize(qbuffer_t* p_node)
{
  return p_node->size;
}

uint32_t qbufferEmpty(qbuffer_t* p_node)
{
  return (p_node->size == 0);
}

uint32_t qbufferFull(qbuffer_t* p_node)
{
  return (p_node->size == p_node->len);
}

uint8_t qbufferCheck(qbuffer_t* p_node, uint32_t index)
{
  if( index < 0 ) index = p_node->len + index;
  if( index > p_node->size - 1 ) return 0;

  return p_node->p_buf[(p_node->out + index) % p_node->len];
}

void qbufferFlush(qbuffer_t* p_node)
{
  p_node->in = 0;
  p_node->out = 0;
}
