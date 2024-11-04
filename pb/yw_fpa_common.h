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
ywfpa_CmdId fpa_get_command(const unsigned char *str, int len);

/**
 * @brief 根据命令 ID 自动反序列化对应的 protobuf 结构体。
 * 
 * 该函数根据提供的字节数组，自动解析出命令 ID 并反序列化出对应的 protobuf 结构体。
 * 
 * @param pp_data 指向包含 protobuf 数据的字节数组的指针。
 * @param vp_data_len 字节数组的长度。
 * @param pp_dest 指向要反序列化的结构体的指针。
 * @return ywfpa_CmdId 解析出的命令 ID，如果解析失败则返回 ywfpa_CmdId_CmdNA。
 */
ywfpa_CmdId fpa_decode_auto(const unsigned char *pp_data, int vp_data_len, void *pp_dest);

#ifdef __cplusplus
}
#endif

#endif /* A8BA1596_86C8_42E5_8634_4F9BC1599F81 */
