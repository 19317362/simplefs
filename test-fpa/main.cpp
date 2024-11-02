#include <gtest/gtest.h>


#include "../pb/test_yw_fpa.pb.h"

TEST(FpaTest, DevAdded) {
  // Expect two strings to be equal.
    test_ywfpa_DevAdded_init();
    test_ywfpa_DevAdded_values();  
}
// ywfpa_SegmentUpdated 的序列化和反序列化测试
TEST(FpaTest, SegmentUpdated) {
    test_ywfpa_SegmentUpdated_init();
    test_ywfpa_SegmentUpdated_values();  
}
TEST(FpaTest, ywfpa_DevRemoved) {
    test_ywfpa_DevRemoved_init();
    test_ywfpa_DevRemoved_values();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
