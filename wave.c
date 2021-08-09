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
 */

// =====================================================================
//
//						WaveTable Generator
//							Linear Ramp:
//
// =====================================================================

#define MAX 32767 
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

void shapeSwitch( char shape[], SAMPLE* pointer, int numSamples, FILE* file);

int main (int argc, char** argv) {


	if (argc < 3 || argc > 4 ) {
		printf("Not correct use of WaveTable Generator! Do better.\n ()");
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
	
	char path[100];
	sprintf(path, "/Users/viktorsandstrom/Documents/C/projects/wavetable/%s.wav", argv[1]);
	
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
	
	int sixteenBit = MAX;

	// Write header to file
	fwrite(wh, sizeof(struct WAVEHEADER), 1, wave);

	int count = 0;

	if (argc == 4) {  // 4 shapes: sine, triangle, sawtooth or softsquare
		if(!strcmp(argv[3], "sine") || !strcmp(argv[3], "triangle") || !strcmp(argv[3], "saw") || !strcmp(argv[3], "ssquare")) {

			shapeSwitch(argv[3], sampleVal, numSamples, wave);
			

		}
	} else {
	
		int flag = 0;

		for ( int i = 0; i < ( numSamples * 2 ); ++i ) {
				
			if ( flag == 0 ) {

				*sampleVal = 0;
				flag = 1;

				count++;
			} else if ( flag == 1 ){

				*sampleVal = round( ( ( sixteenBit ) / ( numSamples * 2) ) * i + 1 );
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

void shapeSwitch( char shape[], SAMPLE* pointer, int numSamples, FILE* file) {

	char symbol = 0;
		
		// 's' = sine, '^' = triangle, 'z' = sawtooth, 'n' = softsquare.
	if (!strcmp(shape, "sine")) {
		symbol = 's';
	} else if (!strcmp(shape, "triangle")) {
		symbol = '^';
	} else if (!strcmp(shape, "saw")) {
		symbol = 'z';
	} else {
		symbol = 'n';
	}



	switch (symbol) { // REMOVE return-statements later on!
		case 's':
			printf("sine");
			float degPerSample = 360.0 / ( numSamples * 2 );
			int flag = 0;


			for (int i = 0; i < numSamples * 2; ++i) { // use sin() to get slices from a sine. sin( ( 360/numSamples ) * iteratorVariable ) * SAMPLE
				// ONLY EXPORTS NOISE! NOT WORKING... 
				
				if (flag) {

					*pointer = 0;
					flag = 0;

				} else {
					printf( "degPerSample * numSamples = %f", degPerSample * ( numSamples / 2 ) );
					*pointer = round( sin( degPerSample * i ) );
					printf("%f\n", round( sin(degPerSample * i) ) );


					flag = 1; 
				}


				fwrite(pointer, sizeof(SAMPLE), 1, file);
			}

			break;
		case '^':
			printf("triangle");

			break;
		case 'z':
			printf("sawtooth");
			
			break;
		case 'n':
			printf("softsquare");

			break;
		default:
			printf("something went wrong...");
			break;


	}
}
