#include <pb_encode.h>
#include <pb_decode.h>
#include "yw_fpa_common.h"




ywfpa_CmdId fpa_get_command(const unsigned char *str, int len)
{
    ywfpa_CmdId cmd_id = ywfpa_CmdId_CmdNA;
    if(len >= ywfpa_PkgHeader_size)
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
            cmd_id = (ywfpa_CmdId)str[8];
            
            //增加对命令ID值的有效性判断
            if (cmd_id >= ywfpa_CmdId_CmdNA && cmd_id <= ywfpa_CmdId_CmdSegmentUpdated)
            {
                
            }
            else
            {
                printf("invalid cmd_id %d\n",cmd_id);
                cmd_id = ywfpa_CmdId_CmdNA;
            }
        }
        else
        {
            printf("invalid header\n");
        }

    }
    return cmd_id;
}

bool fpa_decode_by_command(const unsigned char *pp_data, int vp_data_len, ywfpa_CmdId vp_cmd_id, void *pp_dest)
{
    pb_istream_t stream = pb_istream_from_buffer(pp_data, vp_data_len);
    bool status = false;

    switch (vp_cmd_id)
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
            if (vp_cmd_id >= ywfpa_CmdId_CmdNA && vp_cmd_id <= ywfpa_CmdId_CmdSegmentUpdated)
            {
                status = pb_decode(&stream, ywfpa_CommonMsg_fields, pp_dest);
            }
            break;
    }

    return status;
}