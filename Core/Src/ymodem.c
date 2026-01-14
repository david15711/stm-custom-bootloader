/*
 * ymodem.c
 *
 *  Created on: 2021. 1. 13.
 *      Author: baram
 */

#include "ymodem.h"
#include "uart.h"
#include "cli.h"
#include <string.h>

//TODO: check logic.
#define YMODEM_SOH    0x01
#define YMODEM_STX    0x02
#define YMODEM_ACK    0x06
#define YMODEM_NACK   0x15
#define YMODEM_EOT    0x04
#define YMODEM_C      0x43
#define YMODEM_CAN    0x18
#define YMODEM_BS     0x08

/**
 * @enum YMODEM_STATE
 * @brief 전체 YMODEM 세션의 상태를 정의
 */
enum
{
  YMODEM_STATE_WAIT_HEAD, /**< 파일 정보(헤더) 패킷 대기 */
  YMODEM_STATE_WAIT_FIRST, /**< 첫 번째 데이터 패킷 대기 */
  YMODEM_STATE_WAIT_DATA, /**< 연속적인 데이터 패킷 대기 */
  YMODEM_STATE_WAIT_LAST, /**< EOT 수신 후 마지막 처리 대기 */
  YMODEM_STATE_WAIT_END, /**< 전송 종료 대기 */
  YMODEM_STATE_WAIT_CANCEL, /**< 전송 취소 상태 */
};

/**
 * @enum YMODEM_PACKET_STATE
 * @brief 개별 패킷 수신을 위한 하위 상태 머신 정의
 */
enum
{
  YMODEM_PACKET_WAIT_FIRST, /**< 프레임 시작 바이트(SOH/STX/EOT/CAN) 대기 */
  YMODEM_PACKET_WAIT_SEQ1, /**< 패킷 번호 수신 */
  YMODEM_PACKET_WAIT_SEQ2, /**< 패킷 번호 반전값(Complement) 수신 */
  YMODEM_PACKET_WAIT_DATA, /**< 실제 데이터 필드 수신 */
  YMODEM_PACKET_WAIT_CRCH, /**< CRC 상위 바이트 수신 */
  YMODEM_PACKET_WAIT_CRCL, /**< CRC 하위 바이트 수신 */
};

static uint16_t crc16(uint8_t* data, uint16_t size);
static bool ymodem_ReceivePacket(ymodem_packet_t* p_packet, uint8_t data_in);

bool ymodem_Init(void)
{
  return true;
}

/**
 * @brief YMODEM 제어 구조체를 초기화하고 채널을 엽니다.
 * @param p_modem YMODEM 제어 구조체 포인터
 * @param ch UART 채널 번호
 * @return bool 초기화 성공 여부
 */
bool ymodem_Open(ymodem_t* p_modem, uint8_t ch)
{
  bool ret = true;

  p_modem->ch = ch; // channel for uart. only 1 use now, ignore.
  p_modem->is_init = true;

  p_modem->state = YMODEM_STATE_WAIT_HEAD;
  p_modem->rx_packet.state = YMODEM_PACKET_WAIT_FIRST;
  p_modem->file_buf = &p_modem->rx_packet.buffer[3];
  p_modem->file_buf_length = 0;
  p_modem->pre_time = get_milis();
  p_modem->start_time = 3000;

  p_modem->rx_packet.data = &p_modem->rx_packet.buffer[3];

  return ret;
}

void ymodem_Putch(ymodem_t* p_modem, uint8_t data)
{
  uart_WriteChar(_DEF_UART2, data);
}
/**
 * @brief 수신된 첫 번째 패킷(Packet 0)에서 파일 이름과 크기를 추출합니다.
 * @param p_modem YMODEM 제어 구조체 포인터
 * @return bool 정보 추출 성공 여부
 */
bool ymodem_GetFileInfo(ymodem_t* p_modem)
{
  bool ret = true;
  bool valid;
  uint16_t size_i;

  valid = false;
  for(int i = 0; i < 128; i++)
  {
    p_modem->file_name[i] = p_modem->rx_packet.data[i];
    if( p_modem->file_name[i] == 0x00 )
    {
      size_i = i + 1;
      valid = true;
      break;
    }
  }

  if( valid == true )
  {
    for(int i = size_i; i < 128; i++)
    {
      if( p_modem->rx_packet.data[i] == 0x20 )
      {
        p_modem->rx_packet.data[i] = 0x00;
        break;
      }
    }

    p_modem->file_length = (uint32_t)strtoul((const char*)&p_modem->rx_packet.data[size_i], (char**)NULL, (int)0);
  }

  return ret;
}
/**
 * @brief UART로부터 데이터를 읽어 YMODEM 프로토콜 상태를 업데이트합니다.
 * @param p_modem YMODEM 제어 구조체 포인터
 * @return bool 유효한 패킷 처리가 완료되었거나 상태가 변경되었을 때 true 반환
 */
