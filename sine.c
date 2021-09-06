
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <arpa/inet.h>


#define MAX16 32767 
#define MAX24 8388607 
#define SAMPLERATE 44100
#define BITDEPTH 24
#define NUMCHAN 1


typedef int32_t SAMPLE; // Try using sizeof(char * 3) for reading writing

struct WAVEHEADER {

	int chunkID;
	int chunkSize;
	int format;

	int subChunk1ID;
	int subChunk1Size;
	short audioFormat;
	short numChan;
	int smplRate;
	int byteRate;
	short blockAlign;
	short bps; // bits per sample

	int subChunk2ID;
	int subChunk2Size;
} __attribute__((packed)) ;

int main(void) {

	char path[40];
	sprintf(path, "./24sine.wav");
	struct WAVEHEADER* wh = malloc(sizeof(struct WAVEHEADER));
	FILE* wave = fopen(path, "w");
	int numSamples = 512;
	
	// POPULATE MEMBER VARIABLES OF STRUCT:
	
	// wh -> chunkID = 0x52494646;
	wh -> chunkID = 0x46464952; // RIFF
	wh -> chunkSize = 36 + wh -> subChunk2Size;
	wh -> format = 0x45564157; // WAVE
	
	// wh -> subChunk1ID = 0x666d7420;
	wh -> subChunk1ID = 0x20746d66; // fmt_
	wh -> subChunk1Size = 16;
	wh -> audioFormat = 1;
	wh -> numChan = NUMCHAN;
	wh -> smplRate = SAMPLERATE;
	wh -> byteRate = SAMPLERATE * NUMCHAN * ( BITDEPTH / 8 ); // samplerate * numchannels * ( bits per sampler / 8 ) 
	wh -> blockAlign = NUMCHAN * ( BITDEPTH / 8 );
	wh -> bps = BITDEPTH;
	
	// wh -> subChunk2ID = 0x64617461; endianess from documentation
	wh -> subChunk2ID = 0x61746164; // data
	wh -> subChunk2Size = ( numSamples )  * NUMCHAN * ( BITDEPTH / 8 );
	// wh -> format = 0x57415645;

	
	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	/////////////	WRITE SINE

	double curAngle = 0.0f;
	double angleDelta = 0.0f;
	double curVal = 0.0f;
	angleDelta = (M_PI * 2.0) / (numSamples);
	
	for (int i = 0; i < numSamples; ++i) {
		curAngle = sin( angleDelta * i );
		curVal = (MAX24 - 1) * curAngle;
		
		// typecast double to int for PCM format.
		int32_t sample = (curVal);
		// write 3 byte for 24 bit samples.
		fwrite(&sample, 1, 3, wave);
		
		/* unsigned char byte[3] = {0}; */
		/* byte[0] = (sample			& 0xff); */
		/* byte[1] = (( sample >> 8 )	& 0xff); */
		/* byte[2] = (( sample >> 16 )	& 0xff); */

		/* fwrite(&sample, 1, 3, wave); */
		
	}
	fclose(wave);
	free(wh);
}
