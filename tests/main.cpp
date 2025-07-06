#include <gtest/gtest.h>

#include "utils/snapshot.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    for (size_t index = 0; index < argc; ++index) {
        if (std::string(argv[index]) == "--update_snapshots") {
            UPDATE_SNAPSHOTS = true;
        }
    }

    return RUN_ALL_TESTS();
}