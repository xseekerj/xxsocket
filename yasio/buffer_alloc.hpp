//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

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

#ifndef YASIO__BUFFER_ALLOC_HPP
#define YASIO__BUFFER_ALLOC_HPP
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdexcept>
#include <utility>
#include "yasio/type_traits.hpp"

#define _YASIO_VERIFY_RANGE(cond, mesg)                 \
  do                                                    \
  {                                                     \
    if (cond)                                           \
      ; /* contextually convertible to bool paranoia */ \
    else                                                \
    {                                                   \
      throw std::out_of_range(mesg);                    \
    }                                                   \
                                                        \
  } while (false)

namespace yasio
{
using uint = unsigned int;
template <typename _Elem, enable_if_t<std::is_trivially_copyable<_Elem>::value, int> = 0>
struct default_buffer_allocator {
  static _Elem* reallocate(void* block, size_t /*size*/, size_t new_size) { return static_cast<_Elem*>(::realloc(block, new_size * sizeof(_Elem))); }
  static void deallocate(void* block, size_t /*size*/) { ::free(block); }
};
template <typename _Elem, enable_if_t<std::is_trivially_copyable<_Elem>::value, int> = 0>
struct std_buffer_allocator {
  static _Elem* reallocate(void* block, size_t size, size_t new_size)
  {
    if (!block)
      return new (std::nothrow) _Elem[new_size];
    void* new_block = nullptr;
    if (new_size)
    {
      if (new_size <= size)
        return block;
      new_block = new (std::nothrow) _Elem[new_size];
      if (new_block)
        memcpy(new_block, block, size);
    }
    delete[] (_Elem*)block;
    return (_Elem*)new_block;
  }
  static void deallocate(void* block, size_t /*size*/) { delete[] (_Elem*)block; }
};
template <typename _Ty, bool = std::is_trivially_constructible<_Ty>::value /* trivially_constructible */>
struct construct_helper {
  template <typename... Args>
  static _Ty* construct_at(_Ty* p, Args&&... args)
  {
    return ::new (static_cast<void*>(p)) _Ty{std::forward<Args>(args)...};
  }
};
template <typename _Ty /* not trivially_constructible */>
struct construct_helper<_Ty, false> {
  template <typename... Args>
  static _Ty* construct_at(_Ty* p, Args&&... args)
  {
    return ::new (static_cast<void*>(p)) _Ty(std::forward<Args>(args)...);
  }
};
} // namespace yasio

#endif