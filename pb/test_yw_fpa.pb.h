#ifndef PB_TEST_YWFPA_YW_FPA_PB_H_INCLUDED
#define PB_TEST_YWFPA_YW_FPA_PB_H_INCLUDED
#include <pb_encode.h>
#include <pb_decode.h>
#include "yw_fpa.pb.h"
#include <gtest/gtest.h>
#include <stdio.h>

void test_ywfpa_DevAdded_init() {
    ywfpa_DevAdded dev_added = ywfpa_DevAdded_init_default;
    EXPECT_FALSE(dev_added.has_cmd_header);
    EXPECT_FALSE(dev_added.has_cmd_header);
    EXPECT_EQ(dev_added.cmd_header.magic, 0);
    EXPECT_EQ(dev_added.cmd_header.cmd, 0);
    EXPECT_EQ(dev_added.cmd_header.seq, 0);
    EXPECT_EQ(dev_added.cmd_header.seq_org, 0);
    EXPECT_EQ(dev_added.cmd_header.rc, 0);
    EXPECT_EQ(dev_added.dev_id, 0);
    printf("test_ywfpa_DevAdded_init passed\n");
}

void test_ywfpa_DevAdded_values() {
    ywfpa_DevAdded dev_added = ywfpa_DevAdded_init_zero;
    dev_added.cmd_header.magic = 0xFEADBEEF;
    dev_added.cmd_header.cmd = ywfpa_CmdId_CmdDevAdded;
    dev_added.cmd_header.seq = 1;
    dev_added.cmd_header.seq_org = 0;
    dev_added.cmd_header.rc = 0;
    dev_added.dev_id = 12345;

    EXPECT_EQ(dev_added.cmd_header.magic, 0xFEADBEEF);
    EXPECT_EQ(dev_added.cmd_header.cmd, ywfpa_CmdId_CmdDevAdded);
    EXPECT_EQ(dev_added.cmd_header.seq, 1);
    EXPECT_EQ(dev_added.cmd_header.seq_org, 0);
    EXPECT_EQ(dev_added.cmd_header.rc, 0);
    EXPECT_EQ(dev_added.dev_id, 12345);

    ywfpa_DevAdded dev_added2 = ywfpa_DevAdded_init_zero;
    dev_added2.cmd_header.magic = 0xFEADBEEF;
    dev_added2.cmd_header.cmd = ywfpa_CmdId_CmdDevAdded;
    dev_added2.cmd_header.seq = 2;
    dev_added2.cmd_header.seq_org = 1;
    dev_added2.cmd_header.rc = 0;
    dev_added2.dev_id = 54321;

    EXPECT_EQ(dev_added2.cmd_header.magic, 0xFEADBEEF);
    EXPECT_EQ(dev_added2.cmd_header.cmd, ywfpa_CmdId_CmdDevAdded);
    EXPECT_EQ(dev_added2.cmd_header.seq, 2);
    EXPECT_EQ(dev_added2.cmd_header.seq_org, 1);
    EXPECT_EQ(dev_added2.cmd_header.rc, 0);
    EXPECT_EQ(dev_added2.dev_id, 54321);
}

void test_ywfpa_DevRemoved_init() {
    ywfpa_DevRemoved dev_removed = ywfpa_DevRemoved_init_default;
    EXPECT_FALSE(dev_removed.has_cmd_header);
    EXPECT_FALSE(dev_removed.has_cmd_header);
    EXPECT_EQ(dev_removed.cmd_header.magic, 0);
    EXPECT_EQ(dev_removed.cmd_header.cmd, 0);
    EXPECT_EQ(dev_removed.cmd_header.seq, 0);
    EXPECT_EQ(dev_removed.cmd_header.seq_org, 0);
    EXPECT_EQ(dev_removed.cmd_header.rc, 0);
    EXPECT_EQ(dev_removed.dev_id, 0);
    printf("test_ywfpa_DevRemoved_init passed\n");
}

void test_ywfpa_DevRemoved_values() {
    ywfpa_DevRemoved dev_removed;
    dev_removed.has_cmd_header = true;
    dev_removed.has_cmd_header = true;
    dev_removed.cmd_header.magic = 0xFEADBEEF;
    dev_removed.cmd_header.cmd = ywfpa_CmdId_CmdDevRemoved;
    dev_removed.cmd_header.seq = 1;
    dev_removed.cmd_header.seq_org = 0;
    dev_removed.cmd_header.rc = 0;
    dev_removed.dev_id = 12345;

    // 序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_DevRemoved_fields, &dev_removed);
    EXPECT_TRUE(status);

    // 反序列化
    ywfpa_DevRemoved dev_removed2 = ywfpa_DevRemoved_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_DevRemoved_fields, &dev_removed2);
    EXPECT_TRUE(status);
    EXPECT_TRUE(dev_removed2.has_cmd_header);
    EXPECT_TRUE(dev_removed2.has_cmd_header);
    EXPECT_EQ(dev_removed2.cmd_header.magic, 0xFEADBEEF);
    EXPECT_EQ(dev_removed2.cmd_header.cmd, ywfpa_CmdId_CmdDevRemoved);
    EXPECT_EQ(dev_removed2.cmd_header.seq, 1);
    EXPECT_EQ(dev_removed2.cmd_header.seq_org, 0);
    EXPECT_EQ(dev_removed2.cmd_header.rc, 0);
    EXPECT_EQ(dev_removed2.dev_id, 12345);
    printf("test_ywfpa_DevRemoved_values passed\n");
}

