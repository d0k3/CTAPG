#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int err_exit( const char* msg ) {
	fprintf( stderr, "ERROR: %s\n", msg );
	exit( 0 );
	
	return 0;
}

int main( int argc, char** argv ) {		
	char string[ 0xC8 ];
	FILE* fp_in;
    FILE* fp_out;
	
	
	// say hello
	fprintf( stderr, "3DS ContentInfoArchive Text Extractor by d0k3 v0.1\nworking..." );
	// check input, open file
	if ( argc != 3 ) err_exit( "too many / little arguments" );
	
    // open content info archive
	fp_in = fopen( argv[ 1 ], "rb" );
	if ( fp_in == NULL ) err_exit( "invalid file" );
    fseek( fp_in, 0x08, SEEK_SET );
    
    // open text file
    fp_out = fopen( argv[ 2 ], "wb" );
    
    // write away
    fprintf(stderr, "\n");
    while( fread(string, 1, 0xC8, fp_in) == 0xC8 ) {
        fprintf(fp_out, "%s\n", string);
        fprintf(stderr, "%s\n", string);
    }
    
    // close files
    fclose(fp_in);
    fclose(fp_out);
	
	// all done!
	fprintf( stderr, "SUCCESS: text extracted!\n" );
	
	
	return 1;
}