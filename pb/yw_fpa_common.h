#ifndef A8BA1596_86C8_42E5_8634_4F9BC1599F81
#define A8BA1596_86C8_42E5_8634_4F9BC1599F81

#include "yw_fpa.pb.h"
/**
 * @brief 通用的解析函数
 * 
 */

//要以C方式输出
#ifdef __cplusplus
extern "C" {
#endif
#define YWFPA_CMD_HEADER_MAGIC 0xFEADBEEF

ywfpa_CmdId fpa_parse_common(const unsigned char *str, int len);

#ifdef __cplusplus
}
#endif

#endif /* A8BA1596_86C8_42E5_8634_4F9BC1599F81 */
