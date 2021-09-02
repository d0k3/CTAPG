#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TPS		8   // tile pattern size (for tiled RGB565)
#define HBG		256 // actual height of backgrounds
#define HBGV	240 // visible height of backgrounds

#define RD_BE( v, dt, n ) v = 0; for( int i = 0; i < (n); i++ ) v += (dt)[ i ] << ( i * 8 )
#define WR_BE( v, dt, n ) for( int i = 0; i < (n); i++ ) (dt)[ i ] = ( (v) >> (i*8) ) & 0xFF


int err_exit( const char* msg ) {
	fprintf( stderr, "ERROR: %s\n", msg );
	exit( 0 );
	
	return 0;
}

int rec_build_tpat( int* tp, int x, int y, int w, int t ) {
	if ( t == 1 ) *tp = ( y * w ) + x;
	else {
		t >>= 1;
		rec_build_tpat( tp + ( 0 * t * t ), x    , y    , w, t );
		rec_build_tpat( tp + ( 1 * t * t ), x + t, y    , w, t );
		rec_build_tpat( tp + ( 2 * t * t ), x    , y + t, w, t );
		rec_build_tpat( tp + ( 3 * t * t ), x + t, y + t, w, t );		
	}
	
	return 0;
}

int read_trbg565( FILE* fp, unsigned char* img, int w, int h ) {
	int tp[ TPS * TPS ];
	char pix[ 2 ];
	int p;
	
	rec_build_tpat( tp, 0, 0, w, TPS ); // build tile pattern
	for ( int py = 0; py < h; py += TPS ) {
		for ( int px = 0; px < w; px += TPS ) {
			for ( int i = 0; i < 8*8; i++ ) {
				p = ( ( py * w ) + px + tp[i] ) * 2; // position
				fread( pix, 1, 2, fp );
				memcpy( img + p, pix, 2 );
			}
		}
	}

	return 0;
}

/*int fill_rgb8888( FILE* fp, unsigned char* img, int w, int h ) {
	unsigned char c8888[4];
	unsigned char c565[2];
	
	fread( c8888, 1, 4, fp ); // read colour
	c565[0] = c565[1] = 0x00;
	c565[0] |= ( ( c8888[2] >> 3 ) & 0x1F ) << 3;
	c565[0] |= ( ( c8888[1] >> 5 ) & 0x07 ) << 0;
	c565[1] |= ( ( c8888[1] >> 2 ) & 0x07 ) << 5;
	c565[1] |= ( ( c8888[0] >> 3 ) & 0x1F ) << 0;
	for ( int i = 0; i < 1024 * 256; i++ ) {
		img[ ( 2 * i ) + 0 ] = c565[0];
		img[ ( 2 * i ) + 1 ] = c565[1];
	}
		
	return 0;
}*/

int write_bmp( FILE* fp, unsigned char* img, int w, int h ) {
	unsigned char bmp_hdr[ 0x8A ] = {
		0x42, 0x4D, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x7C, 0x00, // 00000010
		0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00, // 00000020
		0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x0B, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x00, 0x00, // 00000030
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00, // 00000040
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x47, 0x52, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 00000050
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 00000060
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 00000070
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, // 00000080
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	};
	int s = w * h * 2;
	
	// update BMP header with new width / height
	WR_BE( s + 0x8A, bmp_hdr + 0x02, 4 ); // file size
	WR_BE( w, bmp_hdr + 0x12, 4 ); // image width
	WR_BE( (h-1)^0xFFFFFFFF, bmp_hdr + 0x16, 4 ); // image height
	WR_BE( s, bmp_hdr + 0x22, 4 ); // image data size

	// write BMP
	fwrite( bmp_hdr, 1, 0x8A, fp );
	fwrite( img, 1, s, fp );
	
	return 0;
}

int build_preview( unsigned char* prev, unsigned char* top, unsigned char* bot, int wtv, int wt, int wbv, int wb, int wp ) {
	int otp = ( wp - wtv ) / 2; // offset top in preview (may be negative)
	int obp = ( ( wp - wbv ) / 2 ) + ( wp * HBGV ); // offset bottom in preview (may be negative)
	int ott = ( wtv > wp ) ? ( wtv - wp ) / 2 : 0; // offset top in top
	int obb = ( wbv > wp ) ? ( wbv - wp ) / 2 : 0; // offset bottom in bottom

	if ( wtv > wp ) wtv = wp;
	if ( wbv > wp ) wbv = wp;
	memset( prev, 0x00, 2 * 2 * wp * HBGV );
	for ( int l = 0; l < HBGV; l++ ) {
		memcpy( prev + ( ( otp + ott + (l*wp) ) * 2 ), top + ( ( ott + (l*wt) ) * 2 ), wtv * 2 );
		memcpy( prev + ( ( obp + obb + (l*wp) ) * 2 ), bot + ( ( obb + (l*wb) ) * 2 ), wbv * 2 );
	}
	
	return 0;
}

