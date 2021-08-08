#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Set additional CLI arguments for shaping the wavetable being created.
 * Additional function of translating a .wav into SuperCollider WaveTable-format?
 *
 * TODO: Change algorithm to take in account that audio is a signed number, i.e. -1 .. 1..
 */

// =====================================================================
//
//						WaveTable Generator
//							Linear Ramp:
//
// =====================================================================

#define MAX 65535 
#define SAMPLERATE 44100
#define BITDEPTH 16
#define NUMCHAN 1

typedef int16_t SAMPLE; // 24 bit version might be made in the future
typedef unsigned int int4_t; // redefine regular integer with bytesize in name
typedef short int2_t; // same as above, but 2 byte

struct WAVEHEADER {
	
	int4_t chunkID;
	int4_t chunkSize;
	int4_t format;

	int4_t subChunk1ID;
	int4_t subChunk1Size;
	int2_t audioFormat;
	int2_t numChan;
	int4_t smplRate;
	int4_t byteRate;
	int2_t blockAlign;
	int2_t bps; // bits per sample

	int4_t subChunk2ID;
	int4_t subChunk2Size;

} __attribute__((packed)) ;



int main (int argc, char** argv) {


	if (argc != 3) {
		printf("Not correct use of WaveTable Generator! Do better.\n ()");
		printf("(./wave <name-of-output-file> <sample-size>)");
		return 1;
	} else {
		for (int i = 0, n = strlen(argv[2]); i < n; i++) {
			if (!isdigit(argv[2][i])) {
				printf("<sample-size> must be a number, preferably a pow^2");
				return 2;
			}
		}
	}
	
	int sampleSize = atoi(argv[2]);

	struct WAVEHEADER* wh = malloc(sizeof(struct WAVEHEADER));
	
	char path[100];
	sprintf(path, "/Users/viktorsandstrom/Documents/C/projects/wavetable/%s.wav", argv[1]);
	
	FILE* wave = fopen(path, "w");


	// POPULATE MEMBER VARIABLES OF STRUCT:
	
	// wh -> subChunk2ID = 0x64617461; endianess from documentation
	wh -> subChunk2ID = 0x61746164; // reverse endian
	wh -> subChunk2Size = ( sampleSize * 2)  * NUMCHAN * ( BITDEPTH / 8 );
	
	// wh -> chunkID = 0x52494646;
	wh -> chunkID = 0x46464952;
	wh -> chunkSize = 36 + wh -> subChunk2Size;
	// wh -> format = 0x57415645;
	wh -> format = 0x45564157;

	// wh -> subChunk1ID = 0x666d7420;
	wh -> subChunk1ID = 0x20746d66;
	wh -> subChunk1Size = 16;
	wh -> audioFormat = 1;
	wh -> numChan = NUMCHAN;
	wh -> smplRate = SAMPLERATE;
	wh -> byteRate = SAMPLERATE * NUMCHAN * ( BITDEPTH / 8 ); // samplerate * numchannels * ( bits per sampler / 8 ) 
	wh -> bps = BITDEPTH;

	// Write header to file
	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	float* sampleVal = malloc(sizeof(SAMPLE));
	float sixteenBit = MAX;

	int flag = 0;
	int count = 0;

	for ( int i = 0; i < ( sampleSize * 2 ); ++i ) {
			
		if ( flag == 0 ) {

			*sampleVal = 0;
			flag = 1;

			count++;
		} else if ( flag == 1 ){

			*sampleVal = ( ( sixteenBit / 2) / ( sampleSize * 2) ) * i + 1;
			flag = 0;

			count++;
		}


		fwrite(sampleVal, sizeof(SAMPLE), 1, wave);
	
	}

	fclose(wave);

	free(sampleVal);

	int headSize = sizeof(struct WAVEHEADER);

	printf("Size of header: %i  \nSize of samples: %i  -- which size is: %i\n", headSize, count, count*16);
	printf("Total size of file: %i\n", 8 + wh -> chunkSize);

	free(wh);

	return 0;

}
