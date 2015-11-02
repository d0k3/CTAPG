#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TPS		8   // tile pattern size (for tiled RGB565)
#define ICNW	48 // icon width
#define ICNH	48 // icon height

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

int main( int argc, char** argv ) {		
	unsigned char data[ ICNW * ICNH * 2 ];
	FILE* fp;
	
	
	// say hello
	fprintf( stderr, "3DS Custom Theme Icon Converter by d0k3 v0.1\nworking..." );
	// check input, open file
	if ( argc != 3 ) err_exit( "too many / little arguments" );
	
    // read icon data
	fp = fopen( argv[ 1 ], "rb" );
	if ( fp == NULL ) err_exit( "invalid file" );
    read_trbg565(fp, data, ICNW, ICNH);
    fclose(fp);
    
    // write to file
    fp = fopen( argv[ 2 ], "wb" );
	write_bmp( fp, data, ICNW, ICNH );
	fclose( fp );
	
	// all done!
	fprintf( stderr, "SUCCESS: icon converted!\n" );
	
	
	return 1;
}
