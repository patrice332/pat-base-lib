#include "pat/runtime/stream.h"

namespace pat::runtime {

Stream::Stream(Stream&& other) noexcept = default;
Stream& Stream::operator=(Stream&& other) noexcept = default;
Stream::~Stream() = default;

}  // namespace pat::runtime