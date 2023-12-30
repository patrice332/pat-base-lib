#include "pat/bytes/buffer.h"

#include "gtest/gtest.h"
#include "pat/io/io.h"

namespace pat::bytes {

namespace testing {
std::size_t* alloc_counter = nullptr;
}

namespace {

class BufferTesting : public ::testing::Test {
   public:
    BufferTesting() { testing::alloc_counter = &allocations_; }
    BufferTesting(const BufferTesting&) = delete;
    BufferTesting& operator=(const BufferTesting&) = delete;
    BufferTesting(BufferTesting&&) = delete;
    BufferTesting& operator=(BufferTesting&&) = delete;
    ~BufferTesting() { testing::alloc_counter = nullptr; }

   protected:
    constexpr std::size_t Allocations() const { return allocations_; }

   private:
    std::size_t allocations_{0};
};

TEST_F(BufferTesting, asserts) {
    EXPECT_TRUE(io::Reader<Buffer>);
    EXPECT_TRUE(io::Writer<Buffer>);
}

TEST_F(BufferTesting, empty) { Buffer buf; }

TEST_F(BufferTesting, StringWriteWithSpace) {
    constexpr std::string_view kMsg = "This is a test string!!!";
    std::string str;
    str.reserve(kMsg.size());
    Buffer buf{std::move(str)};
    buf.Write(kMsg);
    EXPECT_EQ(Allocations(), 0UL);
}

TEST_F(BufferTesting, StringWriteWithoutSpace) {
    constexpr std::string_view kMsg = "This is a test string!!!";
    Buffer buf{};
    buf.Write(kMsg);
    EXPECT_EQ(Allocations(), 1UL);
}

TEST_F(BufferTesting, StringWriteWithSpaceSparse) {
    constexpr std::string_view kMsg = "This is a test string!!!";
    std::string str;
    constexpr std::size_t kReadSize = kMsg.size() - 4;
    str.reserve(kMsg.size() + 4);
    std::array<char, kReadSize> read_buf{};
    Buffer buf{std::move(str)};
    buf.Write(kMsg);
    buf.Read(read_buf);
    EXPECT_EQ(std::string_view{read_buf}, "This is a test strin");
    buf.Write(kMsg);
    EXPECT_EQ(Allocations(), 0UL);
}

TEST_F(BufferTesting, StringViewWrite) {
    constexpr std::string_view kMsg = "This is a test string!!!";
    std::array<char, kMsg.size()> read_buf{};
    Buffer buf{kMsg};
    buf.Read(read_buf);
    EXPECT_EQ(std::string_view{read_buf}, kMsg);
    EXPECT_EQ(Allocations(), 0UL);
    EXPECT_EQ(buf.Size(), 0);
    EXPECT_EQ(buf.Capacity(), kMsg.size());
    buf.Write(kMsg);
    buf.Read(read_buf);
    EXPECT_EQ(std::string_view{read_buf}, kMsg);
    EXPECT_EQ(Allocations(), 1UL);
    EXPECT_EQ(buf.Size(), 0);
}

TEST_F(BufferTesting, StringViewMultiRead) {
    constexpr std::string_view kMsg = "This is a test string!!!!";
    std::array<char, 4> read_buf{};
    Buffer buf{kMsg};
    auto read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "This");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), " is ");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "a te");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "st s");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "trin");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "g!!!");
    read_bytes = buf.Read(read_buf);
    EXPECT_EQ((std::string_view{read_buf.data(), read_bytes.value_or(0)}), "!");
    EXPECT_EQ(buf.Size(), 0UL);
}

}  // namespace

}  // namespace pat::bytes