bool ymodem_Receive(ymodem_t* p_modem)
{
  bool ret = false;
  bool update = false;
  uint32_t buf_length;

  if( p_modem->is_init != true )
  {
    p_modem->type = YMODEM_TYPE_ERROR;
    return true;
  }

  if( uartAvailable(p_modem->ch) > 0 )
  {
    p_modem->rx_data = uartRead(p_modem->ch);
    update = true;

    //uart_printf(_DEF_UART1, "Rx 0x%X, %d\n", p_modem->rx_data, p_modem->rx_packet.state);
  }

  if( update == true && ymodemReceivePacket(&p_modem->rx_packet, p_modem->rx_data) == true )
  {
    //uart_printf(_DEF_UART1, "RxPacket 0x%X\n", p_modem->rx_packet.stx);

    if( p_modem->state != YMODEM_STATE_WAIT_HEAD )
    {
      if( p_modem->rx_packet.stx == YMODEM_CAN )
      {
        p_modem->state = YMODEM_STATE_WAIT_CANCEL;
      }
    }

    switch( p_modem->state )
    {
      case YMODEM_STATE_WAIT_HEAD:
        if( p_modem->rx_packet.stx == YMODEM_EOT )
        {
          ymodemPutch(p_modem, YMODEM_NACK);
          p_modem->state = YMODEM_STATE_WAIT_LAST;
        }
        else if( p_modem->rx_packet.seq[0] == 0x00 )
        {
          p_modem->file_addr = 0;
          ymodemGetFileInfo(p_modem);

          ymodemPutch(p_modem, YMODEM_ACK);
          ymodemPutch(p_modem, YMODEM_C);

          p_modem->state = YMODEM_STATE_WAIT_FIRST;
          p_modem->type = YMODEM_TYPE_START;
          ret = true;
        }
        break;

      case YMODEM_STATE_WAIT_FIRST:
        if( p_modem->rx_packet.stx == YMODEM_EOT )
        {
          ymodemPutch(p_modem, YMODEM_NACK);
          p_modem->state = YMODEM_STATE_WAIT_LAST;
        }
        else if( p_modem->rx_packet.seq[0] == 0x01 )
        {
          p_modem->file_addr = 0;
          p_modem->file_received = 0;

          buf_length = (p_modem->file_length - p_modem->file_addr);
          if( buf_length > p_modem->rx_packet.length )
          {
            buf_length = p_modem->rx_packet.length;
          }
          p_modem->file_buf_length = buf_length;
          p_modem->file_received += buf_length;

          ymodemPutch(p_modem, YMODEM_ACK);

          p_modem->state = YMODEM_STATE_WAIT_DATA;
          p_modem->type = YMODEM_TYPE_DATA;
          ret = true;
        }
        break;

      case YMODEM_STATE_WAIT_DATA:
        if( p_modem->rx_packet.stx == YMODEM_EOT )
        {
          ymodemPutch(p_modem, YMODEM_NACK);
          p_modem->state = YMODEM_STATE_WAIT_LAST;
        }
        else
        {
          buf_length = (p_modem->file_length - p_modem->file_addr);
          if( buf_length > p_modem->rx_packet.length )
          {
            buf_length = p_modem->rx_packet.length;
          }
          p_modem->file_buf_length = buf_length;
          p_modem->file_addr += buf_length;
          p_modem->file_received += buf_length;

          ymodemPutch(p_modem, YMODEM_ACK);
          p_modem->type = YMODEM_TYPE_DATA;
          ret = true;
        }
        break;

      case YMODEM_STATE_WAIT_LAST:
        ymodemPutch(p_modem, YMODEM_ACK);
        ymodemPutch(p_modem, YMODEM_C);
        p_modem->state = YMODEM_STATE_WAIT_END;
        break;

      case YMODEM_STATE_WAIT_END:
        ymodemPutch(p_modem, YMODEM_ACK);
        p_modem->state = YMODEM_STATE_WAIT_HEAD;
        p_modem->type = YMODEM_TYPE_END;
        ret = true;
        break;

      case YMODEM_STATE_WAIT_CANCEL:
        ymodemPutch(p_modem, YMODEM_ACK);
        p_modem->state = YMODEM_STATE_WAIT_HEAD;
        p_modem->type = YMODEM_TYPE_CANCEL;
        ret = true;
        break;
    }
  }
  else
  {
    if( p_modem->rx_packet.state == YMODEM_PACKET_WAIT_FIRST )
    {
      if( get_milis() - p_modem->pre_time >= p_modem->start_time )
      {
        p_modem->pre_time = millis();
        ymodemPutch(p_modem, YMODEM_C);
      }
    }
  }

  return ret;
}

/**
 * @brief 1바이트씩 입력받아 패킷 단위로 조립하고 CRC를 확인합니다.
 * @param p_packet 패킷 정보를 저장할 구조체 포인터
 * @param data_in UART로 입력된 1바이트 데이터
 * @return bool 완전하고 유효한 패킷이 조립되면 true 반환
 */
