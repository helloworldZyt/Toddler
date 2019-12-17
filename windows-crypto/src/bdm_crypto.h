#ifndef __BDM_CRYPTO_H_
#define __BDM_CRYPTO_H_

#define BDM_CRYPTO_API	

typedef void (*Dbg_Func)(int zeron, const char* f, ...);
typedef void (*Progress_Func)(double val);

#define BDM_FILE_MAXSZ		(0x500000000)		//20G


BDM_CRYPTO_API void bdm_set_cb(Dbg_Func dbgfunc, Progress_Func progfunc);

BDM_CRYPTO_API int bdm_c_file(const char* filepath, const unsigned char* keybuf, int keylen);

BDM_CRYPTO_API int bdm_d_file(const char* filepath, const unsigned char* keybuf, int keylen);

// 拆分大文件成 snum个小文件
BDM_CRYPTO_API int bdm_slice_file(const char* srcfilepath, int snum);

BDM_CRYPTO_API int bdm_stitch_file(const char* dstfilepath, int snum);

#endif // __BDM_CRYPTO_H_