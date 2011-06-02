/*
 *  qrenc.h
 *  QRencode
 *
 *  Created by Lance Drake on 11/5/10.
 *  Copyright 2010 Caprice Productions. All rights reserved.
 *
 */

#import <stdio.h>
#import <stdlib.h>
#import <getopt.h>
#import "png.h"
#import "config.h"
#import "qrencode.h"

#ifndef noErr
#define noErr 0
#endif

#ifndef fileErr
#define fileErr -1
#endif

// ---------------------------------------------------------

//static int writePNG(QRcode* qrcode, const char* outfile, int imagesize);

//static QRcode* encode(const char* intext);

// This is the call in from the outside
//static int qrencode(const char* intext, const char* outfile);

#if defined(__cplusplus)
extern "C" {
#endif
	
int qrencode(const char* intext, const char* outfile, int imagesize);
	
#if defined(__cplusplus)
}
#endif
