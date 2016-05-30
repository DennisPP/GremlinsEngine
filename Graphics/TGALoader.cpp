#include "TGALoader.h"


#include "GraphicsHeader.h"

#include <memory>
#include <cstdlib>

TGAHeader tgaheader;									// TGA header
TGA tga;

bool LoadTGA(Texture * texture, const char * filename)				// Load a TGA file
{
    FILE * fTGA;												// File pointer to texture file
    fTGA = fopen(filename, "rb");								// Open file for reading
    
    if(fTGA == NULL)											// If it didn't open....
        return false;														// Exit function
    
    											// TGA image data

    if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)					// Attempt to read 12 byte header from file
    {
        if(fTGA != NULL)													// Check to seeiffile is still open
        {
            fclose(fTGA);													// If it is, close it
        }
        return false;														// Exit function
    }
    static unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
    static unsigned char cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header
    if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				// See if header matches the predefined header of
    {																		// an Uncompressed TGA image
        LoadUncompressedTGA(texture, filename, fTGA);						// If so, jump to Uncompressed TGA loading code
    }
    else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
    {																		// an RLE compressed TGA image
        LoadCompressedTGA(texture, filename, fTGA);							// If so, jump to Compressed TGA loading code
    }
    else																	// If header matches neither type
    {
        fclose(fTGA);
        return false;																// Exit function
    }
    
    glGenTextures(1, &texture->texID);
    
    glBindTexture(GL_TEXTURE_2D, texture->texID);
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    if( texture->type == GL_RGB)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->imageData);
    }
    else if( texture->type == GL_RGBA)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->imageData);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if( texture->imageData )
    {
        delete [] texture->imageData;
        texture->imageData = NULL;
    }
    
    return true;															// All went well, continue on
}

bool LoadUncompressedTGA(Texture * texture, const char * filename, FILE * fTGA)	// Load an uncompressed TGA (note, much of this code is based on NeHe's
{																			// TGA Loading code nehe.gamedev.net)
    if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Read TGA header
    {
        if(fTGA != NULL)													// if file is still open
        {
            fclose(fTGA);													// Close it
        }
        return false;														// Return failular
    }
    
    texture->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
    texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
    texture->bpp	= tga.header[4];										// Determine the bits per pixel
    tga.Width		= texture->width;										// Copy width into local structure
    tga.Height		= texture->height;										// Copy height into local structure
    tga.Bpp			= texture->bpp;											// Copy BPP into local structure
    
    if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	// Make sure all information is valid
    {
        if(fTGA != NULL)													// Check if file is still open
        {
            fclose(fTGA);													// If so, close it
        }
        return false;														// Return failed
    }
    
    if(texture->bpp == 24)													// If the BPP of the image is 24...
        texture->type	= GL_RGB;											// Set Image type to GL_RGB
    else																	// Else if its 32 BPP
        texture->type	= GL_RGBA;											// Set image type to GL_RGBA
    
    tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute the number of BYTES per pixel
    tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
    texture->imageData	= new unsigned char [tga.imageSize];					// Allocate that much memory
    
    if(texture->imageData == NULL)											// If no space was allocated
    {
        fclose(fTGA);														// Close the file
        return false;														// Return failed
    }
    
    if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	// Attempt to read image data
    {
        if(texture->imageData != NULL)										// If imagedata has data in it
        {
            delete [] texture->imageData;
        }
        fclose(fTGA);														// Close file
        return false;														// Return failed
    }
    
    // Byte Swapping Optimized By Steve Thomas
    for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
    {
        texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
        texture->imageData[cswap] ^= texture->imageData[cswap+2];
    }
    
    fclose(fTGA);															// Close file
    return true;															// Return success
}

