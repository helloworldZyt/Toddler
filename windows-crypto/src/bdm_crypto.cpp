#include "StdAfx.h"
//#include "StdAfx.h"
#include "bdm_crypto.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include "aes.h"

using namespace std;

#define KEY_BUF_LEN	AES_KEYLEN

Dbg_Func g_dbgout = NULL;
Progress_Func g_prog_func = NULL;

void crypto_dbg(int reason, const char* desc, int desclen)
{
	if (g_dbgout)
		g_dbgout(reason, desc, desclen);
	return;
}
/**
 * @desc		:	(non-must) regist call back
 * @dbgfunc		:
 * @progfunc	:
 */
void bdm_set_cb(Dbg_Func dbgfunc, Progress_Func progfunc)
{
	g_dbgout = dbgfunc;
	g_prog_func = progfunc;
	return;
}
/**
 * @desc:		:	crypto file use aes
 * @filepath	:	file path
 * @keybuf		:	crypto key
 * @return		:	0 success,	other failed!
 */
int bdm_c_file(const char* filepath, const unsigned char* keybuf, int keylen)
{
	string src_file = "";
	string dst_file = "";
	int nlen = keylen;
	AES_ctx cryto_ctx;
	FILE* src_fp = NULL;
	FILE* dst_fp = NULL;

	if (KEY_BUF_LEN != nlen)
	{
		crypto_dbg(-1, "param error!", 0);
		return -1;
	}

	// open file
	src_file = filepath;
	dst_file = src_file + "jm";

	src_fp = fopen(src_file.c_str(), "rb");
	if (NULL == src_fp)
	{
		crypto_dbg(-2, string(src_file+" open failed!").c_str(), 0);
		return -2;
	}

	dst_fp = fopen(dst_file.c_str(), "wb+");
	if (NULL == dst_fp)
	{
		string msg = dst_file+" open failed!";
		crypto_dbg(-2, msg.c_str(), 0);
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
			crypto_dbg(0, "file end!", 0);
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
			crypto_dbg(0, "rlen less than 16!", 0);
			break;
		}
	} while (1);

	if (src_fp)
		fclose(src_fp);
	if (dst_fp)
		fclose(dst_fp);

	return 0;
}

int bdm_d_file(const char* filepath, const unsigned char* keybuf, int keylen)
{
	string src_file = "";
	string dst_file = "";
	FILE* srcfp = NULL;
	FILE* dstfp = NULL;
	AES_ctx dec_ctx;

	if (NULL == filepath || NULL == keybuf)
	{
		 crypto_dbg(-1, "param error!",0);
		return -1;
	}

	if (KEY_BUF_LEN != keylen)
	{
		crypto_dbg(-2, "keylen error!", 0);
		return -2;
	}

	// open file
	src_file = filepath;
	dst_file = src_file + "ori";

	srcfp = fopen(src_file.c_str(), "rb");
	if (NULL == srcfp)
	{
		crypto_dbg(-3, string(src_file + "open failed!").c_str(), 0);
		return -3;
	}

	dstfp = fopen(dst_file.c_str(), "wb+");
	if (NULL == dstfp)
	{
		crypto_dbg(-3, string("open "+dst_file+" error!").c_str(), 0);
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
		crypto_dbg(totallen, "unlike encrypto files!", 0);
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
			crypto_dbg(rlen, "get the file end!", 0);
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
			crypto_dbg(declen, "get the file end!", totallen);
			break;
		}

	} while (1);
	if (srcfp)
		fclose(srcfp);
	if (dstfp)
		fclose(dstfp);
	return 0;
}

int bdm_slice_file(const char* filepath, int snum)
{
	string str_srcf = "";
	string str_dstf = "";
	string digitidx = "0123456789abcdef";
	int i = 0;
	if (NULL == filepath || 0 >= snum)
	{
		crypto_dbg(-1, "param error!", 0);
		return -1;
	}
	if (digitidx.length() < snum)
	{
		crypto_dbg(-1, "slice num big than 16!", 0);
		return -1;
	}
	FILE* pfs = fopen(filepath, "rb");
	if (NULL == pfs)
	{
		crypto_dbg(-2, "fopen", 0);
		return -2;
	}
	
	long totallen = 0;
	long steplen = 0;
	long slicelen = 0;
	long r_max_len = 0;
	long r_len = 0;
	unsigned char * psbuf = NULL;
	fseek(pfs, 0, SEEK_END);
	totallen = ftell(pfs);
	rewind(pfs);
	if (0 >= totallen)
	{
		crypto_dbg(-3, "file len", 0);
		return -3;
	}
	slicelen = totallen/snum + totallen % snum;
	r_max_len = (slicelen > 104857600)?104857600:slicelen;// 1024*1024*100
	psbuf = (unsigned char*)malloc(r_max_len);
	if (NULL == psbuf)
	{
		crypto_dbg(-3, "malloc", 0);
		return -3;
	}
	str_srcf = filepath;
	for(int i = 0; i < snum; i++)
	{
		str_dstf = str_srcf + digitidx.at(i);
		FILE* pfdst = fopen(str_dstf.c_str(), "wb+");
		if (NULL == pfdst)
		{
			crypto_dbg(-3, "fopen dst file", i);
			break;
		}
		for (steplen = 0; steplen < slicelen;)
		{
			r_len = fread(psbuf, 1, r_max_len, pfs);
			if (0 >= r_len)
			{
				crypto_dbg(-3, "fread error!", 0);
				break;
			}
			fwrite(psbuf, 1, r_len, pfdst);
			steplen += r_len;
		}
		fclose(pfdst);
	}
	free(psbuf);
	fclose(pfs);
	return 0;
}