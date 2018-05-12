#include "ux_bitmap.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

unsigned char *loadBmp_(const char *path, int &width, int &height);

UxBitmap::UxBitmap(int w, int h, int _type) : width(w), height(h), type(_type) {
	int bpp = 1;
	if (_type == ARGB_8888) {
		bpp = 4;
	}
	data = malloc(w * h * bpp);	
	freeData = 1;
}

UxBitmap::UxBitmap(void *_data, int w, int h, int _type, int _freeData) : data(_data),  width(w), height(h), type(_type)  {
	freeData = _freeData;
}

int UxBitmap::getWidth() const {
	return width;
}

int UxBitmap::getHeight() const {
	return height;
}
	
uint32* UxBitmap::asUInt32() const {	
	return (uint32*)data;
}


uint8* UxBitmap::asUInt8() const {
	return (uint8*)data;
}

UxBitmap::~UxBitmap() {
	if (freeData != 0) {
		free(data);
	}
}

UxBitmap* UxBitmap::wrap(void *data, int w, int h, int _type) {
	return new UxBitmap(data, w, h, _type, 0);
}

UxBitmap* UxBitmap::wrap(void *data, int w, int h, int _type, int _freeData) {
	return new UxBitmap(data, w, h, _type, _freeData);
}


UxBitmap* UxBitmap::loadBmp(const char* url) {
	int w,  h;
	void *data = loadBmp_(url, w, h);
	return new UxBitmap(data, w, h, ARGB_8888, 1);
}


void UxBitmap::drawBitmap(const UxBitmap *src, int dx, int dy) {
	if (type == src->type) {
		int bpp = type == ARGB_8888 ? 4 : 1;
		for(int y=0; y<src->getHeight(); y++) {
			memcpy(asUInt8() + (dx + (y+dy) * getWidth()) * bpp, src->asUInt8()  + (y * src->getWidth()) * bpp, src->getWidth() * bpp);
		}
		
	}
}


#pragma pack(push, 1)
typedef struct {
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data, bytes */
} HEADER;


typedef struct {
   unsigned int size;               /* Header size in bytes      */
   unsigned int width,height;                /* Width and height of image */
   unsigned short planes;       /* Number of colour planes   */
   unsigned short bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
} INFOHEADER;
#pragma pack(pop)

unsigned char *loadBmp_(const char *path, int &width, int &height) {
	FILE *bmp=fopen(path,"rb");
	HEADER header;
	INFOHEADER infoHeader;
	
	fread(&header, sizeof(header), 1, bmp);
	fread(&infoHeader, sizeof(infoHeader), 1, bmp);	
	
	width = infoHeader.width;
	height = infoHeader.height;
	
	
	unsigned char *image = new unsigned char[width * height * 4];
	
	unsigned char *scanLine = new unsigned char[width * 3];
	
	fseek(bmp, header.offset, SEEK_SET);
	unsigned char padding[4];
	
	int offset = 4 * width * (height - 1);
	for(int j=0; j<height; j++) {
		fread(scanLine, width * 3, 1, bmp);
		
		for(int i=0; i<width; i++) {
			image[offset + 4 * i + 0] = scanLine[i * 3 + 0];
			image[offset + 4 * i + 1] = scanLine[i * 3 + 1];
			image[offset + 4 * i + 2] = scanLine[i * 3 + 2];
			image[offset + 4 * i + 3] = 255;
		}
		
		if (width * 3 % 4 != 0) {
			fread(padding, 1, 4 - ((width * 3) % 4), bmp);
		}
		offset -= 4 * width;
	}
	
	fclose(bmp);
	return image;
}

void saveBmp(const char *path, unsigned char *image, int width, int height) {
	FILE *bmp=fopen(path,"wb");
	HEADER header;
	INFOHEADER infoHeader;
		
	header.type = 0x4D42;
	header.size = width * 3;
	if ( (width * 3) % 4 != 0) {
		header.size = width * 3 + 4 - ((width * 3) % 4);				
	}
	header.size *= height;	
	infoHeader.imagesize = header.size;
	
	header.size += 54;
	header.reserved1 = 0;
	header.reserved2 = 0;
	header.offset = 54;
	
	infoHeader.size = 40;
	infoHeader.width = width;
	infoHeader.height = height;
	infoHeader.planes = 1;	
	infoHeader.bits = 24;
	infoHeader.compression = 0;
	
	infoHeader.xresolution = 2835;
	infoHeader.yresolution = 2835;
	infoHeader.ncolours = 0;
	infoHeader.importantcolours = 0;
	
	
	fwrite(&header, sizeof(header), 1, bmp);
	fwrite(&infoHeader, sizeof(infoHeader), 1, bmp);		
	
	unsigned char padding[4];
	
	int offset = 3 * width * (height - 1);
	for(int j=0; j<height; j++) {
	
		for(int i=0; i<width; i++) {
			int tmp = image[i * 3];
			image[i * 3] = image[i * 3 + 2];
			image[i * 3 + 2] =  tmp;
		}
		fwrite(image + offset, width * 3, 1, bmp);
		
		if (width * 3 % 4 != 0) {
			fwrite(padding, 1, 4 - ((width * 3) % 4), bmp);
		}
		offset -= 3 * width;
	}
		
	fclose(bmp);

}