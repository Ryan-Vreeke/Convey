#include "WSFrame.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

WSFrame::WSFrame(uint8_t *buffer, size_t len, int socket)
    : m_buffer(buffer), m_len(len), m_socket(socket) {

  m_fin = (buffer[0] >> 7);
  m_opcode = OPCODE(buffer[0] & 0x0F);
  m_isMask = (buffer[1] >> 7);

  getPayloadLen();

  if (m_isMask) {
    memcpy(m_mask, &buffer[maskStart], 4);
    maskStart += 4;
  }

  extractPayload(&buffer[maskStart], len - maskStart);
}

WSFrame::WSFrame() : m_mask{0}, m_payloadLen(0), m_fin(true), m_isMask(false) {}

WSFrame::~WSFrame() {
  if (m_buffer) {
    delete[] m_buffer;
    m_buffer = nullptr;
  }

  if (m_payload) {
    delete[] m_payload;
    m_payload = nullptr;
  }
}

void WSFrame::EncodePayload(const uint8_t *buffer, size_t len) {
  int offset = 0;

  m_buffer = new uint8_t[len + 10];
  m_payloadLen = len;

  m_buffer[0] = (m_fin ? 0x80 : 0x00) | m_opcode;
  m_buffer[1] = (m_isMask ? 0x80 : 0x00) | (len <= 125 ? len : (len == 126 ? 126 : 127));

  if (len <= 125) {
    m_len = len + 2;
    offset = 2;
  } else if (len == 126) {
    m_len = len + 4;
    offset = 4;
    *(uint16_t *)(m_buffer + 2) = (uint16_t)htons(len);
  } else if (len == 127) {
    m_len = len + 10;
    offset = 10;
    *(uint64_t *)(m_buffer + 2) = (uint64_t)htobe64(len);
  }

  memcpy(&m_buffer[offset], buffer, m_payloadLen);
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
    if ((bytes = read(m_socket, buf, CHUNKSIZE)) <= 0)
      break;

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
  return make64(
      make16(m_buffer[2], m_buffer[3]), make16(m_buffer[4], m_buffer[5]),
      make16(m_buffer[6], m_buffer[7]), make16(m_buffer[8], m_buffer[9]));
}

uint16_t make16(uint8_t msb, uint8_t lsb) {
  return (static_cast<uint16_t>(msb) << 8) | lsb;
}

uint32_t make32(uint16_t msb, uint16_t lsb) {
  return ((uint32_t)msb << 16) | lsb;
}

uint64_t make64(uint16_t m1, uint16_t l1, uint16_t m2, uint16_t l2) {
  uint32_t MSB = make32(m1, l1);
  uint32_t LSB = make32(m2, l2);

  return ((uint64_t)MSB << 32) | LSB;
}
