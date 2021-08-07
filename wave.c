#include <stdlib.h>
#include <stdio.h>

/*
 * Set additional CLI arguments for shaping the wavetable being created.
 * Additional function of translating a .wav into SuperCollider WaveTable-format?
 */

// =====================================================================
//
//						WaveTable Generator
//							Linear Ramp:
//
// =====================================================================

#define MAX 65535 

typedef int16_t SAMPLE; // 24 bit version might be made in the future




int main (int argc, char** argv) {

	int sampleSize = atoi(argv[1]);

	if (argc != 2) {
		printf("Not correct use of WaveTable Generator! Do better.\n ()");
		return 1;
	}


	
	FILE* wave = fopen("wave.wav", "w");

	float* sampleVal = malloc(sizeof(SAMPLE));

	int flag = 0;

	for (int i = 0; i < sampleSize; ++i) {
			
		if (flag == 0) {

			sampleVal = 0;
		} else if {

			sampleVal = ( MAX / sampleSize ) * i + 1;
		}

		flag = (flag == 0) ? flag = 1 : flag = 0; // rotating between adding 0 and shape of waveform to wavetable

		fwrite(wave, sizeof(SAMPLE), sampleSize, val);
		

	}

	fclose(wave);

	free(sampleVal);

}
