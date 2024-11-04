#include "yw_fpa_common.h"

TEST(FpaParseCommonTest, ValidInput) {
    // Test case 1: Valid input
    // 使用 ywfpa_SegmentUpdated 填充测试数据，序列化数据后，将数据填充到 valid_input 中
    ywfpa_SegmentUpdated segment_updated;
    segment_updated.has_cmd_header = true;
    segment_updated.cmd_header.magic = 0xFEADBEEF;
    segment_updated.cmd_header.cmd = ywfpa_CmdId_CmdSegmentUpdated;
    segment_updated.cmd_header.seq = 1;
    segment_updated.cmd_header.seq_org = 0;
    segment_updated.cmd_header.rc = 0;
    segment_updated.has_seg_info = true;
    segment_updated.seg_info.dev_id = 12345;
    segment_updated.seg_info.seg_id = 67890;
    segment_updated.seg_info.begin_time = 1609459200; // 2021-01-01 00:00:00 UTC
    segment_updated.seg_info.end_time = 1609462800; // 2021-01-01 01:00:00 UTC
    segment_updated.seg_info.media_type_mask = 0xFF;
    segment_updated.seg_info.ch_mask = 0xFF;

    // 序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_SegmentUpdated_fields, &segment_updated);
    EXPECT_TRUE(status);


    int valid_len = stream.bytes_written;
    printf("valid_len = %d\n", valid_len);
    dump_buffer("fpa_parse_common", buffer, valid_len);    
    ywfpa_CmdId result = fpa_parse_common(buffer, valid_len);
    EXPECT_EQ(result, ywfpa_CmdId_CmdSegmentUpdated);
}

// TEST(FpaParseCommonTest, InvalidInputEmptyBuffer) {
//     // Test case 2: Invalid input (empty buffer)
//     unsigned char invalid_input[] = {};
//     int invalid_len = sizeof(invalid_input);
//     ywfpa_CmdId result = fpa_parse_common(invalid_input, invalid_len);
//     EXPECT_EQ(result, ywfpa_CmdId_CmdNA);
// }

// TEST(FpaParseCommonTest, InvalidInputCorruptedData) {
//     // Test case 3: Invalid input (corrupted data)
//     unsigned char corrupted_input[] = { /* Add corrupted data here */ };
//     int corrupted_len = sizeof(corrupted_input);
//     ywfpa_CmdId result = fpa_parse_common(corrupted_input, corrupted_len);
//     EXPECT_EQ(result, ywfpa_CmdId_CmdNA);
// }
