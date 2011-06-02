// -------------------------------------------------------------------------
//
// qrenc.c - QRcode encoder
//
// QR Code encoding tool
// Copyright (C) 2006, 2007, 2008, 2009, 2010 Kentaro Fukuchi <fukuchi@megaui.net>
//
// NOTE: Another way to create a PNG file = UIImagePNGRepresentation(UIImage* image)
// -------------------------------------------------------------------------

#include "qrenc.h"

static int	exitval			= noErr;
static int	casesensitive	= 1;
static int	eightbit		= 0;
static int	version			= 0;
static int	imagesizer		= kIMAGESIZER_3;	// original value was 3
static int	margin			= 4;

static QRecLevel level		= QR_ECLEVEL_L;
static QRencodeMode hint	= QR_MODE_8;

// -------------------------------------------------------------------------

static int writePNG(QRcode* qrcode, const char* outfile, int imagesize)
	{
	static FILE*	fp = 0;		// avoid clobbering by setjmp.
	
	png_structp		png_ptr;
	png_infop		info_ptr;
	unsigned char*	row			= NULL;
	unsigned char*	p			= NULL;
	unsigned char*	q			= NULL;
	int				x			= 0;
	int				y			= 0;
	int				xx			= 0;
	int				yy			= 0;
	int				bit			= 0;
	int				realwidth	= 0;
	
	imagesizer	= imagesize;
	realwidth	= ((qrcode->width + (margin * 2)) * imagesizer);
	row			= (unsigned char*) malloc((realwidth + 7) / 8);
	
	if(row == NULL) 
		{
		fprintf(stderr, "Failed to allocate memory.\n");
		exitval = fileErr; // exit(EXIT_FAILURE);
		}
	
	if(noErr == exitval)
		{
		// need to set directory - or use another file write mechanism
		fp = fopen(outfile, "wb");
			
		if(NULL == fp) 
			{
			fprintf(stderr, "Failed to create file: %s\n", outfile);
			perror(NULL);
			exitval = fileErr; // exit(EXIT_FAILURE);
			}
		}
	
	if(noErr == exitval)
		{
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
		if(NULL == png_ptr) 
			{
			fprintf(stderr, "Failed to initialize PNG writer.\n");
			exitval = fileErr; // exit(EXIT_FAILURE);
			}
		}
	
	if(noErr == exitval)
		{
		info_ptr = png_create_info_struct(png_ptr);
		
		if(NULL == info_ptr) 
			{
			fprintf(stderr, "Failed to initialize PNG write.\n");
			exitval = fileErr; // exit(EXIT_FAILURE);
			}
		}
	
	if(noErr == exitval)
		{
		if(0 != setjmp(png_jmpbuf(png_ptr))) 
			{
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fprintf(stderr, "Failed to write PNG image.\n");
			exitval = fileErr; // exit(EXIT_FAILURE);
			}
		}
	
	if((noErr == exitval) && (NULL != row) && (NULL != fp))
		{
		int test;
		png_init_io(png_ptr, fp);
		
		png_set_IHDR(png_ptr, info_ptr,
					realwidth, realwidth,
					1,
					PNG_COLOR_TYPE_GRAY,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT);
		
		png_write_info(png_ptr, info_ptr);

		/* top margin*/
		memset(row, 0xff, (realwidth + 7) / 8);
		for(y = 0; y < (margin* imagesizer); y++) 
			{
			png_write_row(png_ptr, row);
			}

		/* data*/
		p = qrcode->data;
		
		for(y = 0; y < qrcode->width; y++) 
			{
			bit = 7;
			memset(row, 0xff, ((realwidth + 7) / 8));
			q  = row;
			q += ((margin * imagesizer) / 8);
			bit = (7 - (margin * imagesizer % 8));
			
			for(x = 0; x < qrcode->width; x++) 
				{
				for(xx = 0; xx < imagesizer; xx++) 
					{
					*q ^= ((*p & 1) << bit);
					bit--;
					test = bit;
					if(test < 0) 
						{
						q++;
						bit = 7;
						}
					}
				p++;
				}
			
			for(yy = 0; yy < imagesizer; yy++) 
				{
				png_write_row(png_ptr, row);
				}
			}
		
		/* bottom margin*/
		memset(row, 0xff, (realwidth + 7) / 8);
		
		for(y = 0; (y < (margin * imagesizer)); y++) 
			{
			png_write_row(png_ptr, row);
			}
		
		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		}
	
	if(NULL != fp)
		fclose(fp);
	
	if(NULL != row)
		free(row);
	
	return(exitval);
	}

// -------------------------------------------------------------------------

static QRcode* encode(const char* intext)
	{
	QRcode* code;

	if(0 != eightbit) 
		{
		code = QRcode_encodeString8bit(intext, version, level);
		} 
	else 
		{
		code = QRcode_encodeString(intext, version, level, hint, casesensitive);
		}

	return code;
	}

// -------------------------------------------------------------------------
//
// THIS IS THE CALL INTO THIS LIB THAT CREATES A QRcode PNG file FOR GIVEN TEXT
//

int qrencode(const char* intext, const char* outfile, int imagesize)
	{
	QRcode* qrcode = NULL;
	
	if(NULL != intext)
		{
		if(NULL != outfile)
			{
			exitval = noErr;
			qrcode	= encode(intext);
			
			if(NULL == qrcode) 
				{
				perror("Failed to encode the input data:");
				exitval = fileErr; // exit(EXIT_FAILURE);
				}
			
			if(noErr == exitval)
				exitval = writePNG(qrcode, outfile, imagesize);
			
			if(NULL != qrcode) 
				QRcode_free(qrcode);
			}
		}
	return(exitval);
	}

// -------------------------------------------------------------------------
