#pragma once

#include "WSFrame.h"
#include <cstddef>
#include <cstdint>
#include <iostream>

class WSMessage {
public:
  size_t m_size;
  uint8_t *m_msg;
  OPCODE m_type;

  WSMessage();
  WSMessage(uint8_t *msg, size_t size, OPCODE type);

  friend std::ostream &operator<<(std::ostream &os, const WSMessage &obj);
};
