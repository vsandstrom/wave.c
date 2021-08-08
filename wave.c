#include <stdlib.h>
#include <stdio.h>

/*
 * Set additional CLI arguments for shaping the wavetable being created.
 * Additional function of translating a .wav into SuperCollider WaveTable-format?
 *
 * TODO: Change algorithm to take in account that audio is a signed number, i.e. -1 .. 1.
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

	int sampleSize = atoi(argv[1]);

	if (argc != 2) {
		printf("Not correct use of WaveTable Generator! Do better.\n ()");
		return 1;
	}

	struct WAVEHEADER* wh = malloc(sizeof(struct WAVEHEADER));

	wh -> chunkID = 0x52494646;
	wh -> chunkSize = 36 + ( sampleSize * 2 );
	wh -> format = 0x57415645;

	wh -> subChunk1ID = 0x666d7420;
	wh -> subChunk1Size = 16;
	wh -> audioFormat = 1;
	wh -> numChan = NUMCHAN;
	wh -> smplRate = SAMPLERATE;
	wh -> byteRate = SAMPLERATE * NUMCHAN * ( BITDEPTH / 8 ); // samplerate * numchannels * ( bits per sampler / 8 ) 
	wh -> bps = BITDEPTH;

	wh -> subChunk2ID = 0x64617461;
	wh -> subChunk2Size = ( sampleSize * 2)  * NUMCHAN * ( BITDEPTH / 8 );




	
	FILE* wave = fopen("/Users/viktorsandstrom/Documents/C/projects/wavetable/wave.wav", "w");

	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	float* sampleVal = malloc(sizeof(SAMPLE));

	int flag = 0;

	for ( int i = 0; i < ( sampleSize * 2 ); ++i ) {
			
		if ( flag == 0 ) {

			*sampleVal = 0;
			flag = 1;
		} else if ( flag == 1 ){

			*sampleVal = ( MAX / sampleSize ) * i + 1;
			flag = 0;
		}


		fwrite(sampleVal, sizeof(SAMPLE), sampleSize, wave);
	
	}

	fclose(wave);

	free(sampleVal);

	return 0;

}
