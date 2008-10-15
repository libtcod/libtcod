/*
* libtcod 1.4.0
* Copyright (c) 2008 J.C.Wilk
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of J.C.Wilk may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY J.C.WILK ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL J.C.WILK BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "libtcod.h"
#include "libtcod_int.h"

typedef struct {
	TCOD_list_t buffer; // list<int>
	uint32 ibuffer; // byte buffer. bytes are send into buffer 4 by 4
	int isize; // number of bytes in ibuffer
	int bsize; // number of bytes in buffer
	int offset; // current reading position
} zip_data_t;

TCOD_zip_t TCOD_zip_new() {
	zip_data_t *ret=(zip_data_t *)calloc(sizeof(zip_data_t),1);
	return (TCOD_zip_t)ret;
}

void TCOD_zip_delete(TCOD_zip_t pzip) {
	zip_data_t *zip=(zip_data_t *)pzip;
	TCOD_list_delete(zip->buffer);
	free(zip);
}


// output interface
void TCOD_zip_put_char(TCOD_zip_t pzip, char val) {
	zip_data_t *zip=(zip_data_t *)pzip;
	uint32 iv=(uint32)(uint8)val;
	// store one byte in ibuffer
	switch (zip->isize) {
		case 0 : zip->ibuffer|=iv; break;
		case 1 : zip->ibuffer|=(iv<<8); break;
		case 2 : zip->ibuffer|=(iv<<16); break;
		case 3 : zip->ibuffer|=(iv<<24); break;
	}
	zip->isize++;
	zip->bsize++;
	if (zip->isize == 4 ) {
		// ibuffer full. send it to buffer
		if (!zip->buffer) zip->buffer=TCOD_list_new();
		TCOD_list_push(zip->buffer,(void *)zip->ibuffer);
		zip->isize=zip->ibuffer=0;
	}
}

void TCOD_zip_put_int(TCOD_zip_t pzip, int val) {
	zip_data_t *zip=(zip_data_t *)pzip;
	if ( zip->isize == 0 ) {
		// the buffer is padded. read 4 bytes
		if (!zip->buffer) zip->buffer=TCOD_list_new();
		TCOD_list_push(zip->buffer,(void *)val);
		zip->bsize += sizeof(int);
	} else {
		// the buffer is not padded. read 4x1 byte
		TCOD_zip_put_char(pzip,(char)(val&0xFF));
		TCOD_zip_put_char(pzip,(char)((val&0xFF00)>>8));
		TCOD_zip_put_char(pzip,(char)((val&0xFF0000)>>16));
		TCOD_zip_put_char(pzip,(char)((val&0xFF000000)>>24));
	}
}

void TCOD_zip_put_float(TCOD_zip_t pzip, float val) {
	TCOD_zip_put_int(pzip,* (int *)(&val) );
}

void TCOD_zip_put_string(TCOD_zip_t pzip, const char *val) {
	if (val == NULL) TCOD_zip_put_int(pzip,-1);
	else {
		int l=strlen(val),i;
		TCOD_zip_put_int(pzip,l);
		for (i=0; i <= l; i++) TCOD_zip_put_char(pzip,val[i]);
	}
}

void TCOD_zip_put_data(TCOD_zip_t pzip, int nbBytes, const void *data) {
	if (data == NULL) TCOD_zip_put_int(pzip,-1);
	else {
		char *val=(char *)data;
		int i;
		TCOD_zip_put_int(pzip,nbBytes);
		for (i=0; i< nbBytes; i++) TCOD_zip_put_char(pzip,val[i]);
	}
}

int TCOD_zip_save_to_file(TCOD_zip_t pzip, const char *filename) {
	zip_data_t *zip=(zip_data_t *)pzip;
	gzFile f=gzopen(filename,"wb");
	int l=zip->bsize;
	void *buf;
	if (!f) return 0;
	gzwrite(f,&l,sizeof(int));
	if (l==0) {
		gzclose(f);
		return 0;
	}
	if ( zip->isize > 0 ) {
		// send remaining bytes from ibuffer to buffer
		if (!zip->buffer) zip->buffer=TCOD_list_new();
		TCOD_list_push(zip->buffer,(void *)zip->ibuffer);
		zip->isize=zip->ibuffer=0;
	}
	buf=(void *)TCOD_list_begin(zip->buffer);
	l=gzwrite(f,buf,l);
	gzclose(f);
	return l;
}


// input interface
int TCOD_zip_load_from_file(TCOD_zip_t pzip, const char *filename) {
	zip_data_t *zip=(zip_data_t *)pzip;
	gzFile f=gzopen(filename,"rb");
	int l,lread;
	void *buf;
	if (!f) return 0;
	gzread(f,&l,sizeof(int));
	if (l==0) {
		gzclose(f);
		return 0;
	}
	if ( zip->buffer) {
		TCOD_list_delete(zip->buffer);
		memset(zip,0,sizeof(zip_data_t));
	}
	zip->buffer=TCOD_list_allocate((l+3)/4);
	TCOD_list_set_size(zip->buffer,(l+3)/4);
	buf=(void *)TCOD_list_begin(zip->buffer);
	lread=gzread(f,buf,l);
	gzclose(f);
	return lread == 0 ? l : lread;
}

char TCOD_zip_get_char(TCOD_zip_t pzip) {
	zip_data_t *zip=(zip_data_t *)pzip;
	char c=0;
	if ( zip->isize == 0 ) {
		// ibuffer is empty. get 4 new bytes from buffer
		zip->ibuffer=(int)TCOD_list_get(zip->buffer,zip->offset);
		zip->offset++;
		zip->isize=4;
	}
	// read one byte from ibuffer
	switch(zip->isize) {
		case 4: c= zip->ibuffer&0xFF; break;
		case 3: c= (zip->ibuffer&0xFF00)>>8; break;
		case 2: c= (zip->ibuffer&0xFF0000)>>16; break;
		case 1: c= (zip->ibuffer&0xFF000000)>>24; break;
	}
	zip->isize--;
	return c;
}

int TCOD_zip_get_int(TCOD_zip_t pzip) {
	zip_data_t *zip=(zip_data_t *)pzip;
	if ( zip->isize == 0 ) {
		// buffer is padded. read 4 bytes
		int i=(int)TCOD_list_get(zip->buffer,zip->offset);
		zip->offset++;
		return i;
	} else {
		// buffer is not padded. read 4x 1 byte
		uint32 i1=(uint32)(uint8)TCOD_zip_get_char(pzip);
		uint32 i2=(uint32)(uint8)TCOD_zip_get_char(pzip);
		uint32 i3=(uint32)(uint8)TCOD_zip_get_char(pzip);
		uint32 i4=(uint32)(uint8)TCOD_zip_get_char(pzip);

		return i1 | (i2<<8) | (i3<<16) | (i4<<24);
	}
}

float TCOD_zip_get_float(TCOD_zip_t pzip) {
	int i=TCOD_zip_get_int(pzip);
	return *(float *)(&i);
}

const char *TCOD_zip_get_string(TCOD_zip_t pzip) {
	zip_data_t *zip=(zip_data_t *)pzip;
	int l=TCOD_zip_get_int(pzip);
	const char *ret=(const char *)TCOD_list_begin(zip->buffer);
	int boffset; // offset in bytes
	if ( l == -1 ) return NULL;
	boffset=zip->offset*sizeof(int)-zip->isize; // current offset
	ret += boffset; // the string address in buffer
	boffset += l+1; // new offset
	// update ibuffer
	zip->offset = (boffset+sizeof(int)-1)/sizeof(int);
	zip->isize = boffset%sizeof(int);
	if ( zip->isize != 0 ) {
		zip->isize=4-zip->isize;
		zip->ibuffer=(int)TCOD_list_get(zip->buffer,zip->offset-1);
	}
	return ret;
}

int TCOD_zip_get_data(TCOD_zip_t pzip, int nbBytes, void *data) {
	zip_data_t *zip=(zip_data_t *)pzip;
	int l=TCOD_zip_get_int(pzip),i;
	const char *in=(const char *)TCOD_list_begin(zip->buffer);
	char *out=(char *)data;
	int boffset; // offset in bytes
	if ( l == -1 ) return 0;
	boffset=zip->offset*sizeof(int)-zip->isize; // current offset
	in += boffset; // the data address in buffer
	// copy it to data
	for (i=0; i < MIN(l,nbBytes); i++ ) {
		*(out++)=*(in++);
		boffset++;
	}
	// update ibuffer
	zip->offset = (boffset+sizeof(int)-1)/sizeof(int);
	zip->isize = boffset%sizeof(int);
	if ( zip->isize != 0 ) {
		zip->isize=4-zip->isize;
		zip->ibuffer=(int)TCOD_list_get(zip->buffer,zip->offset-1);
	}
	return l;
}