bool LoadCompressedTGA(Texture * texture, const char * filename, FILE * fTGA)		// Load COMPRESSED TGAs
{
    if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Attempt to read header
    {
        if(fTGA != NULL)													// If file is open
        {
            fclose(fTGA);													// Close it
        }
        return false;														// Return failed
    }
    
    texture->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
    texture->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
    texture->bpp	= tga.header[4];										// Determine Bits Per Pixel
    tga.Width		= texture->width;										// Copy width to local structure
    tga.Height		= texture->height;										// Copy width to local structure
    tga.Bpp			= texture->bpp;											// Copy width to local structure
    
    if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	//Make sure all texture info is ok
    {
        if(fTGA != NULL)													// Check if file is open
        {
            fclose(fTGA);													// Ifit is, close it
        }
        return false;														// Return failed
    }
    
    if(texture->bpp == 24)													// If the BPP of the image is 24...
        texture->type	= GL_RGB;											// Set Image type to GL_RGB
    else																	// Else if its 32 BPP
        texture->type	= GL_RGBA;											// Set image type to GL_RGBA
    
    tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute BYTES per pixel
    tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
    texture->imageData	= new unsigned char [tga.imageSize];					// Allocate that much memory
    
    if(texture->imageData == NULL)											// If it wasnt allocated correctly..
    {
        fclose(fTGA);														// Close file
        return false;														// Return failed
    }
    
    GLuint pixelcount	= tga.Height * tga.Width;							// Nuber of pixels in the image
    GLuint currentpixel	= 0;												// Current pixel being read
    GLuint currentbyte	= 0;												// Current byte
    GLubyte * colorbuffer = new unsigned char [tga.bytesPerPixel];
    
    do
    {
        GLubyte chunkheader = 0;											// Storage for "chunk" header
        
        if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				// Read in the 1 byte header
        {
            if(fTGA != NULL)												// If file is open
            {
                fclose(fTGA);												// Close file
            }
            if(texture->imageData != NULL)									// If there is stored image data
            {
                delete [] texture->imageData;									// Delete image data
            }
            return false;													// Return failed
        }
        
        if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
        {																	// that follow the header
            chunkheader++;													// add 1 to get number of following color values
            for(short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
            {
                if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
                {
                    
                    if(fTGA != NULL)													// See if file is open
                    {
                        fclose(fTGA);													// If so, close file
                    }
                    
                    if(colorbuffer != NULL)												// See if colorbuffer has data in it
                    {
                        delete [] colorbuffer;												// If so, delete it
                    }
                    
                    if(texture->imageData != NULL)										// See if there is stored Image data
                    {
                        delete [] texture->imageData;										// If so, delete it too
                    }
                    
                    return false;														// Return failed
                }
                // write to memory
                texture->imageData[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process
                texture->imageData[currentbyte + 1	] = colorbuffer[1];
                texture->imageData[currentbyte + 2	] = colorbuffer[0];
                
                if(tga.bytesPerPixel == 4)												// if its a 32 bpp image
                {
                    texture->imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
                }
                
                currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
                currentpixel++;															// Increase current pixel by 1
                
                if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
                {
                    
                    if(fTGA != NULL)													// If there is a file open
                    {
                        fclose(fTGA);													// Close file
                    }
                    
                    if(colorbuffer != NULL)												// If there is data in colorbuffer
                    {
                        delete [] colorbuffer;												// Delete it
                    }
                    
                    if(texture->imageData != NULL)										// If there is Image data
                    {
                        delete [] texture->imageData;										// delete it
                    }
                    
                    return false;														// Return failed
                }
            }
        }
        else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
        {
            chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
            if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
            {
                
                if(fTGA != NULL)														// If thereis a file open
                {
                    fclose(fTGA);														// Close it
                }
                
                if(colorbuffer != NULL)													// If there is data in the colorbuffer
                {
                    delete [] colorbuffer;													// delete it
                }
                
                if(texture->imageData != NULL)											// If thereis image data
                {
                    delete [] texture->imageData;											// delete it
                }
                
                return false;															// return failed
            }
            
            for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
            {																			// by the header
                texture->imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
                texture->imageData[currentbyte + 1	] = colorbuffer[1];
                texture->imageData[currentbyte + 2	] = colorbuffer[0];
                
                if(tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
                {
                    texture->imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
                }
                
                currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
                currentpixel++;															// Increase pixel count by 1
                
                if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
                {
                    
                    if(fTGA != NULL)													// If there is a file open
                    {
                        fclose(fTGA);													// Close file
                    }	
                    
                    if(colorbuffer != NULL)												// If there is data in colorbuffer
                    {
                        delete [] colorbuffer;												// Delete it
                    }
                    
                    if(texture->imageData != NULL)										// If there is Image data
                    {
                        delete [] texture->imageData;										// delete it
                    }
                    
                    return false;														// Return failed
                }
            }
        }
    } while(currentpixel < pixelcount);
    
    fclose(fTGA);
    
    
    if( colorbuffer )
    {
        delete [] colorbuffer;
        colorbuffer = NULL;
    }
    
    return true;
}
