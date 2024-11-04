#include <pb_encode.h>
#include <pb_decode.h>
#include "yw_fpa_common.h"

/**
 * @brief 解析一个通用的 FPA 命令。
 * 
 * 该函数检查提供的字节数组是否包含有效的 FPA 命令头，
 * 如果头有效，则提取命令 ID。
 * 
 * @param str 指向包含命令数据的字节数组的指针。
 * @param len 字节数组的长度。
 * @return ywfpa_CmdId 提取的命令 ID，如果头无效则返回 ywfpa_CmdId_CmdNA。
 */
ywfpa_CmdId fpa_get_command(const unsigned char *str, int len)
{
    ywfpa_CmdId cmd_id = ywfpa_CmdId_CmdNA;
    if(len > 12)
    {
        //0A 16 0D EF BE AD FE 15 06 00 00 00
        //0A 16 0D EF BE AD FE 15 04 00 00 00
        //0A 0C 0D EF BE AD FE 15 06 00 00 00
        //如果前面的数据是 0A XX 0D EF BE AD FE 15 
        //则说明是数据包头
        if (str[0] == 0x0A && str[2] == 0x0D && str[3] == 0xEF &&
            str[4] == 0xBE && str[5] == 0xAD && str[6] == 0xFE && str[7] == 0x15)
        {
            //从第8个字节取4个字节则是命令ID
            uint32_t cmd_id_raw = 0;
            memcpy(&cmd_id_raw, str+8, sizeof(cmd_id_raw));
            cmd_id = (ywfpa_CmdId)cmd_id_raw;
            
            //增加对命令ID值的有效性判断
            if (cmd_id >= ywfpa_CmdId_CmdNA && cmd_id <= ywfpa_CmdId_CmdSegmentUpdated)
            {
                
            }
            else
            {
                cmd_id = ywfpa_CmdId_CmdNA;
            }
        }
    }
    return cmd_id;
}

ywfpa_CmdId fpa_decode_auto(const unsigned char *pp_data, int vp_data_len, void *pp_dest)
{
    ywfpa_CmdId cmd_id = fpa_get_command(pp_data, vp_data_len);
    pb_istream_t stream = pb_istream_from_buffer(pp_data, vp_data_len);
    bool status = false;

    switch (cmd_id)
    {
        case ywfpa_CmdId_CmdDevAdded:
            status = pb_decode(&stream, ywfpa_DevAdded_fields, pp_dest);
            break;
        case ywfpa_CmdId_CmdDevRemoved:
            status = pb_decode(&stream, ywfpa_DevRemoved_fields, pp_dest);
            break;
        case ywfpa_CmdId_CmdSegmentUpdated:
            status = pb_decode(&stream, ywfpa_SegmentUpdated_fields, pp_dest);
            break;
        // Add cases for other command IDs and their corresponding protobuf structures
        default:
            if (cmd_id >= ywfpa_CmdId_CmdNA && cmd_id <= ywfpa_CmdId_CmdSegmentUpdated)
            {
                status = pb_decode(&stream, ywfpa_CommonMsg_fields, pp_dest);
            }
            break;
    }

    return status ? cmd_id : ywfpa_CmdId_CmdNA;
}