int main( int argc, char** argv ) {		
	unsigned char thm_hdr[ 0xC4 ];
	unsigned char img_top[ 2 * 1024 * 256 ];
	unsigned char img_bot[ 2 * 1024 * 256 ];
	unsigned char img_pre[ 2 * 1008 * 2 * HBGV ];
	FILE* fp;
	
	int wt, wtv;
	int wb, wbv;
	int offs;
	
	
	// preparations
	memset(img_top, 0xFF, 2 * 1024 * 256);
	memset(img_bot, 0xFF, 2 * 1024 * 256);
	wt = 512;
	wtv = 412;
	wb = 512;
	wbv = 320;
	
	// say hello
	fprintf( stderr, "3DS Custom Theme Preview Extractor by d0k3 v0.3\nworking...\n" );
	// check input, open file
	if ( argc != 2 ) err_exit( "too many / little arguments" );
	fp = fopen( argv[ 1 ], "rb" );
	if ( fp == NULL ) err_exit( "invalid file" );
	
	// read theme header to local memory
	fread( thm_hdr, 1, 0xC4, fp );
	
	
	// process upper background
	if ( thm_hdr[ 0x0C ] == 0x03 ) {
		switch ( thm_hdr[ 0x10 ] ) {
			case 0x0:
			case 0x3: wt = 1024; wtv = 1008; break; // texture1
			case 0x1: wt =  512; wtv =  412; break; // texture0
			default: err_exit( "top background invalid texture format" );
		}
		RD_BE( offs, thm_hdr + 0x18, 4 );
		fseek( fp, offs, SEEK_SET );
		read_trbg565( fp, img_top, wt, HBG );
	} else fprintf( stderr, "WARNING: top background has no texture\n" );
	
	
	// process lower background
	if ( thm_hdr[ 0x20 ] == 0x03 ) {
		switch ( thm_hdr[ 0x24 ] ) {
			case 0x0: 
			case 0x3: wb = 1024; wbv = 1008; break; // texture4
			case 0x2:
			case 0x4: wb = 1024; wbv =  960; break; // texture3
			case 0x1: wb =  512; wbv =  320; break; // texture2
			default: err_exit( "bottom background invalid texture format" );
		}
		RD_BE( offs, thm_hdr + 0x28, 4 );
		fseek( fp, offs, SEEK_SET );
		read_trbg565( fp, img_bot, wb, HBG );
	} else fprintf( stderr, "WARNING: bottom background has no texture\n" );
		
	// all done, close theme file
	fclose( fp );
	
	// write top / bottom BMP files
	fp = fopen( "top.bmp", "wb" );
	write_bmp( fp, img_top, wt, HBG );
	fclose( fp );
	fp = fopen( "bottom.bmp", "wb" );
	write_bmp( fp, img_bot, wb, HBG );
	fclose( fp );
	
	// build & write preview image
	build_preview( img_pre, img_top, img_bot, 412, wt, 320, wb, 412 );
	fp = fopen( "preview.bmp", "wb" );
	write_bmp( fp, img_pre, 412, 2 * HBGV );
	fclose( fp );
	
	// build & write CHMM preview image
	build_preview( img_pre, img_top, img_bot, 412, wt, 320, wb, 400 );
	fp = fopen( "preview_chmm.bmp", "wb" );
	write_bmp( fp, img_pre, 400, 2 * HBGV );
	fclose( fp );
	
	// build & write extended preview image
	build_preview( img_pre, img_top, img_bot, wtv, wt, wbv, wb, ( wtv > wbv ) ? wtv : wbv );
	fp = fopen( "preview_ext.bmp", "wb" );
	write_bmp( fp, img_pre, ( wtv > wbv ) ? wtv : wbv, 2 * HBGV );
	fclose( fp );
	
	// all done!
	fprintf( stderr, "SUCCESS: all files generated!\n" );
	
	
	return 1;
}
