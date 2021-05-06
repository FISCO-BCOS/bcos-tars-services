#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <tup/Tars.h>
#include <bcos-framework/libutilities/Common.h>

namespace bcostars {
namespace protocol {
class BufferWriterByteVector {
protected:
  mutable std::vector<bcos::byte> _buffer;
  bcos::byte *_buf;
  std::size_t _len;
  std::size_t _buf_len;
  std::function<bcos::byte *(BufferWriterByteVector &, size_t)> _reserve;

private:
  BufferWriterByteVector(const BufferWriterByteVector &);
  BufferWriterByteVector &operator=(const BufferWriterByteVector &buf);

public:
  BufferWriterByteVector() : _buf(NULL), _len(0), _buf_len(0) {
#ifndef GEN_PYTHON_MASK
    _reserve = [](BufferWriterByteVector &os, size_t len) {
      os._buffer.resize(len);
      return os._buffer.data();
    };
#endif
  }

  ~BufferWriterByteVector() {}

  void reset() { _len = 0; }

  void writeBuf(const bcos::byte *buf, size_t len) {
    TarsReserveBuf(*this, _len + len);
    memcpy(_buf + _len, buf, len);
    _len += len;
  }

  const std::vector<bcos::byte> &getByteBuffer() const {
    _buffer.resize(_len);
    return _buffer;
  }
  std::vector<bcos::byte> &getByteBuffer() {
    _buffer.resize(_len);
    return _buffer;
  }
  const bcos::byte *getBuffer() const { return _buf; }
  size_t getLength() const { return _len; }
  void swap(std::vector<bcos::byte> &v) {
    _buffer.resize(_len);
    v.swap(_buffer);
    _buf = NULL;
    _buf_len = 0;
    _len = 0;
  }
  void swap(BufferWriterByteVector &buf) {
    buf._buffer.swap(_buffer);
    std::swap(_buf, buf._buf);
    std::swap(_buf_len, buf._buf_len);
    std::swap(_len, buf._len);
  }
};
} // namespace protocol
} // namespace bcostars