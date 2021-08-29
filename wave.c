#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

/*
 *
 * TODO: Set bitDepth, sampleRate and numSamples by reading header of input file.
 * TODO: Additional function of translating a .wav into SuperCollider WaveTable-format?
 * TODO: Be able to use both 16 and 24 bit depth.
 *
 */

// =====================================================================
//
//						WaveTable Generator
//				   Sine | Saw |	Triangle | Square
//
// =====================================================================

// Definde macros:
#define MAX16 32767 
#define MAX24 8388607
#define SAMPLERATE 44100
#define BITDEPTH 16
#define NUMCHAN 1

// Typedef and Struct:
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

// Function prototype:
void shapeSwitch( char symbol, int numSamples, FILE* file);

////////////////////////////////////////////////////////////////////////////////
///////////////////				Main Function			   /////////////////////
////////////////////////////////////////////////////////////////////////////////

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
	

	// Write header to file
	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	
	char symbol = 0;

	printf("------->	");

	if (argc == 4) {	// 4 shapes: sine, triangle, sawtooth or softsquare
						// 's' = sine, '^' = triangle, 'z' = sawtooth, 'n' = softsquare.

		if (!strcmp(argv[3], "sine") || (!strcmp(argv[3], "sin"))) {
			symbol = 's';
		} else if (!strcmp(argv[3], "triangle") || (!strcmp(argv[3], "tri"))) {
			symbol = '^';
		} else if (!strcmp(argv[3], "saw") || (!strcmp(argv[3], "sawtooth"))) {
			symbol = 'z';
		} else if (!strcmp(argv[3], "square") || (!strcmp(argv[3], "softsquare")) || (!strcmp(argv[3], "sqr"))){
			symbol = 'n';
		} else {
			exit(0);
		}

		shapeSwitch(symbol, numSamples, wave);
			
	} else {				// If no waveform is given at CLI
							// you get a random one.
		
		srand(time(NULL));
		rand();
		int random = rand() % 4;

		if ( random == 0 ) {
			symbol = 's';
		} else if ( random == 1 ) {
			symbol = '^';
		} else if ( random == 2 ) {
			symbol = 'z';
		} else {
			symbol = 'n';
		}

		shapeSwitch(symbol, numSamples, wave);

	}
	
	fclose(wave);

	free(sampleVal);

	free(wh);

	return 0;

}

////////////////////////////////////////////////////////////////////////////////
///////////////////				Helper Functions		   /////////////////////
////////////////////////////////////////////////////////////////////////////////

void shapeSwitch( char symbol, int numSamples, FILE* file) {

	int count = 0;
		
	switch (symbol) { 
		case 's':
			printf("sine\n");

			double currentAngle = 0;
			double angleDelta = 0;
			float sinCurVal = 0;
			float ssamples = ( numSamples * 2 );
			angleDelta = ( M_PI * 2 ) / numSamples;
			printf("%f", angleDelta);
			float max =  MAX16 - 1 ;
			int j = 0;


			int sineflag = 1;

			for (int i = 0; i < ssamples; ++i) { 
				
				if (!sineflag) {

					sinCurVal = 0;
					sineflag = 1;
				} else if (sineflag){

					// printf("Value from sin() : %f \n", sin( angleDelta * j ));
					currentAngle = sin( angleDelta * j );
					sinCurVal = max * currentAngle;

					// printf("2pi / nSamples:		%f\nsin(%f * %i):	%f \nresult * max:		%f\n", angleDelta, angleDelta, j, currentAngle, sinCurVal);

					j++;

					sineflag = 0; 

				}
				SAMPLE newsamp = (int) sinCurVal;
				fwrite(&newsamp, sizeof(SAMPLE), 1, file);
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
			float zsamples = numSamples * 2;
			

			for ( int i = 0; i < zsamples; ++i ) {
					
				if (!sawflag) {

					sawCurVal = 0;

					sawflag = 1;

					count++;
				} else if (sawflag){ // Write bipolar saw from maximum positive range to maximum negative range.

			 		sawCurVal = MAX16 - ( ( MAX16 * 2 ) / zsamples ) * i + 1;
					sawflag = 0;

					count++;
				}

				// printf("%i\n", sawCurVal);

				fwrite(&sawCurVal, sizeof(SAMPLE), 1, file);
			}
			break;
		case 'n':
			printf("softsquare\n");
			
			int sqrflag = 1;
			SAMPLE sqrCurVal = 0;
			const SAMPLE MAXSQR = MAX16 - 1;
			float decrement = 0;
			float nsamples = numSamples * 2;

			for (int i = 0; i < ( nsamples / 10 ); ++i) { // ramp up
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

			for (int j = 0; j < ( ( nsamples / 10 ) * 3 ); ++j) { // pos up
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					sqrCurVal = MAXSQR;
					sqrflag = 0;

				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}

			for (int k = 0; k < ( nsamples / 10 ); ++k) { // ramp down
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					decrement = MAXSQR / ( ( nsamples / 10 ) - ( k + 1 ) );
					sqrCurVal = MAXSQR - decrement;
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}
			
			for (int l = 0; l < ( nsamples / 10 ); ++l) { // neg-ramp down
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

			for (int m = 0; m < ( ( nsamples / 10 ) * 3 ); ++m) { // neg-down
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;

				} else if (sqrflag) {
					sqrCurVal = ( -1 * MAXSQR );
					sqrflag = 0;
				}
			fwrite(&sqrCurVal, sizeof(SAMPLE), 1, file);
			}
			
			for (int n = 0; n < ( nsamples / 10 ); ++n) { // neg-ramp up
				if (!sqrflag){
					sqrCurVal = 0;
					sqrflag = 1;
				} else if (sqrflag) {
					decrement = (-1 * MAXSQR ) / ( ( nsamples / 10 ) - ( n + 1 ) );
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
