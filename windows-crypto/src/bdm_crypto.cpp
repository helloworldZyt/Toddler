#include "StdAfx.h"
//#include "StdAfx.h"
#include "bdm_crypto.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#ifdef WIN32
#include <Windows.h>
#endif
#include "aes.h"

using namespace std;

#define KEY_BUF_LEN	AES_KEYLEN

typedef struct _cfdh_ {
	char szP0[1024];
	unsigned char szP1[1024];
	int  nP0;
	int  nP1;
	int  nP2;
}BdmTParam;

typedef enum _abcd_ {
	BDM_API_ZERO=0,
	BDM_API_CRYPTO,
	BDM_API_DECRYPTO,
	BDM_API_SLICE,
	BDM_API_STITCH,
	BDM_API_MUYOULE
}; 

Dbg_Func crypto_dbg = NULL;
Progress_Func g_prog_func = NULL;

/**
 * @desc		:	(non-must) regist call back
 * @dbgfunc		:
 * @progfunc	:
 */
void bdm_set_cb(Dbg_Func dbgfunc, Progress_Func progfunc)
{
	crypto_dbg = dbgfunc;
	g_prog_func = progfunc;
	return;
}
/**
 * @desc:		:	crypto file use aes
 * @filepath	:	file path
 * @keybuf		:	crypto key
 * @return		:	0 success,	other failed!
 */
int bdm_c_file_exec(const char* filepath, const unsigned char* keybuf, int keylen)
{
	string src_file = "";
	string dst_file = "";
	int nlen = keylen;
	AES_ctx cryto_ctx;
	FILE* src_fp = NULL;
	FILE* dst_fp = NULL;

	if (KEY_BUF_LEN != nlen)
	{
		crypto_dbg(-1, "param error! %d", 0);
		return -1;
	}

	// open file
	src_file = filepath;
	dst_file = src_file + "jm";

	src_fp = fopen(src_file.c_str(), "rb");
	if (NULL == src_fp)
	{
		crypto_dbg(-2, string(src_file+" open failed!").c_str());
		return -2;
	}

	dst_fp = fopen(dst_file.c_str(), "wb+");
	if (NULL == dst_fp)
	{
		string msg = dst_file+" open failed!";
		crypto_dbg(-2, msg.c_str());
		return -2;
	}

	// aes init
	memset(&cryto_ctx, 0, sizeof(AES_ctx));
	AES_init_ctx(&cryto_ctx, keybuf);

	// crypto file
	long totallen = 0;
	long cryped_len = 0;
	long nstep = 0;
	int nstepcnt = 0;
	fseek(src_fp, 0, SEEK_END);
	totallen = ftell(src_fp);
	rewind(src_fp);
	nstep = totallen / 100;
	do 
	{
		unsigned char vbuf[KEY_BUF_LEN];
		int rlen = 0, wlen = 0;
		int i = 0;
		int cnt = 0;

		memset(vbuf, 0, KEY_BUF_LEN);
		rlen = fread(vbuf, 1, KEY_BUF_LEN, src_fp);
		if (0 > rlen)
		{
			crypto_dbg(0, "file end!");
			break;
		}
		if (KEY_BUF_LEN != rlen)
		{
			for (i = rlen%KEY_BUF_LEN,cnt=0; i < KEY_BUF_LEN; i++)
			{
				vbuf[rlen+cnt] = KEY_BUF_LEN-rlen%KEY_BUF_LEN;
				cnt++;
			}
		}
		cryped_len += rlen;
		
		AES_ECB_encrypt(&cryto_ctx, vbuf);

		wlen = fwrite(vbuf, 1, KEY_BUF_LEN, dst_fp);

		if (nstepcnt*nstep < cryped_len)
		{
			nstepcnt++;
			if (g_prog_func)
				g_prog_func(cryped_len*100.0/totallen);
		}
		
		if (KEY_BUF_LEN != rlen)
		{
			crypto_dbg(0, "rlen less than 16!");
			break;
		}
	} while (1);

	if (src_fp)
		fclose(src_fp);
	if (dst_fp)
		fclose(dst_fp);

	return 0;
}

