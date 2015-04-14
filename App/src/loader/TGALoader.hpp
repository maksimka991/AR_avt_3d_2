#pragma once

#include <windows.h>									// Standard windows header
#include <stdio.h>										// Standard I/O header 
#include <gl\gl.h>	
#include <cstdio>
#include <iostream> 


namespace NApp
{
		struct Texture
		{
			GLubyte	* imageData;									// Image Data (Up To 32 Bits)
			GLuint	bpp;											// Image Color Depth In Bits Per Pixel
			GLuint	width;											// Image Width
			GLuint	height;											// Image Height
			GLuint	texID;											// Texture ID Used To Select A Texture
			GLuint	type;											// Image Type (GL_RGB, GL_RGBA)
		};



		  struct TGAHeader
		{
			GLubyte Header[12];									// TGA File Header
		} ;


		 struct TGA
		{
			GLubyte		header[6];								// First 6 Useful Bytes From The Header
			GLuint		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
			GLuint		imageSize;								// Used To Store The Image Size When Setting Aside Ram
			GLuint		temp;									// Temporary Variable
			GLuint		type;
			GLuint		Height;									//Height of Image
			GLuint		Width;									//Width ofImage
			GLuint		Bpp;									// Bits Per Pixel
		} ;

		class TGALoader
		{
		public:
			/** Constructor. */
			TGALoader();

      bool LoadTGA(Texture * texture, const char * filename);
		
 	
			
		private:
          bool LoadUncompressedTGA(Texture *, const char *, FILE *);	// Load an Uncompressed file
         bool LoadCompressedTGA(Texture *, const char *, FILE *);		// Load a Compressed file
		
		 
		public:

		private:
			  TGAHeader tgaheader;									// TGA header
			  TGA tga;
			  Texture texture;

			// GLubyte cTGAcompare[];
			// GLubyte uTGAcompare[];

		};

}



