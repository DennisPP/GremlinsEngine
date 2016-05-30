#ifndef __TGALoader__H__
#define __TGALoader__H__

#include <cstdio>

typedef	struct
{
    unsigned char	* imageData;									// Image Data (Up To 32 Bits)
    unsigned int	bpp;											// Image Color Depth In Bits Per Pixel
    unsigned int	width;											// Image Width
    unsigned int	height;											// Image Height
    unsigned int	texID;											// Texture ID Used To Select A Texture
    unsigned int	type;											// Image Type (GL_RGB, GL_RGBA)
} Texture;	

typedef struct
{
    unsigned char Header[12];									// TGA File Header
} TGAHeader;


typedef struct
{
    unsigned char		header[6];			// First 6 Useful Bytes From The Header
    unsigned int		bytesPerPixel;		// Holds Number Of Bytes Per Pixel Used In The TGA File
    unsigned int		imageSize;			// Used To Store The Image Size When Setting Aside Ram
    unsigned int		temp;				// Temporary Variable
    unsigned int		type;
    unsigned int		Height;				//Height of Image
    unsigned int		Width;				//Width ofImage
    unsigned int		Bpp;				// Bits Per Pixel
} TGA;




bool LoadTGA(Texture * texture, const char * filename);
bool LoadUncompressedTGA(Texture *, const char *, FILE *);	// Load an Uncompressed file
bool LoadCompressedTGA(Texture *, const char *, FILE *);		// Load a Compressed file

#endif