int bdm_d_file_exec(const char* filepath, const unsigned char* keybuf, int keylen)
{
	string src_file = "";
	string dst_file = "";
	FILE* srcfp = NULL;
	FILE* dstfp = NULL;
	AES_ctx dec_ctx;

	if (NULL == filepath || NULL == keybuf)
	{
		 crypto_dbg(-1, "param error!");
		return -1;
	}

	if (KEY_BUF_LEN != keylen)
	{
		crypto_dbg(-2, "keylen error!");
		return -2;
	}

	// open file
	src_file = filepath;
	dst_file = src_file + "ori";

	srcfp = fopen(src_file.c_str(), "rb");
	if (NULL == srcfp)
	{
		crypto_dbg(-3, string(src_file + "open failed!").c_str());
		return -3;
	}

	dstfp = fopen(dst_file.c_str(), "wb+");
	if (NULL == dstfp)
	{
		crypto_dbg(-3, string("open "+dst_file+" error!").c_str());
		return -3;
	}

	// init AES
	memset(&dec_ctx, 0, sizeof(AES_ctx));
	AES_init_ctx(&dec_ctx, keybuf);

	// decrypto
	long totallen = 0;
	long declen = 0;
	long nstep = 0;
	int nstepcnt = 0;
	fseek(srcfp, 0, SEEK_END);
	totallen = ftell(srcfp);
	rewind(srcfp);
	nstep = totallen/100;

	if (0 != totallen%KEY_BUF_LEN)
	{
		crypto_dbg(totallen, "unlike encrypto files!");
		fclose(srcfp);
		fclose(dstfp);
		return -5;
	}
	do 
	{
		int rlen = 0;
		unsigned char vbuf[KEY_BUF_LEN];
		memset(vbuf, 0, KEY_BUF_LEN);

		rlen = fread(vbuf, 1, KEY_BUF_LEN, srcfp);
		if (0 >= rlen)
		{
			crypto_dbg(rlen, "get the file end!");
			break;
		}

		AES_ECB_decrypt(&dec_ctx, vbuf);
		declen += rlen;

		if (declen == totallen)
		{
			int ev = vbuf[KEY_BUF_LEN-1];
			rlen -= ev;
		}
		fwrite(vbuf, 1, rlen, dstfp);

		if (nstep * nstepcnt < declen)
		{
			nstepcnt += 1;
			if (g_prog_func)
				g_prog_func(declen*100/totallen);
		}
		if (declen == totallen)
		{
			crypto_dbg(declen, "get the file end! %d", totallen);
			break;
		}

	} while (1);
	if (srcfp)
		fclose(srcfp);
	if (dstfp)
		fclose(dstfp);
	return 0;
}

int bdm_slice_file_exec(const char* srcfilepath, int snum)
{
	string str_srcf = "";
	string str_dstf = "";
	string digitidx = "0123456789abcdef";
	int i = 0;
	if (NULL == srcfilepath || 0 >= snum)
	{
		crypto_dbg(-1, "param error!");
		return -1;
	}
	if (digitidx.length() < snum)
	{
		crypto_dbg(-1, "slice num big than 16!");
		return -1;
	}
	FILE* pfs = fopen(srcfilepath, "rb");
	if (NULL == pfs)
	{
		crypto_dbg(-2, "fopen");
		return -2;
	}
	
	uint64_t totallen = 0;
	uint64_t steplen = 0;
	uint64_t slicelen = 0;
	uint64_t r_max_len = 0;
	uint64_t cryped_len = 0;
	int	last_tick = 0;
	long r_len = 0;
	unsigned char * psbuf = NULL;
// 	{
// 		fseek(pfs, 0, SEEK_END);
// 		totallen = ftell(pfs);
// 		rewind(pfs);
// 	}
	{
		struct stat statbuf = {0};
		stat(srcfilepath, &statbuf);
		totallen = statbuf.st_size;
	}

	if (0 == totallen)
	{
#ifdef WIN32
		HANDLE hFile=CreateFile(
			srcfilepath, //文件名
			GENERIC_READ, //对文件进行读操作
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING, //打开已存在文件
			FILE_ATTRIBUTE_NORMAL,
			0);
		DWORD sizeLow,sizeHigh;
		sizeLow = ::GetFileSize(hFile,&sizeHigh);
		totallen = sizeHigh;
		totallen = totallen << 32;
		totallen += sizeLow;
#else
#endif
	}
	
	if (0 >= totallen)
	{
		crypto_dbg(-3, "file len totallen %d", totallen);
		return -3;
	}
	slicelen = totallen/snum + totallen % snum;
	r_max_len = (slicelen > 104857600)?104857600:slicelen;// 1024*1024*100
	psbuf = (unsigned char*)malloc(r_max_len);
	if (NULL == psbuf)
	{
		crypto_dbg(-3, "malloc");
		return -3;
	}
	str_srcf = srcfilepath;
	for(int i = 0; i < snum; i++)
	{
		str_dstf = str_srcf + digitidx.at(i);
		FILE* pfdst = fopen(str_dstf.c_str(), "wb+");
		if (NULL == pfdst)
		{
			crypto_dbg(-3, "fopen dst file %d", i);
			break;
		}
		for (steplen = 0; steplen < slicelen;)
		{
			r_len = fread(psbuf, 1, r_max_len, pfs);
			if (0 >= r_len)
			{
				crypto_dbg(-3, "fread error!");
				break;
			}
			fwrite(psbuf, 1, r_len, pfdst);
			steplen += r_len;

#ifdef WIN32
			int cur_tick = ::GetTickCount();
			cryped_len += r_len;
			if (0 == last_tick || (2000 < (cur_tick - last_tick)))
			{
				if (g_prog_func)
					g_prog_func(cryped_len*100.0/totallen);
				last_tick = cur_tick;
			}
			if (cryped_len == totallen)
			{
				crypto_dbg(0, "finished!");
				break;
			}
#endif
		}
		fclose(pfdst);
	}
	free(psbuf);
	fclose(pfs);
	return 0;
}

