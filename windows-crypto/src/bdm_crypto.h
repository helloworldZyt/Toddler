#ifndef __BDM_CRYPTO_H_
#define __BDM_CRYPTO_H_

#define BDM_CRYPTO_API	

typedef void (*Dbg_Func)(int reason, const char* desc, int desclen);
typedef void (*Progress_Func)(double val);


BDM_CRYPTO_API void bdm_set_cb(Dbg_Func dbgfunc, Progress_Func progfunc);

BDM_CRYPTO_API int bdm_c_file(const char* filepath, const unsigned char* keybuf, int keylen);

BDM_CRYPTO_API int bdm_d_file(const char* filepath, const unsigned char* keybuf, int keylen);

#endif // __BDM_CRYPTO_H_