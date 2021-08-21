//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YASIO__OBSTREAM_HPP
#define YASIO__OBSTREAM_HPP
#include <stddef.h>
#include <vector>
#include <array>
#include <limits>
#include <stack>
#include <fstream>
#include "yasio/cxx17/string_view.hpp"
#include "yasio/detail/endian_portable.hpp"
#include "yasio/detail/utils.hpp"
namespace yasio
{
namespace detail
{
template <typename _Stream, typename _Intty>
inline void write_ix_impl(_Stream* stream, _Intty value)
{
  // Write out an int 7 bits at a time.  The high bit of the byte,
  // when on, tells reader to continue reading more bytes.
  auto v = (typename std::make_unsigned<_Intty>::type)value; // support negative numbers
  while (v >= 0x80)
  {
    stream->write_byte((uint8_t)((uint32_t)v | 0x80));
    v >>= 7;
  }
  stream->write_byte((uint8_t)v);
}
template <typename _Stream, typename _Intty>
struct write_ix_helper {};

template <typename _Stream>
struct write_ix_helper<_Stream, int32_t> {
  static void write_ix(_Stream* stream, int32_t value) { write_ix_impl<_Stream, int32_t>(stream, value); }
};

template <typename _Stream>
struct write_ix_helper<_Stream, int64_t> {
  static void write_ix(_Stream* stream, int64_t value) { write_ix_impl<_Stream, int64_t>(stream, value); }
};
} // namespace detail

class fixed_buffer_view {
public:
  using implementation_type = fixed_buffer_view;
  implementation_type& get_implementation() { return *this; }
  const implementation_type& get_implementation() const { return *this; }

  template <size_t _N>
  fixed_buffer_view(std::array<char, _N>& buf) : first_(buf.data()), last_(buf.data() + _N)
  {}

  template <size_t _N>
  fixed_buffer_view(char (&buf)[_N]) : first_(buf), last_(buf + _N)
  {}

  fixed_buffer_view(char* buf, size_t n) : first_(buf), last_(buf + n) {}
  fixed_buffer_view(char* first, char* last) : first_(first), last_(last) {}

  void reserve(size_t /*capacity*/){};
  void resize(size_t newsize)
  {
    if (newsize < max_size())
      this->wpos_ = newsize;
    else
      throw std::out_of_range("const_buffer: out of range");
  }

  void write_byte(uint8_t value)
  {
    if (wpos_ < this->max_size())
    {
      this->data()[this->wpos_] = value;
      ++this->wpos_;
    }
  }

  void write_bytes(const void* d, int n)
  {
    if (n > 0)
      write_bytes(this->wpos_, d, n);
  }
  void write_bytes(size_t offset, const void* d, int n)
  {
    if ((offset + n) <= this->max_size())
    {
      ::memcpy(this->data() + offset, d, n);
      this->wpos_ += n;
    }
    else
      throw std::out_of_range("const_buffer: out of range");
  }
  void shrink_to_fit(){};
  void clear() { this->wpos_ = 0; }
  char* data() { return first_; }
  size_t length() const { return this->wpos_; }
  bool empty() const { return first_ == last_; }
  size_t max_size() const { return last_ - first_; }

private:
  char* first_;
  char* last_;
  size_t wpos_ = 0;
};

template <size_t _N>
class fixed_buffer : public fixed_buffer_view {
public:
  fixed_buffer() : fixed_buffer_view(impl_) {}

private:
  std::array<char, _N> impl_;
};

class dynamic_buffer {
public:
  using implementation_type = std::vector<char>;
  implementation_type& get_implementation() { return this->impl_; }
  const implementation_type& get_implementation() const { return this->impl_; }

  void reserve(size_t capacity) { impl_.reserve(capacity); }
  void resize(size_t newsize) { impl_.resize(newsize); }
  void write_byte(uint8_t value) { impl_.push_back(value); }

  void write_bytes(const void* d, int n)
  {
    if (n > 0)
      impl_.insert(impl_.end(), static_cast<const char*>(d), static_cast<const char*>(d) + n);
  }
  size_t write_bytes(size_t offset, const void* d, int n)
  {
    if ((offset + n) > impl_.size())
      impl_.resize(offset + n);

    ::memcpy(impl_.data() + offset, d, n);
    return n;
  }

  void shrink_to_fit(){};

  void clear() { impl_.clear(); }

  char* data() { return impl_.data(); }

  size_t length() const { return impl_.size(); }
  bool empty() const { return impl_.empty(); }

private:
  implementation_type impl_;
};

template <typename _ConvertTraits, typename _BufferType = fixed_buffer_view>
class basic_obstream_view {
public:
  using convert_traits_type        = _ConvertTraits;
  using buffer_type                = _BufferType;
  using buffer_implementation_type = typename buffer_type::implementation_type;
  using this_type                  = basic_obstream_view<convert_traits_type, buffer_type>;

  static const size_t npos = -1;

  basic_obstream_view(buffer_type* outs) : outs_(outs) {}
  ~basic_obstream_view() {}

