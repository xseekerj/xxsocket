#pragma once
#include "yasio/config.hpp"
#include "yasio/byte_buffer.hpp"
#include "yasio/endian_portable.hpp"
#include <limits>

namespace yasio
{
struct frame_options {
  int max_frame_size         = (std::numeric_limits<int>::max)();
  int length_field_offset    = 0;
  int length_field_length    = 4; // 0,1,2,3,4, 0 means no length field
  int length_adjustment      = 0; // if the length field value not whole packet
  int initial_bytes_to_strip = 0;
  int byte_order             = 1;
};
class frame {
public:
  frame(const frame_options& opts) : opts_(opts), max_parsing_offset_(opts.length_field_offset + opts.length_field_length) {}

  void input(const void* d, size_t size)
  {
    const char* ptr = static_cast<const char*>(d);
    int avail       = static_cast<int>(size);
    int offset      = 0;
    do
    {
      if (max_parsing_offset_ > 0)
      { // frame mode
        if (!is_size_parsed())
        {
          int read_size = (std::min)(avail, max_parsing_offset_ - parsing_offset_);
          if (!parse_size(ptr, read_size, avail))
            break;
          if (!is_size_parsed())
            break; // wait pending frame data
        }

        int read_size = (std::min)(frame_size_ - cur_size_, avail);
        if (read_size > 0)
          consume(ptr, read_size, avail);
        // else: wait pending fame data
      }
      else
      { // stream mode
        frame_size_ = avail;
        consume(ptr, avail, avail);
      }
      if (cur_size_ == frame_size_)
        flush();
    } while (avail > 0);
  }

private:
  void consume(const char*& ptr, int n, int& avail)
  {
    // on_data
    cur_size_ += n;

    ptr += n;
    avail -= n;
  }

  void flush()
  {
    // on_flush
    parsing_offset_ = 0;
    frame_size_     = 0;
    cur_size_       = 0;
  }

  bool parse_size(const char*& ptr, int n, int& avail)
  {
    const int field_offset = opts_.length_field_offset;
    const int advance      = (std::min)(field_offset - parsing_offset_, n);
    if (advance > 0)
    {
      consume(ptr, advance, n);
      avail -= advance;
      parsing_offset_ += advance;
    }
    if (n > 0)
    {
      const auto offset = parsing_offset_ - field_offset;
      if (offset + n > sizeof(frame_size_))
      {
        return false;
      }
      memcpy((uint8_t*)&frame_size_ + offset, ptr, n);
      consume(ptr, n, avail);

      parsing_offset_ += n;
      if (parsing_offset_ == max_parsing_offset_)
      {
        frame_size_ = yasio::network_to_host(frame_size_, parsing_offset_);
        frame_size_ += static_cast<int>(opts_.length_adjustment);

        if (frame_size_ > opts_.max_frame_size)
        {
          return false;
        }
      }
    } // else: wait pending frame data

    return true;
  }

  bool is_size_parsed() const { return parsing_offset_ == max_parsing_offset_; }

  const frame_options& opts_;
  const int max_parsing_offset_;
  int parsing_offset_{0};
  int frame_size_ = 0;
  int cur_size_   = 0;
};
} // namespace yasio
