
/*
 *
 * TODO: Set bitDepth, sampleRate and numSamples by reading header of input file.
 * TODO: Additional function of translating a .wav into SuperCollider WaveTable-format?
 * TODO: Be able to use both 16 and 24 bit depth.
 * TODO: Function for parsing the "bext" chunk in wave header.
 *
 */

// =====================================================================
//
//						WaveTable Generator
//				   Sine | Saw |	Triangle | Square
//
// =====================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define MAX16 32767 
#define MAX24 8388607
#define DEFSAMPLERATE 44100
#define DEFNUMCHAN 1
#define DEFBITDEPTH 24

int BITDEPTH = 0;
int SAMPLERATE = 0;
int NUMCHAN = 0;

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


void createSin( int numSamples, FILE* file, int32_t sampleMax );
void createSqr( int numSamples, FILE* file, int32_t sampleMax );
void createTri( int numSamples, FILE* file, int32_t sampleMax );
void createSaw( int numSamples, FILE* file, int32_t sampleMax );


int main (int argc, char** argv) {

	// ./wave output.wav -o

	if (argc < 3 || argc > 4 ) {
		printf("Not correct use of WaveTable Generator! Do better.\n");
		printf("(./wave <name-of-output-file> <sample-size> (opt: <wave-shape>))\n");
		printf("or: (./wave <name-of-output-file> <[-o] of [open]>)\n");
		
		return 1;
	} 
	
	char path[40];
	sprintf(path, "./%s.wav", argv[1]);

	if (argc == 4) {
		for (int i = 0, n = strlen(argv[2]); i < n; i++) {
			if (!isdigit(argv[2][i])) {
				printf("<sample-size> must be a number, power of ^2");
				return 2;
			}
			else if (atoi(argv[2]) % 2 != 0) {
				printf("<sample-size> must be a number, power of ^2");
				return 2;
			}
		}
	
	
		struct WAVEHEADER* wh = malloc(sizeof(struct WAVEHEADER));
		int numSamples = atoi(argv[2]);

		FILE* wave = fopen(path, "w");
		if (wh == NULL) {

			printf("memalloc failed");
			return 3;
		}
		
		// POPULATE MEMBER VARIABLES OF STRUCT:
		
		// wh -> subChunk2ID = 0x64617461; endianess from documentation
		wh -> subChunk2ID = 0x61746164; // reverse endian
		wh -> subChunk2Size = ( numSamples )  * DEFNUMCHAN * ( DEFBITDEPTH / 8 );
		
		// wh -> chunkID = 0x52494646;
		wh -> chunkID = 0x46464952;
		wh -> chunkSize = 36 + wh -> subChunk2Size;
		// wh -> format = 0x57415645;
		wh -> format = 0x45564157;

		// wh -> subChunk1ID = 0x666d7420;
		wh -> subChunk1ID = 0x20746d66;
		wh -> subChunk1Size = 16;
		wh -> audioFormat = 1;
		wh -> numChan = DEFNUMCHAN;
		wh -> smplRate = DEFSAMPLERATE;
		wh -> byteRate = DEFSAMPLERATE * DEFNUMCHAN * ( DEFBITDEPTH / 8 ); // samplerate * numchannels * ( bits per sampler / 8 ) 
		wh -> blockAlign = DEFNUMCHAN * ( DEFBITDEPTH / 8 );
		wh -> bps = DEFBITDEPTH;
		
		fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

		printf("------->	");




		if (!strcmp(argv[3], "sine") || (!strcmp(argv[3], "sin"))) {
			createSin( numSamples, wave, MAX24 );
		} else if (!strcmp(argv[3], "triangle") || (!strcmp(argv[3], "tri"))) {
			createTri( numSamples, wave, MAX24 );
		} else if (!strcmp(argv[3], "saw") || (!strcmp(argv[3], "sawtooth"))) {
			createSaw( numSamples, wave, MAX24 );
		} else if (!strcmp(argv[3], "square") || (!strcmp(argv[3], "softsquare")) || (!strcmp(argv[3], "sqr"))){
			createSqr( numSamples, wave, MAX24 );
		} else {
			exit(0);
		}
	
	fclose(wave);
	free(wh);
	return 0;
	
	} 

	else if (argc == 3) {  
			
		/* if (strcmp(argv[2], "open") != 0  || strcmp(argv[2], "-o") != 0) { */
		/* 	printf("Incorrect command\n"); */
		/* 	return 1; */
		/* } */
	 	FILE* raw = fopen( path, "r" ); 
		if (raw == NULL){
			printf("Cannot open file\n");
			return 1;
		}	

	 	char dest_path[40]; 
	 	sprintf(dest_path, "./%s_wavetable.wav", argv[1]); 
	 	FILE* dest = fopen( dest_path, "w" ); 
		if (dest == NULL){
			printf("Something went wrong\n");
			return 1;
		}

		struct WAVEHEADER* header = malloc(sizeof(struct WAVEHEADER));

		fread(header, sizeof(struct WAVEHEADER), 1, raw);
		if (header -> audioFormat != 1){
			printf("Input format must be PCM\n");
			return 2;
		}
		if (header -> numChan != 1){
			printf("input must be mono\n");
			return 2;
		}
		
		NUMCHAN = header -> numChan;
		BITDEPTH = header -> bps;
		SAMPLERATE = header -> smplRate;
		int numSamples = header -> subChunk2Size / BITDEPTH / 3;

		if (SAMPLERATE % 2 != 0){
			SAMPLERATE += 1;
			header -> subChunk2Size = SAMPLERATE * BITDEPTH / 3;
		}

		fwrite( header, sizeof(struct WAVEHEADER), 1, dest );

		printf("ChunkID: %x\n", header -> chunkID);
		printf("ChunkSize: %i\n", header -> chunkSize);
		printf("Format: %x\n\n", header -> format);
		
		printf("SubChunkID 1: %x\n", header -> subChunk1ID);
		printf("SubChunkSize 1: %i\n", header -> subChunk1Size);
		printf("AudioFormat: %i\n", header -> audioFormat);
		printf("NumChannels: %i\n", header -> numChan);
		printf("SampleRate: %i\n", header -> smplRate);
		printf("ByteRate: %i\n", header -> byteRate);
		printf("BlockAlign: %i\n", header -> blockAlign);
		printf("BitsPerSample: %i\n\n", header -> bps);

		printf("SubChunkID 2: %x\n", header -> subChunk2ID);
		printf("SubChunkSize 2 %i\n", header -> subChunk2Size);


		


		int numBytes = BITDEPTH / 8;

		SAMPLE sample = 0;
		int flag = 0;

		while (fread( &sample, 1, numBytes, raw )) {
			if (!flag){ 
				fwrite( &sample, 1, numBytes, dest );	
				flag = 1;
			} else if (flag) {
				flag = 0;
				fwrite( &flag, 1, numBytes, dest ); 
			}
		}

		free(header);
		fclose(raw);
		fclose(dest);

		return 0;
	}
}


