
#ifndef PB_TEST_YWFPA_YW_FPA_PB_H_INCLUDED
#define PB_TEST_YWFPA_YW_FPA_PB_H_INCLUDED
#include <pb_encode.h>
#include <pb_decode.h>
#include "yw_fpa.pb.h"
#include <assert.h>
#include <stdio.h>

void test_ywfpa_DevAdded_init() {
    ywfpa_DevAdded dev_added = ywfpa_DevAdded_init_default;
    assert(dev_added.has_cmd_header == false);
    assert(dev_added.cmd_header.has_pkg_header == false);
    assert(dev_added.cmd_header.pkg_header.magic == 0);
    assert(dev_added.cmd_header.pkg_header.cmd == 0);
    assert(dev_added.cmd_header.pkg_header.length == 0);
    assert(dev_added.cmd_header.pkg_header.length_org == 0);
    assert(dev_added.cmd_header.seq == 0);
    assert(dev_added.cmd_header.seq_org == 0);
    assert(dev_added.cmd_header.rc == 0);
    assert(dev_added.dev_id == 0);
    printf("test_ywfpa_DevAdded_init passed\n");
}

void test_ywfpa_DevAdded_values() {
    ywfpa_DevAdded dev_added;
    dev_added.has_cmd_header = true;
    dev_added.cmd_header.has_pkg_header = true;
    dev_added.cmd_header.pkg_header.magic = 0xFEADBEEF;
    dev_added.cmd_header.pkg_header.cmd = ywfpa_CmdId_CmdDevAdded;
    dev_added.cmd_header.pkg_header.length = 100;
    dev_added.cmd_header.pkg_header.length_org = 120;
    dev_added.cmd_header.seq = 1;
    dev_added.cmd_header.seq_org = 0;
    dev_added.cmd_header.rc = 0;
    dev_added.dev_id = 12345;

    //加上 序列化 和 反序列化 的测试
    //序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_DevAdded_fields, &dev_added);
    assert(status == true);
    //反序列化
    ywfpa_DevAdded dev_added2 = ywfpa_DevAdded_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_DevAdded_fields, &dev_added2);
    assert(status == true);
    assert(dev_added2.has_cmd_header == true);
    assert(dev_added2.cmd_header.has_pkg_header == true);
    assert(dev_added2.cmd_header.pkg_header.magic == 0xFEADBEEF);
    assert(dev_added2.cmd_header.pkg_header.cmd == ywfpa_CmdId_CmdDevAdded);
    assert(dev_added2.cmd_header.pkg_header.length == 100);
    assert(dev_added2.cmd_header.pkg_header.length_org == 120);
    assert(dev_added2.cmd_header.seq == 1);
    assert(dev_added2.cmd_header.seq_org == 0);
    assert(dev_added2.cmd_header.rc == 0);
    assert(dev_added2.dev_id == 12345);
    printf("test_ywfpa_DevAdded_values passed\n");
}

void test_ywfpa_DevRemoved_init() {
    ywfpa_DevRemoved dev_removed = ywfpa_DevRemoved_init_default;
    assert(dev_removed.has_cmd_header == false);
    assert(dev_removed.cmd_header.has_pkg_header == false);
    assert(dev_removed.cmd_header.pkg_header.magic == 0);
    assert(dev_removed.cmd_header.pkg_header.cmd == 0);
    assert(dev_removed.cmd_header.pkg_header.length == 0);
    assert(dev_removed.cmd_header.pkg_header.length_org == 0);
    assert(dev_removed.cmd_header.seq == 0);
    assert(dev_removed.cmd_header.seq_org == 0);
    assert(dev_removed.cmd_header.rc == 0);
    assert(dev_removed.dev_id == 0);
    printf("test_ywfpa_DevRemoved_init passed\n");
}

