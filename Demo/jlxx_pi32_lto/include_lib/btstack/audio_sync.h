#ifndef _AUDIO_SYNC_H_
#define _AUDIO_SYNC_H_

typedef struct _SYNC_CONTEXT_PS_
{
	u8 *outpriv;
	int (*getlen)(void* priv);                                        //获取buf的长度
	void (*output)(void *priv,u8 *buf,int len);
}SYNC_autolink_IO;


typedef struct _SYN_AUTOLINK_API_CONTEXT_
{
	u32 (*need_size)(void);
	u32 (*open)(u8 *ptr ,SYNC_autolink_IO *audiolink_IO,u32 Limit_up,u32 limit_down,u16 check_frame);
	u32 (*run)(u8 *ptr, u8 *data,u32 len);

}SYN_AUTOLINK_API_CONTEXT;

extern SYN_AUTOLINK_API_CONTEXT *  get_syn_autolink_api();

void bt_esco_sync_out_init(s16 *tmp_buffer);

u32 bt_esco_sync_out_run(u8 *in_buf, u32 len);

void bt_esco_sync_out_exit(void);

void set_esco_sr(u16 sr);

u16 get_esco_sr(void);

#endif // _BT_ESCO_SYNC_H_