void createSin( int numSamples, FILE* file, int32_t MAXDEPTH ) {
	printf("sine\n");
	int flag = 1;

	double currentAngle = 0;
	double angleDelta = 0;
	float curVal = 0;
	angleDelta = ( M_PI * 2 ) / ( numSamples * 0.5 );
	printf("%f", angleDelta);
	float sinMax = MAXDEPTH - 1 ;
	int j = 0;

	for (int i = 0; i < numSamples; ++i) { // every even sample is '0'
		if (!flag) {
			curVal = 0;
			flag = 1;

		} else if (flag){
			currentAngle = sin( angleDelta * j );
			curVal = sinMax * currentAngle;
			j++;
			flag = 0; 

		}
		SAMPLE sample = (int) curVal;
		fwrite(&sample, 1, 3, file);
		
		//fwrite(&newsamp, sizeof(SAMPLE), 1, file);
	}
}

void createSaw(int numSamples, FILE* file, int32_t MAXDEPTH) {
	printf("sawtooth\n");
	int flag = 1;
	SAMPLE curVal = 0;

	for ( int i = 0; i < numSamples; ++i ) {
		if (!flag) {
			curVal = 0;
			flag = 1;

		} else if (flag){ // Write bipolar saw from maximum positive range to maximum negative range.
			curVal = ( MAXDEPTH - 1 ) - ( ( MAXDEPTH * 2 ) / numSamples ) * i + 1;
			flag = 0;

		}

		fwrite(&curVal, 1, 3, file);

	}
}