void test_ywfpa_DevRemoved_values() {
    ywfpa_DevRemoved dev_removed;
    dev_removed.has_cmd_header = true;
    dev_removed.cmd_header.has_pkg_header = true;
    dev_removed.cmd_header.pkg_header.magic = 0xFEADBEEF;
    dev_removed.cmd_header.pkg_header.cmd = ywfpa_CmdId_CmdDevRemoved;
    dev_removed.cmd_header.pkg_header.length = 100;
    dev_removed.cmd_header.pkg_header.length_org = 120;
    dev_removed.cmd_header.seq = 1;
    dev_removed.cmd_header.seq_org = 0;
    dev_removed.cmd_header.rc = 0;
    dev_removed.dev_id = 12345;

    //加上 序列化 和 反序列化 的测试
    //序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_DevRemoved_fields, &dev_removed);
    assert(status == true);
    //反序列化
    ywfpa_DevRemoved dev_removed2 = ywfpa_DevRemoved_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_DevRemoved_fields, &dev_removed2);
    assert(status == true);
    assert(dev_removed2.has_cmd_header == true);
    assert(dev_removed2.cmd_header.has_pkg_header == true);
    assert(dev_removed2.cmd_header.pkg_header.magic == 0xFEADBEEF);
    assert(dev_removed2.cmd_header.pkg_header.cmd == ywfpa_CmdId_CmdDevRemoved);
    assert(dev_removed2.cmd_header.pkg_header.length == 100);
    assert(dev_removed2.cmd_header.pkg_header.length_org == 120);
    assert(dev_removed2.cmd_header.seq == 1);
    assert(dev_removed2.cmd_header.seq_org == 0);
    assert(dev_removed2.cmd_header.rc == 0);
    assert(dev_removed2.dev_id == 12345);
    printf("test_ywfpa_DevRemoved_values passed\n");
}

void test_ywfpa_SegmentUpdated_init() {
    ywfpa_SegmentUpdated segment_updated = ywfpa_SegmentUpdated_init_default;
    assert(segment_updated.has_cmd_header == false);
    assert(segment_updated.cmd_header.has_pkg_header == false);
    assert(segment_updated.cmd_header.pkg_header.magic == 0);
    assert(segment_updated.cmd_header.pkg_header.cmd == 0);
    assert(segment_updated.cmd_header.pkg_header.length == 0);
    assert(segment_updated.cmd_header.pkg_header.length_org == 0);
    assert(segment_updated.cmd_header.seq == 0);
    assert(segment_updated.cmd_header.seq_org == 0);
    assert(segment_updated.cmd_header.rc == 0);
    assert(segment_updated.has_seg_info == false);
    assert(segment_updated.seg_info.dev_id == 0);
    assert(segment_updated.seg_info.seg_id == 0);
    assert(segment_updated.seg_info.begin_time == 0);
    assert(segment_updated.seg_info.end_time == 0);
    assert(segment_updated.seg_info.media_type_mask == 0);
    assert(segment_updated.seg_info.ch_mask == 0);
    printf("test_ywfpa_SegmentUpdated_init passed\n");
}

void test_ywfpa_SegmentUpdated_values() {
    ywfpa_SegmentUpdated segment_updated;
    segment_updated.has_cmd_header = true;
    segment_updated.cmd_header.has_pkg_header = true;
    segment_updated.cmd_header.pkg_header.magic = 0xFEADBEEF;
    segment_updated.cmd_header.pkg_header.cmd = ywfpa_CmdId_CmdSegmentUpdated;
    segment_updated.cmd_header.pkg_header.length = 100;
    segment_updated.cmd_header.pkg_header.length_org = 120;
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

    //加上 序列化 和 反序列化 的测试
    //序列化
    uint8_t buffer[100];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ywfpa_SegmentUpdated_fields, &segment_updated);
    assert(status == true);
    //反序列化
    ywfpa_SegmentUpdated segment_updated2 = ywfpa_SegmentUpdated_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
    status = pb_decode(&istream, ywfpa_SegmentUpdated_fields, &segment_updated2);
    assert(status == true);
    assert(segment_updated2.has_cmd_header == true);
    assert(segment_updated2.cmd_header.has_pkg_header == true);
    assert(segment_updated2.cmd_header.pkg_header.magic == 0xFEADBEEF);
    assert(segment_updated2.cmd_header.pkg_header.cmd == ywfpa_CmdId_CmdSegmentUpdated);
    assert(segment_updated2.cmd_header.pkg_header.length == 100);
    assert(segment_updated2.cmd_header.pkg_header.length_org == 120);
    assert(segment_updated2.cmd_header.seq == 1);
    assert(segment_updated2.cmd_header.seq_org == 0);
    assert(segment_updated2.cmd_header.rc == 0);
    assert(segment_updated2.has_seg_info == true);
    assert(segment_updated2.seg_info.dev_id == 12345);
    assert(segment_updated2.seg_info.seg_id == 67890);
    assert(segment_updated2.seg_info.begin_time == 1609459200);
    assert(segment_updated2.seg_info.end_time == 1609462800);
    assert(segment_updated2.seg_info.media_type_mask == 0xFF);
    assert(segment_updated2.seg_info.ch_mask == 0xFF);
    printf("test_ywfpa_SegmentUpdated_values passed\n");
}

#endif
