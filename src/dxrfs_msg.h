#ifndef C60A08FC_F16F_425A_AA15_DF5295E54329
#define C60A08FC_F16F_425A_AA15_DF5295E54329

#define DFS_HEAD_DUMMY  4
struct t_dxrfs_msg_hdr
{
    uint32_t v_Ver;   //版本: 从 0x0001 开始
    uint32_t v_Magic; //校验用：magic 0xFEEDBEAF
    uint32_t v_Cmd;
    uint32_t v_Len;
    uint8_t a_Data[DFS_HEAD_DUMMY];
};
enum e_cmd
{
    DFC_ECHO=0L,//心跳
    DFC_INIT, //
    
};
// 大碎 
#define DFS_SIZE_OF_DATA(LEN)  ((sizeof(t_dxrfs_msg_hdr) - DFS_HEAD_DUMMY) + (LEN))


#endif /* C60A08FC_F16F_425A_AA15_DF5295E54329 */
