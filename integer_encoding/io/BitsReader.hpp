/*-----------------------------------------------------------------------------
 *  BitsReader.hpp - A read interface for a sequence of bits
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

#ifndef __BITSREADER_HPP__
#define __BITSREADER_HPP__

#include "encoding_internals.hpp"

namespace integer_encoding {
namespace internals {

class BitsReader {
 public:
  explicit BitsReader(const uint32_t *in,
                      uint64_t len);
  ~BitsReader() throw();

  uint32_t read_bits(uint32_t num);
  const uint32_t *pos() const;

 private:
  friend class VSE_R;

  const uint32_t  *in_;
  const uint32_t  *term_;
  uint64_t  buffer_;
  uint32_t  fill_;

  DISALLOW_COPY_AND_ASSIGN(BitsReader);
}; /* BitsReader */

} /* namespace: internals */
} /* namespace: integer_encoding */

#endif /* __BITSREADER_HPP__ */
