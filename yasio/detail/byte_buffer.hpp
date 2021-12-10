
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

/* The byte_buffer concepts
   a. The memory model same with std::string, std::vector<char>
   b. use c realloc/free to manage memory
   c. implemented operations:
      - resize(without fill)
      - detach(stl not support)
      - stl likes: insert, reserve, front, begin, end, push_back and etc.
*/

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <memory>
#include <vector>
#include <exception>

namespace yasio
{
template <typename _Elem>
class basic_byte_buffer final {
  static_assert(sizeof(_Elem) == 1, "The basic_byte_buffer only accept type which is char or unsigned char!");
public:
  using pointer       = _Elem*;
  using const_pointer = const _Elem*;
  using size_type     = size_t;

  basic_byte_buffer() {}
  basic_byte_buffer(size_t capacity) { reserve(capacity); }
  basic_byte_buffer(const _Elem* first, const _Elem* last) { assign(first, last); }
  basic_byte_buffer(size_t size, _Elem val) { resize(size, val); }
  basic_byte_buffer(const basic_byte_buffer& rhs) { assign(rhs.begin(), rhs.end()); };
  basic_byte_buffer(basic_byte_buffer&& rhs)
  {
    memcpy(this, &rhs, sizeof(rhs));
    memset(&rhs, 0, sizeof(rhs));
  }
  basic_byte_buffer(const std::vector<_Elem>& rhs) { assign(rhs.data(), rhs.data() + rhs.size()); }

  ~basic_byte_buffer() { _Tidy(); }

  basic_byte_buffer& operator=(const basic_byte_buffer& rhs) { return assign(rhs.begin(), rhs.end()); }
  basic_byte_buffer& operator=(basic_byte_buffer&& rhs) { return this->swap(rhs); }

  basic_byte_buffer& assign(const _Elem* first, const _Elem* last)
  {
    ptrdiff_t count = last - first;
    if (count > 0)
      memcpy(resize(count), first, count);
    else
      clear();
    return *this;
  }
  basic_byte_buffer& swap(basic_byte_buffer& rhs)
  {
    std::swap(_Myfirst, rhs._Myfirst);
    std::swap(_Mylast, rhs._Mylast);
    std::swap(_Myend, rhs._Myend);
    return *this;
  }
  void insert(_Elem* where, const void* first, const void* last)
  {
    ptrdiff_t count = (const _Elem*)last - (const _Elem*)first;
    if (count > 0)
    {
      auto offset   = where - this->begin();
      auto old_size = size();
      resize(old_size + count);

      if (offset >= static_cast<ptrdiff_t>(old_size))
        memcpy(_Myfirst + old_size, first, count);
      else if (offset >= 0)
      {
        auto ptr = this->begin() + offset;
        auto to  = ptr + count;
        memmove(to, ptr, this->end() - to);
        memcpy(ptr, first, count);
      }
    }
  }
  void push_back(_Elem v)
  {
    auto old_size                  = size();
    resize(old_size + 1)[old_size] = v;
  }
  _Elem& front()
  {
    if (!empty())
      return *_Myfirst;
    else
      throw std::out_of_range("byte_buffer: out of range!");
  }
  _Elem* begin() { return _Myfirst; }
  _Elem* end() { return _Mylast; }
  const _Elem* begin() const { return _Myfirst; }
  const _Elem* end() const { return _Mylast; }
  pointer data() { return _Myfirst; }
  const_pointer data() const { return _Myfirst; }
  void reserve(size_t new_cap)
  {
    if (capacity() < new_cap)
      _Reset_cap(new_cap);
  }
  _Elem* resize(size_t new_size, _Elem val)
  {
    auto old_size = size();
    resize(new_size);
    if (old_size < new_size)
      memset(_Myfirst + old_size, val, new_size - old_size);
    return _Myfirst;
  }
  _Elem* resize(size_t new_size)
  {
    if (this->capacity() < new_size)
      _Reset_cap(new_size * 3 / 2);

    _Mylast = _Myfirst + new_size;
    return _Myfirst;
  }
  size_t capacity() const { return _Myend - _Myfirst; }
  size_t size() const { return _Mylast - _Myfirst; }
  void clear() { _Mylast = _Myfirst; }
  bool empty() const { return _Mylast == _Myfirst; }
  void shrink_to_fit() { _Reset_cap(size()); }
  template <typename _TSIZE>
  _Elem* detach(_TSIZE& out_size)
  {
    auto ptr = _Myfirst;
    out_size = static_cast<_TSIZE>(this->size());
    memset(this, 0, sizeof(*this));
    return ptr;
  }

private:
  void _Tidy()
  {
    clear();
    shrink_to_fit();
  }
  void _Reset_cap(size_t new_cap)
  {
    if (new_cap > 0)
    {
      auto new_blk = (_Elem*)realloc(_Myfirst, new_cap);
      if (new_blk)
      {
        _Myfirst = new_blk;
        _Myend   = _Myfirst + new_cap;
      }
      else
        throw std::bad_alloc();
    }
    else
    {
      if (_Myfirst != nullptr)
      {
        free(_Myfirst);
        _Myfirst = nullptr;
      }
      _Myend = _Myfirst;
    }
  }

private:
  _Elem* _Myfirst = nullptr;
  _Elem* _Mylast  = nullptr;
  _Elem* _Myend   = nullptr;
};
using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<uint8_t>;
} // namespace yasio