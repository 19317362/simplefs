#include <gtest/gtest.h>

// 做一个通用的 printf dump buffer 函数
void dump_buffer(const char* desc, const uint8_t *buffer, int len) {
    printf("%s:\n", desc);
    for(int i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
        if(i &0xF == 0)
        { 
            printf("\n");
        }
    }
    printf("\n");
}

#include "../pb/test_yw_fpa.pb.h"
#include "../pb/test_yw_fpa_common.h"

TEST(FpaTest, PkgHeader) {
  // Expect two strings to be equal.
    test_ywfpa_PkgHeader_init();
    test_ywfpa_PkgHeader_values();  
}
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
