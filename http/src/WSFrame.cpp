#include "WSFrame.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

WSFrame::WSFrame(uint8_t *buffer, size_t len) : m_buffer(buffer), m_len(len) {
  m_fin = (buffer[0] >> 7);
  m_opcode = buffer[0] & 0x0F;
  m_isMask = (buffer[1] >> 7);

  getPayloadLen(buffer);

  if (m_isMask) {
    memcpy(m_mask, &buffer[2], 4);
  }

  extractPayload();
}

WSFrame::~WSFrame() { delete[] m_buffer; }

void WSFrame::extractPayload() {
  uint8_t start = 2;
  m_payload = (uint8_t *)malloc(m_payloadLen + 1);

  if (m_isMask)
    start += 4;

  if (m_isMask) {
    size_t i;
    for (i = 0; i < m_payloadLen; i++) {
      m_payload[i] = m_buffer[start + i] ^ m_mask[i % 4];
    }

    m_payload[i] = 0;
  } else {
    memcpy(m_payload, &m_buffer[start], m_payloadLen);
  }
}

void WSFrame::getPayloadLen(uint8_t *buffer) {
  size_t len = (buffer[1] & 0x7F);
  if (len <= 125) {
    m_payloadLen = len;
    return;
  } else if (len == 126) {
    // TODO: step 2
  } else if (len == 127) {
    // TODO: step 3
  }
}