bool ymodem_ReceivePacket(ymodem_packet_t* p_packet, uint8_t data_in)
{
  bool ret = false;

  switch( p_packet->state )
  {
    case YMODEM_PACKET_WAIT_FIRST:
      if( data_in == YMODEM_SOH )
      {
        p_packet->length = 128;
        p_packet->stx = data_in;
        p_packet->state = YMODEM_PACKET_WAIT_SEQ1;
      }
      if( data_in == YMODEM_STX )
      {
        p_packet->length = 1024;
        p_packet->stx = data_in;
        p_packet->state = YMODEM_PACKET_WAIT_SEQ1;
      }
      if( data_in == YMODEM_EOT )
      {
        p_packet->stx = data_in;
        ret = true;
      }
      if( data_in == YMODEM_CAN )
      {
        p_packet->stx = data_in;
        ret = true;
      }
      break;

    case YMODEM_PACKET_WAIT_SEQ1:
      p_packet->seq[0] = data_in;
      p_packet->state = YMODEM_PACKET_WAIT_SEQ2;
      break;

    case YMODEM_PACKET_WAIT_SEQ2:
      p_packet->seq[1] = data_in;
      if( p_packet->seq[0] == (uint8_t)(~data_in) )
      {
        p_packet->index = 0;
        p_packet->state = YMODEM_PACKET_WAIT_DATA;
      }
      else
      {
        p_packet->state = YMODEM_PACKET_WAIT_FIRST;
      }
      break;

    case YMODEM_PACKET_WAIT_DATA:
      p_packet->data[p_packet->index] = data_in;
      p_packet->index++;
      if( p_packet->index >= p_packet->length )
      {
        p_packet->state = YMODEM_PACKET_WAIT_CRCH;
      }
      break;

    case YMODEM_PACKET_WAIT_CRCH:
      p_packet->crc_recv = (data_in << 8);
      p_packet->state = YMODEM_PACKET_WAIT_CRCL;
      break;

    case YMODEM_PACKET_WAIT_CRCL:
      p_packet->crc_recv |= (data_in << 0);
      p_packet->state = YMODEM_PACKET_WAIT_FIRST;

      p_packet->crc = crc16(p_packet->data, p_packet->length);

      if( p_packet->crc == p_packet->crc_recv )
      {
        ret = true;
      }
      //uart_printf(_DEF_UART1, "crc %X %X\n", p_packet->crc, p_packet->crc_recv);
      break;
  }

  return ret;
}

#define CRC_POLY 0x1021

uint16_t crc_update(uint16_t crc_in, int incr)
{
  uint16_t xor = crc_in >> 15;
  uint16_t out = crc_in << 1;

  if( incr )
  {
    out++;
  }

  if( xor )
  {
    out ^= CRC_POLY;
  }

  return out;
}

/**
 * @brief 데이터 블록에 대한 CRC16 (CCITT) 값을 계산합니다.
 * @param data 계산할 데이터 버퍼 포인터
 * @param size 데이터 크기
 * @return uint16_t 계산된 CRC16 값
 */
uint16_t crc16(uint8_t* data, uint16_t size)
{
  uint16_t crc, i;

  for(crc = 0; size > 0; size--, data++)
  {
    for(i = 0x80; i; i >>= 1)
    {
      crc = crc_update(crc, *data & i);
    }
  }

  for(i = 0; i < 16; i++)
  {
    crc = crc_update(crc, 0);
  }

  return crc;
}

#ifdef _USE_HW_CLI
void cmd_ymodem(int argc, char** argv)
{
  bool ret = false;
  ymodem_t ymodem;
  bool keep_loop;
  uint8_t log_ch = _DEF_UART2;

  if( argc == 1 )
  {
    ymodem_Open(&ymodem, _DEF_UART2);

    keep_loop = true;

    while( keep_loop )
    {
      if( ymodemReceive(&ymodem) == true )
      {
        switch( ymodem.type )
        {
          case YMODEM_TYPE_START:
            uart_printf(log_ch, "YMODEM_TYPE_START %s %d\n", ymodem.file_name, ymodem.file_length);
            break;

          case YMODEM_TYPE_DATA:
            uart_printf(log_ch, "YMODEM_TYPE_DATA %d %d %%\n", ymodem.rx_packet.seq[0], ymodem.file_received * 100 / ymodem.file_length);
            break;

          case YMODEM_TYPE_END:
            uart_printf(log_ch, "YMODEM_TYPE_END \n");
            keep_loop = false;
            break;

          case YMODEM_TYPE_CANCEL:
            uart_printf(log_ch, "YMODEM_TYPE_CANCEL \n");
            keep_loop = false;
            break;

          case YMODEM_TYPE_ERROR:
            uart_printf(log_ch, "YMODEM_TYPE_ERROR \n");
            keep_loop = false;
            break;
        }
      }
    }
    ret = true;
  }

  if( ret != true )
  {
    uart_printf("ymodem fail");
  }
}
#endif