void createTri( int numSamples, FILE* file, int32_t MAXDEPTH) {
	printf("triangle\n");
	int flag = 1;

	SAMPLE curVal = 0;
	float increment = ( MAXDEPTH - 1 ) / ( numSamples * 0.125 ); // Calculate increment per sample in tri
	float tempCur = 0;

	for (int i = 0; i < ( numSamples * 0.25 ); ++i) { // up
		if (!flag) {
			curVal = 0;
			flag = 1;

		} else if (flag) {
			tempCur += increment;
			curVal = tempCur;
			flag = 0;
		}
		fwrite(&curVal, 1, 3, file);
	}

	for (int j = 0; j < ( numSamples * 0.5 ); ++j) { // neg-down
		if (!flag) {
			curVal = 0;
			flag = 1;

		} else if (flag) {
			tempCur -= increment;
			curVal = tempCur;
			flag = 0;
		}
		fwrite(&curVal, 1, 3, file);
	}

	for (int k = 0; k < ( numSamples * 0.25 ); ++k) { // neg-up
		if (!flag) {
			curVal = 0;
			flag = 1;

		} else if (flag) {
			tempCur += increment;
			curVal = tempCur;
			flag = 0;
		}
		fwrite(&curVal, 1, 3, file);
	}
}

void createSqr( int numSamples, FILE* file, int32_t MAXDEPTH) {
	printf("softsquare\n");
	int flag = 1;

	float curVal = 0;
	const float sqrDepth = MAXDEPTH - 1;
	float decrement = 0;
	int newsamp = 0;

	for (int i = 0; i < ( numSamples * 0.1 ); ++i) { // ramp up
		if (!flag){
			curVal = 0;
			flag = 1;

		} else if (flag) {
			decrement = sqrDepth / ( i + 1 );
			curVal = sqrDepth - decrement;
			flag = 0;

		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}

	for (int j = 0; j < ( ( numSamples * 0.1 ) * 3 ); ++j) { // pos up
		if (!flag){
			curVal = 0;
			flag = 1;

		} else if (flag) {
			curVal = sqrDepth;
			flag = 0;

		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}

	for (int k = 0; k < ( numSamples * 0.1 ); ++k) { // ramp down
		if (!flag){
			curVal = 0;
			flag = 1;

		} else if (flag) {
			decrement = sqrDepth / ( ( numSamples * 0.1 ) - ( k + 1 ) );
			curVal = sqrDepth - decrement;
			flag = 0;
		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}
	
	for (int l = 0; l < ( numSamples * 0.1 ); ++l) { // neg-ramp down
		if (!flag){
			curVal = 0;
			flag = 1;

		} else if (flag) {
			decrement = sqrDepth / ( l + 1 );
			curVal =  -1 * ( sqrDepth - decrement );
			flag = 0;
		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}

	for (int m = 0; m < ( ( numSamples * 0.1 ) * 3 ); ++m) { // neg-down
		if (!flag){
			curVal = 0;
			flag = 1;

		} else if (flag) {
			curVal = ( -1 * sqrDepth );
			flag = 0;
		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}
	
	for (int n = 0; n < ( numSamples * 0.1 ); ++n) { // neg-ramp up
		if (!flag){
			curVal = 0;
			flag = 1;
		} else if (flag) {
			decrement = (-1 * sqrDepth ) / ( ( numSamples * 0.1 ) - ( n + 1 ) );
			curVal = ( -1 * sqrDepth ) - decrement;
			flag = 0;
		}
	newsamp = (int) curVal;
	fwrite(&newsamp, 1, 3, file);
	}
}
