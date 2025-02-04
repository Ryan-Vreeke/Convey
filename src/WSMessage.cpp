#include "WSMessage.h"

#include "WSFrame.h"

using namespace Convey;
WSMessage::WSMessage() {}

WSMessage::WSMessage(uint8_t *msg, size_t size, OPCODE type)
    : m_size(size), m_msg(msg), m_type(type) {}

std::ostream &operator<<(std::ostream &os, const WSMessage &obj) {
  os << obj.m_msg;
  return os;
}
