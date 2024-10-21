#pragma once

#include <cstddef>
#include <cstdint>


class WSFrame {
public:

  WSFrame(const uint8_t *buffer, size_t len);
  ~WSFrame();
  WSFrame &operator=(WSFrame &) = default;
  WSFrame &operator=(WSFrame &&) = default;

private:
  uint8_t *buffer;
  size_t bufferLen;
};
