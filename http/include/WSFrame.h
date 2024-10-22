#pragma once
#include <cstddef>
#include <cstdint>

extern uint16_t make16(uint8_t msb, uint8_t lsb);

extern uint32_t make32(uint16_t msb, uint16_t lsb);

extern uint64_t make64(uint16_t m1, uint16_t l1, uint16_t m2, uint16_t l2);

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
  uint8_t maskStart;

  void extractPayload();
  void getPayloadLen();
  uint64_t longlongPL();
};
