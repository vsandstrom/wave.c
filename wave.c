#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/*
 * Set additional CLI arguments for shaping the wavetable being created.
 * Additional function of translating a .wav into SuperCollider WaveTable-format?
 *
 * TODO: Change algorithm to take in account that audio is a signed number, i.e. -1 .. 1..
 *
 * TODO: continue switch-statement for different shapes. implement algorithms 
 *
 * TODO: Set bitDepth, sampleRate and numSamples by reading header of input file.
 *
 */

// =====================================================================
//
//						WaveTable Generator
//							Linear Ramp:
//
// =====================================================================

#define MAX16 32767 
#define MAX24 8388607
#define SAMPLERATE 44100
#define BITDEPTH 16
#define NUMCHAN 1

typedef int16_t SAMPLE; // 24 bit version might be made in the future
typedef int32_t SAMPLE24; // Try using sizeof(char * 3) for reading writing


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

void shapeSwitch( char symbol, int numSamples, FILE* file);


int main (int argc, char** argv) {


	if (argc < 3 || argc > 4 ) {
		printf("Not correct use of WaveTable Generator! Do better.\n");
		printf("(./wave <name-of-output-file> <sample-size> (opt: <wave-shape>))");
		return 1;
	} else {
		for (int i = 0, n = strlen(argv[2]); i < n; i++) {
			if (!isdigit(argv[2][i])) {
				printf("<sample-size> must be a number, preferably a pow^2");
				return 2;
			}
		}
	}
	
	struct WAVEHEADER* wh = malloc(sizeof(struct WAVEHEADER));
	
	SAMPLE* sampleVal = malloc(sizeof(SAMPLE));
	if (sampleVal == NULL) {
		printf("memalloc failed");
		return 3;
	}
	
	int numSamples = atoi(argv[2]);
	
	char path[40];
	sprintf(path, "./%s.wav", argv[1]);
	
	FILE* wave = fopen(path, "w");
	if (wh == NULL) {
		printf("memalloc failed");
		return 3;
	
	}
	
	// POPULATE MEMBER VARIABLES OF STRUCT:
	
	// wh -> subChunk2ID = 0x64617461; endianess from documentation
	wh -> subChunk2ID = 0x61746164; // reverse endian
	wh -> subChunk2Size = ( numSamples * 2)  * NUMCHAN * ( BITDEPTH / 8 );
	
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
	
	int sixteenBit = MAX16;

	// Write header to file
	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	int count = 0;

	if (argc == 4) {  // 4 shapes: sine, triangle, sawtooth or softsquare
			// 's' = sine, '^' = triangle, 'z' = sawtooth, 'n' = softsquare.
		char symbol = 0;

		if (!strcmp(argv[3], "sine")) {
			symbol = 's';
		} else if (!strcmp(argv[3], "triangle")) {
			symbol = '^';
		} else if (!strcmp(argv[3], "saw") || (!strcmp(argv[3], "sawtooth"))) {
			symbol = 'z';
		} else if (!strcmp(argv[3], "square") || (!strcmp(argv[3], "softsquare"))){
			symbol = 'n';
		} else {
			exit(0);
		}

		shapeSwitch(symbol, numSamples, wave);
			
	} else {
	
		int flag = 0;

		for ( int i = 0; i < ( numSamples * 2 ); ++i ) {
				
			if ( flag == 0 ) {

				*sampleVal = 0;
				flag = 1;

				count++;
			} else if ( flag == 1 ){ // Write bipolar saw from maximum positive range to maximum negative range.

				*sampleVal = round( sixteenBit - ( ( sixteenBit * 2 ) / ( numSamples * 2) ) * i + 1 );
				flag = 0;

				count++;
			}


			printf("%i\n", *sampleVal);


			fwrite(sampleVal, sizeof(SAMPLE), 1, wave);
		
		}

	}
	
	fclose(wave);

	free(sampleVal);

	int headSize = sizeof(struct WAVEHEADER);

	printf("Size of header: %i  \nSize of samples: %i  -- which size is: %i\n", headSize, count, count*16);
	printf("Total size of file: %i\n", 8 + wh -> chunkSize);

	free(wh);

	return 0;

}


