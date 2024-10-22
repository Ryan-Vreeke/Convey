#pragma once
#include <cstddef>
#include <cstdint>
class WSFrame {
public:
  uint8_t *m_buffer;
  uint8_t *m_payload;
  uint8_t m_opcode;
  uint8_t m_mask[4];
  size_t m_len;
  size_t m_payloadLen;
  bool m_fin;
  bool m_isMask;

  WSFrame(uint8_t *buffer, size_t len);
  ~WSFrame();

private:
  void extractPayload();
  void getPayloadLen(uint8_t *buffer);
};
