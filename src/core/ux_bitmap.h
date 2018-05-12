#ifndef __UX_BITMAP__H__
#define __UX_BITMAP__H__

#define ARGB_8888 0
#define GRAYSCALE 1
#define INDEXED 2

#define uint32 int
#define uint16 unsigned short
#define uint8 unsigned char

class UxBitmap {
public:
	UxBitmap(int w, int h, int _type);
	UxBitmap(void *data, int w, int h, int _type, int freeData);
	
	static UxBitmap* wrap(void *data, int w, int h, int _type);
	static UxBitmap* wrap(void *data, int w, int h, int _type, int freeData);
	static UxBitmap* loadBmp(const char* url);

	int getWidth() const;
	int getHeight() const;
	
	uint32* asUInt32() const;	
	uint8* asUInt8() const;
	
	void drawBitmap(const UxBitmap *src, int x, int y);
	
	~UxBitmap();

private:
	int width, height;
	int type;
	
	void *data;
	
	int freeData;
};

#endif