void shapeSwitch( char symbol, int numSamples, FILE* file) {

	int count = 0;
		
	switch (symbol) { // Still not working tho:
		case 's':					// SINE wavetable stolen from JUCE tutorial.

			printf("sine\n");

			double currentAngle = 0.0f, angleDelta = 0.0f;
			SAMPLE currentValue = 0;

			angleDelta = M_2_PI / ( numSamples - 1);

			int sineflag = 0;

			for (int i = 0; i < numSamples * 2; ++i) { // use sin() to get slices from a sine. sin( ( 360/numSamples ) * iteratorVariable ) * SAMPLE
				// ONLY EXPORTS NOISE! NOT WORKING... 
				
				if (sineflag) {

					currentValue = 0;
					sineflag = 0;

					count++;
				} else {
					currentValue = sin(currentAngle);
					
					currentAngle += angleDelta;
					/* if (currentAngle >= M_2_PI){ */
					/* 	 currentAngle -= M_2_PI; */
					/* } */
					sineflag = 1; 

					/* printf("%f\n", currentAngle); */
					/* printf( "degPerSample * numSamples = %f", currentAngle ); */

					count++;
				}

				
				fwrite(&currentValue, sizeof(SAMPLE), 1, file);
				printf("%i", currentValue);
			}

			break;
		case '^':
			printf("triangle\n");

			int triflag = 1;
			SAMPLE triCurVal = 0;
			float increment = ( MAX16 - 1 ) / ( numSamples / 4 ); // Calculate increment per sample in tri
			float tempCur = 0;

			// 4 segments: up | down | neg-down | neg-up

			for (int i = 0; i < ( numSamples / 2 ); ++i) { // up

				if (!triflag) {
					triCurVal = 0;
					triflag = 1;

				} else if (triflag) {
					tempCur += increment;
					triCurVal = tempCur;
					triflag = 0;
				}

				fwrite(&triCurVal, sizeof(SAMPLE), 1, file);
			}
			
			for (int j = 0; j < ( numSamples ); ++j) { // neg-down

				if (!triflag) {
					triCurVal = 0;
					triflag = 1;

				} else if (triflag) {
					tempCur -= increment;
					triCurVal = tempCur;
					triflag = 0;
				}
				fwrite(&triCurVal, sizeof(SAMPLE), 1, file);
			}

			for (int k = 0; k < ( numSamples / 2 ); ++k) { // neg-up

				if (!triflag) {
					triCurVal = 0;
					triflag = 1;

				} else if (triflag) {
					tempCur += increment;
					triCurVal = tempCur;
					triflag = 0;
				}

				fwrite(&triCurVal, sizeof(SAMPLE), 1, file);
				
			}

			break;
		case 'z':
			printf("sawtooth\n");
			int sawflag = 1;
			
			SAMPLE sawCurVal = 0;
			

			for ( int i = 0; i < ( numSamples * 2 ); ++i ) {
					
				if (!sawflag) {

					sawCurVal = 0;

					sawflag = 1;

					count++;
				} else if (sawflag){ // Write bipolar saw from maximum positive range to maximum negative range.

			 		sawCurVal = round( MAX16 - ( ( MAX16 * 2 ) / ( numSamples * 2) ) * i + 1 );
					sawflag = 0;

					count++;
				}

				printf("%i\n", sawCurVal);

				fwrite(&sawCurVal, sizeof(SAMPLE), 1, file);
			}
			break;
		case 'n':
			printf("softsquare");
			
			int sqrflag = 0;
			SAMPLE sqrCurVal = 0;
			const SAMPLE MAXSQR = MAX16 - 1;
			float decrement = 0;
			float samples = numSamples * 2;

			for (int i = 0; i < ( samples / 10 ); ++i) { // ramp up
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					decrement = MAXSQR / ( i + 1 );
					sqrCurVal = MAXSQR - decrement;
					sqrflag = 0;

				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}

			for (int j = 0; j < ( ( samples / 10 ) * 3 ); ++j) { // pos up
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					sqrCurVal = MAXSQR;
					sqrflag = 0;

				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}

			for (int k = 0; k < ( samples / 10 ); ++k) { // ramp down
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					decrement = MAXSQR / ( ( samples / 10 ) - ( k + 1 ) );
					sqrCurVal = MAXSQR - decrement;
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}
			
			for (int l = 0; l < ( samples / 10 ); ++l) { // neg ramp down
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					decrement = MAXSQR / ( l + 1 );
					sqrCurVal =  -1 * ( MAXSQR - decrement );
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}

			for (int m = 0; m < ( ( samples / 10 ) * 3 ); ++m) { // neg down
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					sqrCurVal = ( -1 * MAXSQR );
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}
			
			for (int n = 0; n < ( samples / 10 ); ++n) { // ramp up
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;
				} else if (sqrflag) {
					decrement = (-1 * MAXSQR ) / ( ( samples / 10 ) - ( n + 1 ) );
					sqrCurVal = ( -1 * MAXSQR ) - decrement;
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}

			break;
		default:
			printf("something went wrong...");
			break;


	}
}