int bdm_stitch_file_exec(const char* dstfilepath, int snum)
{
	uint64_t rlen = 0;
	uint64_t cryped_len = 0;
	uint64_t steplen = 0;
	int last_tick = 0;
	string str_srcf = "";
	string str_dstf = "";
	string digitidx = "0123456789abcdef";
	unsigned char* psbuf;
	FILE* pfdst = NULL;
	
	if (NULL == dstfilepath)
	{
		crypto_dbg(0,"param error filepath null\n");
		return -1;
	}
	pfdst = fopen(dstfilepath, "wb+");
	if (NULL == pfdst)
	{
		crypto_dbg(0, "create dst file failed!");
		return -2;
	}
	str_dstf = dstfilepath;
	str_srcf = str_dstf + digitidx[0];
	{
		struct stat statbuf = {0};
		stat(str_srcf.c_str(), &statbuf);
		steplen = statbuf.st_size;
	}
	if (0 == steplen)
	{
		steplen = 1024*1024*2;// 2M
	}
	psbuf = (unsigned char*)malloc(steplen);
	if (NULL == psbuf)
	{
		crypto_dbg(0, "malloc return null");
		return 0;
	}

	for (int i = 0; i < 99; i++)
	{
		str_srcf = str_dstf + digitidx[i];
		FILE* pfsrc = fopen(str_srcf.c_str(), "rb");
		crypto_dbg(0, "[%d]fopen %s ptr %x", i, str_srcf.c_str(), pfsrc);
		if (NULL == pfsrc)
		{
			break;
		}
		while (0 != (rlen = fread(psbuf, 1, steplen, pfsrc)))
		{
			fwrite(psbuf, 1, rlen, pfdst);
#ifdef WIN32
			int cur_tick = ::GetTickCount();
			cryped_len += rlen;
			if (0 == last_tick || (2000 < (cur_tick - last_tick)))
			{
				crypto_dbg(0, "process cryped_len %lu ...", cryped_len);
				last_tick = cur_tick;
			}
#endif
		}
		fclose(pfsrc);
	}
	fclose(pfdst);
	free(psbuf);
	return 0;
}

#ifdef WIN32
DWORD WINAPI bdm_exec_func(void* arg)
{
	BdmTParam* parg = ((BdmTParam*)arg);
	if (NULL == parg)
	{
		return 0;
	}

	switch(parg->nP0)
	{
	case BDM_API_CRYPTO:
		{
			bdm_c_file_exec(parg->szP0, parg->szP1, parg->nP1);
		}
		break;
	case BDM_API_DECRYPTO:
		{
			bdm_d_file_exec(parg->szP0, parg->szP1, parg->nP1);
		}
		break;
	case BDM_API_SLICE:
		{
			bdm_slice_file_exec(parg->szP0, parg->nP1);
		}
		break;
	case BDM_API_STITCH:
		{
			bdm_stitch_file_exec(parg->szP0, parg->nP1);
		}
		break;
	default:
		break;
	}

	free(parg);
	return 0;
}
int  async_threadadd(int napi, const char* filepath, const unsigned char*keybuf, int snum)
{
	HANDLE pthnd = NULL;
	BdmTParam* parg = (BdmTParam*)malloc(sizeof(BdmTParam));
	memset(parg, 0, sizeof(BdmTParam));
	parg->nP0 = napi;
	parg->nP1 = snum;
	strcpy(parg->szP0, filepath);
	if (keybuf)
	{
		memcpy(parg->szP1, keybuf, snum);
	}

	pthnd = ::CreateThread(NULL, 0, bdm_exec_func, (void*)parg, 0, 0);
	//::WaitForSingleObject(pthnd, INFINITE);
	return 0;
}

int bdm_stitch_file(const char* filepath, int snum)
{
	return async_threadadd(BDM_API_STITCH, filepath, NULL, snum);
}

int bdm_slice_file(const char* srcfilepath, int snum)
{
	return async_threadadd(BDM_API_SLICE, srcfilepath, NULL,snum);
}

int bdm_c_file(const char* filepath, const unsigned char* keybuf, int keylen)
{
	return async_threadadd(BDM_API_CRYPTO, filepath, keybuf, keylen);
}

int bdm_d_file(const char* filepath, const unsigned char* keybuf, int keylen)
{
	return async_threadadd(BDM_API_DECRYPTO, filepath, keybuf, keylen);
}
#else
#endif