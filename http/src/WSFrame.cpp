#include "WSFrame.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

WSFrame::WSFrame(uint8_t *buffer, size_t len) : m_buffer(buffer), m_len(len) {
  m_fin = (buffer[0] >> 7);
  m_opcode = buffer[0] & 0x0F;
  m_isMask = (buffer[1] >> 7);

  getPayloadLen();

  if (m_isMask) {
    memcpy(m_mask, &buffer[maskStart], 4);
  }

  extractPayload();
}

WSFrame::~WSFrame() { delete[] m_payload; }

void WSFrame::extractPayload() {
  m_payload = new uint8_t[m_payloadLen + 1];

  if (m_isMask)
    maskStart += 4;

  if (m_isMask) {
    size_t i;
    for (i = 0; i < m_payloadLen; i++) {
      m_payload[i] = m_buffer[maskStart + i] ^ m_mask[i % 4];
    }

    m_payload[i] = 0;
  } else {
    memcpy(m_payload, &m_buffer[maskStart], m_payloadLen);
  }
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
