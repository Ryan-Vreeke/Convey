#pragma once
#include <cstddef>
#include <cstdint>

extern uint16_t make16(uint8_t msb, uint8_t lsb);
extern uint32_t make32(uint16_t msb, uint16_t lsb);
extern uint64_t make64(uint16_t m1, uint16_t l1, uint16_t m2, uint16_t l2);

enum OPCODE {
  CONTINUE = 0x0,
  TEXT = 0x1,
  BINARY = 0x2,
  CLOSE = 0x8,
  PING = 0x9,
  PONG = 0xA
};

class WSFrame {
public:
  uint8_t *m_buffer;
  uint8_t *m_payload;
  OPCODE m_opcode;
  uint8_t m_mask[4];
  size_t m_len;
  size_t m_payloadLen;
  bool m_fin;
  bool m_isMask;

  WSFrame(uint8_t *buffer, size_t len, int socket);
  WSFrame();
  ~WSFrame();

  void EncodePayload(const uint8_t *buffer, size_t len);

private:
  uint8_t maskStart;
  size_t bytesRead = 0;
  int m_socket;

  int j = 0;

  void extractPayload(const uint8_t *buff, size_t bytesRead);
  uint8_t decode(uint8_t byte, uint8_t index);

  void getPayloadLen();
  uint64_t longlongPL();
};