void test_ywfpa_SegmentUpdated_init() {
    ywfpa_SegmentUpdated segment_updated = ywfpa_SegmentUpdated_init_default;
    EXPECT_FALSE(segment_updated.has_cmd_header);
    EXPECT_FALSE(segment_updated.has_cmd_header);
    EXPECT_EQ(segment_updated.cmd_header.magic, 0);
    EXPECT_EQ(segment_updated.cmd_header.cmd, 0);
    EXPECT_EQ(segment_updated.cmd_header.seq, 0);
    EXPECT_EQ(segment_updated.cmd_header.seq_org, 0);
    EXPECT_EQ(segment_updated.cmd_header.rc, 0);
    EXPECT_FALSE(segment_updated.has_seg_info);
    EXPECT_EQ(segment_updated.seg_info.dev_id, 0);
    EXPECT_EQ(segment_updated.seg_info.seg_id, 0);
    EXPECT_EQ(segment_updated.seg_info.begin_time, 0);
    EXPECT_EQ(segment_updated.seg_info.end_time, 0);
    EXPECT_EQ(segment_updated.seg_info.media_type_mask, 0);
    EXPECT_EQ(segment_updated.seg_info.ch_mask, 0);
    printf("test_ywfpa_SegmentUpdated_init passed\n");
}

void test_ywfpa_SegmentUpdated_values() {
    ywfpa_SegmentUpdated segment_updated;
    segment_updated.has_cmd_header = true;
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

    // 反序列化
    ywfpa_SegmentUpdated segment_updated2 = ywfpa_SegmentUpdated_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_SegmentUpdated_fields, &segment_updated2);
    EXPECT_TRUE(status);
    EXPECT_TRUE(segment_updated2.has_cmd_header);
    EXPECT_TRUE(segment_updated2.has_cmd_header);
    EXPECT_EQ(segment_updated2.cmd_header.magic, 0xFEADBEEF);
    EXPECT_EQ(segment_updated2.cmd_header.cmd, ywfpa_CmdId_CmdSegmentUpdated);
    EXPECT_EQ(segment_updated2.cmd_header.seq, 1);
    EXPECT_EQ(segment_updated2.cmd_header.seq_org, 0);
    EXPECT_EQ(segment_updated2.cmd_header.rc, 0);
    EXPECT_TRUE(segment_updated2.has_seg_info);
    EXPECT_EQ(segment_updated2.seg_info.dev_id, 12345);
    EXPECT_EQ(segment_updated2.seg_info.seg_id, 67890);
    EXPECT_EQ(segment_updated2.seg_info.begin_time, 1609459200);
    EXPECT_EQ(segment_updated2.seg_info.end_time, 1609462800);
    EXPECT_EQ(segment_updated2.seg_info.media_type_mask, 0xFF);
    EXPECT_EQ(segment_updated2.seg_info.ch_mask, 0xFF);
    printf("test_ywfpa_SegmentUpdated_values passed\n");
}

void test_ywfpa_PkgHeader_init() {
    ywfpa_PkgHeader pkg_header = ywfpa_PkgHeader_init_default;
    EXPECT_EQ(pkg_header.magic, 0);
    EXPECT_EQ(pkg_header.cmd, 0);
    EXPECT_EQ(pkg_header.seq, 0);
    EXPECT_EQ(pkg_header.seq_org, 0);
    printf("test_ywfpa_PkgHeader_init passed\n");
}

void test_ywfpa_PkgHeader_values() {
    ywfpa_PkgHeader pkg_header;
    pkg_header.magic = 0xFEADBEEF;
    pkg_header.cmd = ywfpa_CmdId_CmdStart;
    pkg_header.seq = 0x12345678;
    pkg_header.seq_org = 0x87654321;

    // 序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_PkgHeader_fields, &pkg_header);
    printf("test_ywfpa_PkgHeader_values encode %d size %d\n", status, (int)stream.bytes_written);
    //dump buffer
    dump_buffer("test_ywfpa_PkgHeader_values", buffer, stream.bytes_written);
    EXPECT_TRUE(status);

    // 反序列化
    ywfpa_PkgHeader pkg_header2 = ywfpa_PkgHeader_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_PkgHeader_fields, &pkg_header2);
    EXPECT_TRUE(status);
    EXPECT_EQ(pkg_header2.magic, 0xFEADBEEF);
    EXPECT_EQ(pkg_header2.cmd, ywfpa_CmdId_CmdStart);
    EXPECT_EQ(pkg_header2.seq, 0x12345678);
    EXPECT_EQ(pkg_header2.seq_org, 0x87654321);
    printf("\ntest_ywfpa_PkgHeader_values passed\n");
}

#endif
