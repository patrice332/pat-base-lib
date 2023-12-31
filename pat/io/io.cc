#include "pat/io/io.h"

namespace pat::io {

namespace {
struct IoErrorCategoryType : std::error_category {
    const char* name() const noexcept override { return "io"; }
    std::string message(int error_value) const override;
};

std::string IoErrorCategoryType::message(int error_value) const {
    switch (static_cast<IoError>(error_value)) {
        case IoError::kSuccess:
            return "Success";
        case IoError::kEOF:
            return "EOF";
        case IoError::kClosedPipe:
            return "io: read/write on closed pipe";
        case IoError::kNoProgress:
            return "multiple Read calls return no data or error";
        case IoError::kShortBuffer:
            return "short buffer";
        case IoError::kShortWrite:
            return "short write";
        case IoError::kUnexpectedEOF:
            return "unexpected EOF";
    }
}

const IoErrorCategoryType IoErrorCategory{};

}  // namespace

std::error_code make_error_code(IoError error_code) {
    return {static_cast<int>(error_code), IoErrorCategory};
}

}  // namespace pat::io