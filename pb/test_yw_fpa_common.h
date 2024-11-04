#include "yw_fpa_common.h"

void dump_buffer(const char *title, const uint8_t *buffer, size_t size) {
    printf("%s:\n", title);
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

TEST(FpaParseCommonTest, ValidInput) {
    // Test case 1: Valid input
    // 使用 ywfpa_SegmentUpdated 填充测试数据，序列化数据后，将数据填充到 valid_input 中
    ywfpa_SegmentUpdated segment_updated;
    segment_updated.has_cmd_header = true;
    segment_updated.cmd_header.magic = YWFPA_CMD_HEADER_MAGIC;
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
    ywfpa_CmdId result = fpa_get_command(buffer, valid_len);
    EXPECT_EQ(result, ywfpa_CmdId_CmdSegmentUpdated);
}

TEST(FpaParseCommonTest, TestAllMessages) {
    // Test ywfpa_CommonMsg
    {
        ywfpa_CommonMsg msg = ywfpa_CommonMsg_init_zero;
        msg.has_cmd_header = true;
        msg.cmd_header.magic = YWFPA_CMD_HEADER_MAGIC;
        msg.cmd_header.cmd = ywfpa_CmdId_CmdAlive;
        msg.cmd_header.seq = 1;
        msg.cmd_header.seq_org = 0;
        msg.cmd_header.rc = 0;
        msg.param1 = 123;
        msg.param2 = 456;

        uint8_t buffer[100];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status = pb_encode(&stream, ywfpa_CommonMsg_fields, &msg);
        EXPECT_TRUE(status);

        int len = stream.bytes_written;
        dump_buffer("CommonMsg", buffer, len);

        ywfpa_CmdId cmd_id = fpa_get_command(buffer, len);
        EXPECT_EQ(cmd_id, ywfpa_CmdId_CmdAlive);

        ywfpa_CommonMsg decoded_msg = ywfpa_CommonMsg_init_zero;
        status = fpa_decode_by_command(buffer, len, cmd_id, &decoded_msg);
        EXPECT_TRUE(status);
        EXPECT_TRUE(decoded_msg.has_cmd_header);
        EXPECT_EQ(decoded_msg.cmd_header.magic, YWFPA_CMD_HEADER_MAGIC);
        EXPECT_EQ(decoded_msg.cmd_header.cmd, ywfpa_CmdId_CmdAlive);
        EXPECT_EQ(decoded_msg.param1, 123);
        EXPECT_EQ(decoded_msg.param2, 456);
    }

    // Test ywfpa_DevAdded
    {
        ywfpa_DevAdded msg = ywfpa_DevAdded_init_zero;
        msg.has_cmd_header = true;
        msg.cmd_header.magic = YWFPA_CMD_HEADER_MAGIC;
        msg.cmd_header.cmd = ywfpa_CmdId_CmdDevAdded;
        msg.cmd_header.seq = 1;
        msg.cmd_header.seq_org = 0;
        msg.cmd_header.rc = 0;
        msg.dev_id = 12345;

        uint8_t buffer[100];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status = pb_encode(&stream, ywfpa_DevAdded_fields, &msg);
        EXPECT_TRUE(status);

        int len = stream.bytes_written;
        dump_buffer("DevAdded", buffer, len);

        ywfpa_CmdId cmd_id = fpa_get_command(buffer, len);
        EXPECT_EQ(cmd_id, ywfpa_CmdId_CmdDevAdded);

        ywfpa_DevAdded decoded_msg = ywfpa_DevAdded_init_zero;
        status = fpa_decode_by_command(buffer, len, cmd_id, &decoded_msg);
        EXPECT_TRUE(status);
        EXPECT_TRUE(decoded_msg.has_cmd_header);
        EXPECT_EQ(decoded_msg.cmd_header.magic, YWFPA_CMD_HEADER_MAGIC);
        EXPECT_EQ(decoded_msg.cmd_header.cmd, ywfpa_CmdId_CmdDevAdded);
        EXPECT_EQ(decoded_msg.dev_id, 12345);
    }

    // Test ywfpa_DevRemoved
    {
        ywfpa_DevRemoved msg = ywfpa_DevRemoved_init_zero;
        msg.has_cmd_header = true;
        msg.cmd_header.magic = YWFPA_CMD_HEADER_MAGIC;
        msg.cmd_header.cmd = ywfpa_CmdId_CmdDevRemoved;
        msg.cmd_header.seq = 1;
        msg.cmd_header.seq_org = 0;
        msg.cmd_header.rc = 0;
        msg.dev_id = 12345;

        uint8_t buffer[100];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status = pb_encode(&stream, ywfpa_DevRemoved_fields, &msg);
        EXPECT_TRUE(status);

        int len = stream.bytes_written;
        dump_buffer("DevRemoved", buffer, len);

        ywfpa_CmdId cmd_id = fpa_get_command(buffer, len);
        EXPECT_EQ(cmd_id, ywfpa_CmdId_CmdDevRemoved);

        ywfpa_DevRemoved decoded_msg = ywfpa_DevRemoved_init_zero;
        status = fpa_decode_by_command(buffer, len, cmd_id, &decoded_msg);
        EXPECT_TRUE(status);
        EXPECT_TRUE(decoded_msg.has_cmd_header);
        EXPECT_EQ(decoded_msg.cmd_header.magic, YWFPA_CMD_HEADER_MAGIC);
        EXPECT_EQ(decoded_msg.cmd_header.cmd, ywfpa_CmdId_CmdDevRemoved);
        EXPECT_EQ(decoded_msg.dev_id, 12345);
    }

    // Test ywfpa_SegmentUpdated
    {
        ywfpa_SegmentUpdated msg = ywfpa_SegmentUpdated_init_zero;
        msg.has_cmd_header = true;
        msg.cmd_header.magic = YWFPA_CMD_HEADER_MAGIC;
        msg.cmd_header.cmd = ywfpa_CmdId_CmdSegmentUpdated;
        msg.cmd_header.seq = 1;
        msg.cmd_header.seq_org = 0;
        msg.cmd_header.rc = 0;
        msg.has_seg_info = true;
        msg.seg_info.dev_id = 12345;
        msg.seg_info.seg_id = 67890;
        msg.seg_info.begin_time = 1609459200; // 2021-01-01 00:00:00 UTC
        msg.seg_info.end_time = 1609462800; // 2021-01-01 01:00:00 UTC
        msg.seg_info.media_type_mask = 0xFF;
        msg.seg_info.ch_mask = 0xFF;

        uint8_t buffer[100];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status = pb_encode(&stream, ywfpa_SegmentUpdated_fields, &msg);
        EXPECT_TRUE(status);

        int len = stream.bytes_written;
        dump_buffer("SegmentUpdated", buffer, len);

        ywfpa_CmdId cmd_id = fpa_get_command(buffer, len);
        EXPECT_EQ(cmd_id, ywfpa_CmdId_CmdSegmentUpdated);

        ywfpa_SegmentUpdated decoded_msg = ywfpa_SegmentUpdated_init_zero;
        status = fpa_decode_by_command(buffer, len, cmd_id, &decoded_msg);
        EXPECT_TRUE(status);
        EXPECT_TRUE(decoded_msg.has_cmd_header);
        EXPECT_EQ(decoded_msg.cmd_header.magic, YWFPA_CMD_HEADER_MAGIC);
        EXPECT_EQ(decoded_msg.cmd_header.cmd, ywfpa_CmdId_CmdSegmentUpdated);
        EXPECT_EQ(decoded_msg.seg_info.dev_id, 12345);
        EXPECT_EQ(decoded_msg.seg_info.seg_id, 67890);
        EXPECT_EQ(decoded_msg.seg_info.begin_time, 1609459200);
        EXPECT_EQ(decoded_msg.seg_info.end_time, 1609462800);
        EXPECT_EQ(decoded_msg.seg_info.media_type_mask, 0xFF);
        EXPECT_EQ(decoded_msg.seg_info.ch_mask, 0xFF);
    }
}