  void push8()
  {
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint8_t>(0));
  }
  void pop8()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint8_t>(outs_->length() - offset - sizeof(uint8_t)));
    offset_stack_.pop();
  }
  void pop8(uint8_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push16()
  {
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint16_t>(0));
  }
  void pop16()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint16_t>(outs_->length() - offset - sizeof(uint16_t)));
    offset_stack_.pop();
  }
  void pop16(uint16_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push32()
  {
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint32_t>(0));
  }
  void pop32()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint32_t>(outs_->length() - offset - sizeof(uint32_t)));
    offset_stack_.pop();
  }
  void pop32(uint32_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto bufsize = outs_->length();
    offset_stack_.push(bufsize);
    outs_->resize(bufsize + size);
  }

  void pop(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    auto value  = static_cast<int>(outs_->length() - offset - size);
    value       = convert_traits_type::toint(value, size);
    write_bytes(offset, &value, size);
    offset_stack_.pop();
  }

  void pop(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    value       = convert_traits_type::toint(value, size);
    write_bytes(offset, &value, size);
    offset_stack_.pop();
  }

  /* write blob data with '7bit encoded int' length field */
  void write_v(cxx17::string_view value)
  {
    int len = static_cast<int>(value.length());
    write_ix(len);
    write_bytes(value.data(), len);
  }

  /* 32 bits length field */
  void write_v32(cxx17::string_view value) { write_v_fx<uint32_t>(value); }
  /* 16 bits length field */
  void write_v16(cxx17::string_view value) { write_v_fx<uint16_t>(value); }
  /* 8 bits length field */
  void write_v8(cxx17::string_view value) { write_v_fx<uint8_t>(value); }

  void write_byte(uint8_t value) { outs_->write_byte(value); }

  void write_bytes(cxx17::string_view v) { return write_bytes(v.data(), static_cast<int>(v.size())); }
  void write_bytes(const void* d, int n) { outs_->write_bytes(d, n); }
  void write_bytes(size_t offset, const void* d, int n) { outs_->write_bytes(offset, d, n); }

  bool empty() const { return outs_->empty(); }
  size_t length() const { return outs_->length(); }
  const char* data() const { return outs_->data(); }
  char* data() { return outs_->data(); }

  const buffer_implementation_type& buffer() const { return outs_->get_implementation(); }
  buffer_implementation_type& buffer() { return outs_->get_implementation(); }

  void clear()
  {
    outs_->clear();
    std::stack<size_t> tmp;
    tmp.swap(offset_stack_);
  }
  void shrink_to_fit() { outs_->shrink_to_fit(); }

  template <typename _Nty>
  inline void write(_Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    write_bytes(&nv, sizeof(nv));
  }

  template <typename _Intty>
  void write_ix(_Intty value)
  {
    detail::write_ix_helper<this_type, _Intty>::write_ix(this, value);
  }

  void write_varint(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    value = convert_traits_type::toint(value, size);
    write_bytes(&value, size);
  }

  template <typename _Nty>
  inline void pwrite(ptrdiff_t offset, const _Nty value)
  {
    swrite(this->data() + offset, value);
  }
  template <typename _Nty>
  static void swrite(void* ptr, const _Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    ::memcpy(ptr, &nv, sizeof(nv));
  }

  void save(const char* filename) const
  {
    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    if (!this->empty())
      fout.write(this->data(), this->length());
  }

private:
  template <typename _LenT>
  inline void write_v_fx(cxx17::string_view value)
  {
    int size = static_cast<int>(value.size());
    this->write<_LenT>(static_cast<_LenT>(size));
    if (size)
      write_bytes(value.data(), size);
  }

  template <typename _Intty>
  inline void write_ix_impl(_Intty value)
  {
    // Write out an int 7 bits at a time.  The high bit of the byte,
    // when on, tells reader to continue reading more bytes.
    auto v = (typename std::make_unsigned<_Intty>::type)value; // support negative numbers
    while (v >= 0x80)
    {
      write_byte((uint8_t)((uint32_t)v | 0x80));
      v >>= 7;
    }
    write_byte((uint8_t)v);
  }

protected:
  buffer_type* outs_;
  std::stack<size_t> offset_stack_;
}; // CLASS basic_obstream

enum
{
  dynamic_extent = -1
};

template <typename _ConvertTraits, size_t _Extent = dynamic_extent>
class basic_obstream;

template <typename _ConvertTraits, size_t _Extent>
class basic_obstream : public basic_obstream_view<_ConvertTraits, fixed_buffer<_Extent>> {
public:
  basic_obstream() : basic_obstream_view(&buffer_) {}

protected:
  buffer_type buffer_;
};

template <typename _ConvertTraits>
class basic_obstream<_ConvertTraits, dynamic_extent> : public basic_obstream_view<_ConvertTraits, dynamic_buffer> {
public:
  basic_obstream(size_t capacity = 128) : basic_obstream_view(&buffer_) { buffer_.reserve(capacity); }
  basic_obstream(const basic_obstream& rhs) : basic_obstream_view(&buffer_), buffer_(rhs.buffer_) {}
  basic_obstream(basic_obstream&& rhs) : basic_obstream_view(&buffer_), buffer_(std::move(rhs.buffer_)) {}
  basic_obstream& operator=(const basic_obstream& rhs)
  {
    buffer_ = rhs.buffer_;
    return *this;
  }
  basic_obstream& operator=(basic_obstream&& rhs)
  {
    buffer_ = std::move(rhs.buffer_);
    return *this;
  }

  basic_obstream sub(size_t offset, size_t count = npos)
  {
    basic_obstream obs;
    auto n = length();
    if (offset < n)
    {
      if (count > (n - offset))
        count = (n - offset);

      obs.write_bytes(this->data() + offset, count);
    }
    return obs;
  }

protected:
  buffer_type buffer_;
};

using obstream_view = basic_obstream_view<convert_traits<network_convert_tag>>;
template <size_t _Extent>
using obstream_span = basic_obstream<convert_traits<network_convert_tag>, _Extent>;
using obstream      = obstream_span<dynamic_extent>;

using fast_obstream_view = basic_obstream_view<convert_traits<host_convert_tag>>;

template <size_t _Extent>
using fast_obstream_span = basic_obstream<convert_traits<host_convert_tag>, _Extent>;
using fast_obstream      = fast_obstream_span<dynamic_extent>;

} // namespace yasio

#endif
