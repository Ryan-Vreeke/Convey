#include "WSFrame.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace Convey;
WSFrame::WSFrame(uint8_t *buffer, size_t len, int socket) : m_len(len), m_socket(socket) {
  m_buffer = new uint8_t[len];
  memcpy(m_buffer, buffer, len);
  m_fin = (m_buffer[0] >> 7);
  m_opcode = OPCODE(m_buffer[0] & 0x0F);
  m_isMask = (m_buffer[1] >> 7);

  getPayloadLen();

  if (m_isMask) {
    memcpy(m_mask, &m_buffer[maskStart], 4);
    maskStart += 4;
  }

  extractPayload(&m_buffer[maskStart], len - maskStart);
}

WSFrame::WSFrame() : m_mask{0}, m_payloadLen(0), m_fin(true), m_isMask(false) {}

WSFrame::~WSFrame() {
  delete[] m_buffer;
  delete[] m_payload;
}

void WSFrame::encodePayload(const uint8_t *buffer, size_t len) {
  int offset = 0;
  size_t lenByte;

  m_payloadLen = len;
  m_buffer = new uint8_t[len + 10];
  m_payload = new uint8_t[len];
  memcpy(m_payload, buffer, len);

  m_buffer[0] = (m_fin ? 0x80 : 0x00) | m_opcode;

  if (len <= 125) {
    m_len = len + 2;
    offset = 2;
    lenByte = len;
  } else if (len < 65535) {
    m_len = len + 4;
    offset = 4;
    lenByte = 126;
    *(uint16_t *)(m_buffer + 2) = (uint16_t)htons(len);
  } else {
    m_len = len + 10;
    offset = 10;
    lenByte = 127;
    *(uint64_t *)(m_buffer + 2) = (uint64_t)htobe64(len);
  }

  m_buffer[1] = (m_isMask ? 0x80 : 0x00) | lenByte;
  memcpy(&m_buffer[offset], m_payload, m_payloadLen);
}

void WSFrame::extractPayload(const uint8_t *buffer, size_t len) {
  constexpr int CHUNKSIZE = 4096;
  m_payload = new uint8_t[m_payloadLen + 1];

  for (size_t i = 0; i < len; i++) {
    m_payload[i] = decode(buffer[i], i % 4);
  }

  if (m_payloadLen <= 125) {
    m_payload[m_payloadLen] = 0;
    return;
  }

  uint8_t buf[CHUNKSIZE];
  int bytes = 0;

  for (size_t j = len; j < m_payloadLen;) {
    if ((bytes = read(m_socket, buf, CHUNKSIZE)) <= 0) break;

    for (int i = 0; i < bytes; i++) {
      m_payload[j + i] = decode(buf[i], i % 4);
    }

    j += bytes;
  }
}

uint8_t WSFrame::decode(uint8_t byte, uint8_t index) {
  if (m_isMask) {
    return (byte ^ m_mask[index]);
  }

  return byte;
}

void WSFrame::getPayloadLen() {
  size_t len = (m_buffer[1] & 0x7F);
  if (len <= 125) {
    m_payloadLen = len;
    maskStart = 2;
  } else if (len == 126) {
    m_payloadLen = make16(m_buffer[2], m_buffer[3]);
    maskStart = 4;
  } else if (len == 127) {
    m_payloadLen = longlongPL();
    maskStart = 10;
  }
}

uint64_t WSFrame::longlongPL() {
  return make64(make16(m_buffer[2], m_buffer[3]), make16(m_buffer[4], m_buffer[5]),
                make16(m_buffer[6], m_buffer[7]), make16(m_buffer[8], m_buffer[9]));
}

uint16_t WSFrame::make16(uint8_t msb, uint8_t lsb) {
  return (static_cast<uint16_t>(msb) << 8) | lsb;
}

uint32_t WSFrame::make32(uint16_t msb, uint16_t lsb) { return ((uint32_t)msb << 16) | lsb; }

uint64_t WSFrame::make64(uint16_t m1, uint16_t l1, uint16_t m2, uint16_t l2) {
  uint32_t MSB = make32(m1, l1);
  uint32_t LSB = make32(m2, l2);

  return ((uint64_t)MSB << 32) | LSB;
}
