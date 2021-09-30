#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <err.h>

// TODO: check on fopen function.
// TODO: solve infinite loop

struct WAVEHEADER {
	int riffID; //  0x46464952 'FFIR' ('RIFF')
	int headerSize;
	int filetypeID; // 0x45564157 'EVAW' ('WAVE')
} __attribute__((packed)) ;

struct B_EXTENSION {
	int bextID; // 0x74786562 'txeb' ('bext')
	int bextSize;
};

struct FORMAT {
	int formatID; // 0x20746d66 'tmf' ('fmt')
	int formatSize;
	short audioFormat;
	short numChan;
	int smplRate;
	int byteRate;
	short blockAlign;
	short bps; // bits per sample
};

struct DATA {
	int dataID; // 0x61746164 = 'atad' ('data')
	int dataSize;
}; 

typedef int32_t SAMPLE16;

union SAMPLE24 {
	char byte[3];
	int32_t sample;
};



int main(int argc, char** argv) {
	struct WAVEHEADER mainHeader;
	struct FORMAT fmtHeader;
	struct B_EXTENSION bextHeader;
	struct DATA dataHeader;
	int cursor;
	printf("here");

	if (argc != 2) {
		printf("wrong number of arguments\n");
		return 1;
	}
	

	char path[40];
	sprintf(path, "./%s.wav", argv[1]); // file opened must be in same directory as c program
	printf("%s\n", path);
	
	FILE* wave = fopen(path, "r");
	if(wave == NULL) {
		printf("unable to open file\n");
		return 3;
	} else {
		printf("opened file successfully\n");
	};
	// fread(&mainHeader, sizeof(struct WAVEHEADER), 1, wave);
	fread(&cursor, 4, 1, wave);
	if (cursor == 0x46464952) {
		printf("true\n");
		mainHeader.riffID = cursor;
		fread(&mainHeader.headerSize, 4, 1, wave);
		fread(&mainHeader.filetypeID, 4, 1, wave);
		printf(
				"RIFF ID:\n%x\n"
				"Size of file after this and previous block ( 8 bytes ):\n%i\n"
				"Filetype ( WAVE ):\n%x\n", mainHeader.riffID, mainHeader.headerSize, mainHeader.filetypeID);

	} else {
		printf("false\n");
	}
	int flag = 0;
	int *bextChunk = 0; // ptr to store bext chunk
	while( !flag ) {
		// loop until 'data' chunk is found
		fread(&cursor, 4, 1, wave);
			//printf("in loop");
			//printf("%x", cursor);

		if ( cursor == 0x20746d66 ) {
			// if fmt
			printf("fmt\n\n");
			fmtHeader.formatID = cursor;
			fread(&fmtHeader.formatSize, 4, 1, wave);
			printf("Chunk size: %i\n",fmtHeader.formatSize);
			fread(&fmtHeader.audioFormat, 2, 1, wave);
			printf("Audio format: %i\n", fmtHeader.audioFormat);
			fread(&fmtHeader.numChan, 2, 1, wave);
			printf("Number of channels: %i\n", fmtHeader.numChan);
			fread(&fmtHeader.smplRate, 4, 1, wave);
			printf("Samplerate: %i\n", fmtHeader.smplRate);
			fread(&fmtHeader.byteRate, 4, 1, wave);
			printf("Byterate: %i\n", fmtHeader.byteRate);
			fread(&fmtHeader.blockAlign, 2, 1, wave);
			printf("Block align: %i\n", fmtHeader.blockAlign);
			fread(&fmtHeader.bps, 2, 1, wave);
			printf("Bits per sample: %i\n", fmtHeader.bps);

		} else if ( cursor == 0x74786562 ) { // have to test if it copies properly, otherwise scrub
			// if bext
			printf("\nbext\n\n");
			bextHeader.bextID = cursor;
			fread(&bextHeader.bextSize, 4, 1, wave);
			bextChunk = malloc(sizeof(char) * bextHeader.bextSize);
			printf("%i\n", bextHeader.bextSize);
			printf("modulo: %i\n", bextHeader.bextSize % 4);
			fread(bextChunk, bextHeader.bextSize, 1, wave);
			for(int i = 0, n = bextHeader.bextSize; i < n; ++i) {
				if (bextChunk[i] == 0x00) {
					printf(" ");
				} else {
					printf("%c", bextChunk[i]);
				}
				//printf("%i\n", i);
				printf("%c", bextChunk[i]);
			}
			printf("%i", bextChunk[67]);
			if (bextHeader.bextSize % 4 != 0){
				fseek(wave, bextHeader.bextSize % 4, SEEK_CUR);
			}
			printf("\n");
			

		} else if ( cursor == 0x61746164 ) {
			// if data
			printf("\ndata\n\n");
			fread(&dataHeader.dataSize, 4, 1, wave);
			printf("%i\n", dataHeader.dataSize);


			flag = 1;
		}
	} 


	fclose(wave);

	/*
		fread(&mainHeader, sizeof(struct WAVEHEADER), 1, wave);

	int headID;

	while(fread(&headID, 4, 1, wave) != 0x61746164) {

		if (headID == 0x20746d66) { // check for filetypeID chunk
			fmtHeader.formatID = headID;
			fread(&fmtHeader.formatSize, 4, 1, wave);

			if (fmtHeader.formatSize != 16) {
				// handle edge case if filetypeID chunk is longer than expected
				printf("filetypeID not understood");
				return 3;

			} else {
				fread(&fmtHeader.audioFormat, sizeof(short), 1, wave);
				fread(&fmtHeader.numChan, sizeof(short), 1, wave);
				if (fmtHeader.audioFormat != 1) {
					// handle edge case if audioFormat not PCM
					return 3;

				} 
				fread(&fmtHeader.smplRate, sizeof(int16_t), 1, wave);
				fread(&fmtHeader.byteRate, sizeof(int16_t), 1, wave);
				fread(&fmtHeader.blockAlign, sizeof(short), 1, wave);
				fread(&fmtHeader.bps, sizeof(short), 1, wave);

			}

		} else if (headID == 0x74786562) { // check for bext chunk
			bextHeader.bextID = headID;
			fread(&bextHeader.bextSize, sizeof(int16_t), 1, wave);

			int* bextChunk = malloc(sizeof(bextHeader.bextSize / 8));
			fread(bextChunk, sizeof( bextHeader.bextSize / 8 ), 1, wave);

		}

	} 
	dataHeader.dataID = headID; // read data chunk
	fread(&dataHeader.dataSize, sizeof(int16_t), 1, wave);
	int32_t* data = malloc(dataHeader.dataSize / 8);
	fread(data, sizeof(data), 1, wave);

	// Print specs from header
	printf("-- %c\n-- %i\n-- %c\n--------", mainHeader.riffID, mainHeader.chunkSize, mainHeader.filetypeID);
	printf("-- %c\n-- %i\n-- %i\n-- %i\n-- %i\n-- %i\n-- %i\n-- %i\n--------", 
			fmtHeader.formatID, fmtHeader.formatSize, fmtHeader.audioFormat, fmtHeader.numChan, 
			fmtHeader.smplRate, fmtHeader.byteRate, fmtHeader.blockAlign, fmtHeader.bps);
	if (bextHeader.bextID != 0) {
		printf("-- %c\n-- %i\n--------", bextHeader.bextID, bextHeader.bextSize);
	};
	printf("-- %c\n-- %i\n \n", dataHeader.dataID, dataHeader.dataSize);

	fclose(wave);
	*/
	return 0;


	// Do sample manipulation

	/* if (fmtHeader.bps == 16) { */
	/* 	// use 16 bit depth */
	/* } else if (fmtHeader.bps == 24) { */
	/* 	// use 24 bit depth */
	/* } else if (fmtHeader.bps > 24 || fmtHeader.bps < 16) { */
	/* 	printf("not compatible bit depth"); */
	/* 	return 3; */
	/* } */

	// Write header and new data	
	
	/* char newPath[40]; */
	/* sprintf(newPath, "%s_wavetable.wav", path); */
	/* FILE* dest = fopen(newPath, "w"); */
	/* fwrite(&mainHeader, sizeof(mainHeader), 1, dest); */
	/* fwrite(&fmtHeader, sizeof(fmtHeader), 1, dest); */
	/* if (bextHeader.bextID != 0) { */
	/* 	fwrite(&bextHeader, sizeof(bextHeader), 1, dest); */
	/* } */
	/* fwrite(&dataHeader, sizeof(dataHeader), 1, dest); */

	// free mallocs and close files.
	
	

};
