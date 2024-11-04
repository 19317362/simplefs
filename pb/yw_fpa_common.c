#include <pb_encode.h>
#include <pb_decode.h>
#include "yw_fpa_common.h"


/**
 * @brief 
 * 
 * @param str 
 * @param len 
 * @return int 
 */

ywfpa_CmdId fpa_parse_common(const unsigned char *str, int len)
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

        }

    }
    return cmd_id;
}