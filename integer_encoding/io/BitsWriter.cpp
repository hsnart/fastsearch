/*-----------------------------------------------------------------------------
 *  BitsWriter.cpp - A code to write compressed data during encoding
 *
 *  Coding-Style: google-styleguide
 *      https://code.google.com/p/google-styleguide/
 *
 *  Authors:
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *
 *  Copyright 2012 Integer Encoding Library <integerencoding_at_isti.cnr.it>
 *      http://integerencoding.ist.cnr.it/
 *-----------------------------------------------------------------------------
 */

#include "BitsWriter.hpp"

namespace integer_encoding {
namespace internals {

BitsWriter::BitsWriter(uint32_t *out,
                       uint64_t len)
    : out_(out),
      term_(out + len),
      buffer_(0),
      fill_(0),
      nwritten_(0) {
  if (out == NULL)
    THROW_ENCODING_EXCEPTION("Invalid value: out");
  if (len == 0)
    THROW_ENCODING_EXCEPTION("Invalid value: len");

  ASSERT_ADDR(out, len);
}

BitsWriter::~BitsWriter() throw() {}

void BitsWriter::write_bits(uint32_t val,
                            uint32_t num) {
  if (num > 32)
    THROW_ENCODING_EXCEPTION("Out of range exception: num");

  ASSERT(out_ != NULL);

  if (LIKELY(num > 0)) {
    if (out_ >= term_)
      THROW_ENCODING_EXCEPTION("Buffer-overflowed exception");

    buffer_ = (buffer_ << num) |
        (val & ((uint64_t(1) << num) - 1));
    fill_ += num;

    if (fill_ >= 32) {
      BYTEORDER_FREE_STORE32(
          out_, (buffer_ >> (fill_ - 32)) &
                    ((uint64_t(1) << 32) - 1));
      out_++, nwritten_++, fill_ -= 32;
      if (out_ >= term_&& fill_ > 0)
        THROW_ENCODING_EXCEPTION("Buffer-overflowed exception");
    }
  }
}

void BitsWriter::flush_bits() {
  if (out_ >= term_ && fill_ > 0)
    THROW_ENCODING_EXCEPTION("Buffer-overflowed exception");

  ASSERT(out_ != NULL);
  ASSERT(fill_ < 32);

  if (fill_ > 0) {
    BYTEORDER_FREE_STORE32(
      out_, buffer_ << (32 - fill_) &
                ((uint64_t(1) << 32) - 1));
    out_++, nwritten_++;
    buffer_ = 0, fill_ = 0;
  }
}

uint32_t *BitsWriter::pos() const {
  ASSERT(out_ != NULL);
  return out_;
}

uint64_t BitsWriter::size() const {
  return nwritten_;
}

} /* namespace: internals */
} /* namespace: integer_encoding */
