/*-----------------------------------------------------------------------------
 *  BitsReader.cpp - A code to read compressed data for decoding
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

#include "BitsReader.hpp"

namespace integer_encoding {
namespace internals {

namespace {

#define BITSRD_BUFFILL(__cond__)  \
    ASSERT(__cond__ <= 32);       \
    ASSERT(in_ != NULL);          \
    if (fill_ < __cond__) {       \
      buffer_ = (buffer_ << 32) | BYTEORDER_FREE_LOAD32(in_);     \
      in_++, fill_ += 32;         \
      if (UNLIKELY(in_ > term_))  \
        THROW_ENCODING_EXCEPTION("Buffer-overflowed exception");  \
    }

const uint32_t F_MASK32 = 0xffff;

}; /* namespace: */

BitsReader::BitsReader(const uint32_t *in,
                       uint64_t len)
    : in_(in),
      term_(in + len),
      buffer_(0),
      fill_(0) {
  if (in == NULL)
    THROW_ENCODING_EXCEPTION("Invalid value: in");
  if (len == 0)
    THROW_ENCODING_EXCEPTION("Invalid value: len");

  ASSERT_ADDR(in, len);
}

BitsReader::~BitsReader() throw() {}

uint32_t BitsReader::read_bits(uint32_t num) {
  if (UNLIKELY(num == 0))
    return 0;
  ASSERT(num <= 32);
  BITSRD_BUFFILL(num);
  fill_ -= num;
  return (buffer_ >> fill_) &
      ((uint64_t(1) << num) - 1);
}

const uint32_t *BitsReader::pos() const {
  ASSERT(in_ != NULL);
  return in_ - 1;
}

} /* namespace: internals */
} /* namespace: integer_encoding */
