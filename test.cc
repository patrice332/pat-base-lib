#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "pat/runtime/file.h"
#include "pat/runtime/io_context.h"
#include "unifex/just.hpp"
#include "unifex/let_value.hpp"
#include "unifex/sync_wait.hpp"
#include "unifex/then.hpp"

int main() {
    auto io_context = pat::runtime::IOContext::Create();

    auto res = unifex::let_value(unifex::just(), [&io_context]() {
        return pat::runtime::File::Open(io_context, "/tmp/test.out", O_CREAT | O_CLOEXEC | O_RDWR,
                                        0777);
    });

    auto res_s = unifex::sync_wait(res);

    return 0;
}