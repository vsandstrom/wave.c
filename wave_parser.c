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

	if (argc != 2) {
		printf("wrong number of arguments\n");
		return 1;
	}
	

	char path[40];
	sprintf(path, "./%s", argv[1]); // file opened must be in same directory as c program
	printf("\n%s\n", path);
	
	FILE* wave = fopen(path, "r");

	if(wave == NULL) {
		printf("unable to open file\n");
		return 3;
	} else {
		printf("opened file successfully\n");
	};
	// fread(&mainHeader, sizeof(struct WAVEHEADER), 1, wave);
	fread(&cursor, 4, 1, wave);
	if (cursor == 0x46464952) { // Check if file is RIFF

		mainHeader.riffID = cursor;
		fread(&mainHeader.headerSize, 4, 1, wave);
		fread(&mainHeader.filetypeID, 4, 1, wave);

	} else {
		printf("File format not recognized\n");
	}

	int flag = 0;
	char *bextChunk = 0; // ptr to store bext chunk

	while( flag != 1 ) {
		// loop until 'data' chunk is found
		
		fread(&cursor, 4, 1, wave);

		if ( cursor == 0x20746d66 ) {
			// if fmt
			
			fmtHeader.formatID = cursor;
			fread(&fmtHeader.formatSize, 4, 1, wave);
			fread(&fmtHeader.audioFormat, 2, 1, wave);
			fread(&fmtHeader.numChan, 2, 1, wave);
			fread(&fmtHeader.smplRate, 4, 1, wave);
			fread(&fmtHeader.byteRate, 4, 1, wave);
			fread(&fmtHeader.blockAlign, 2, 1, wave);
			fread(&fmtHeader.bps, 2, 1, wave);

		} else if ( cursor == 0x74786562 ) { 
			// if bext
			
			printf("\nbext\n\n");
			bextHeader.bextID = cursor;
			fread(&bextHeader.bextSize, 4, 1, wave);
			bextChunk = malloc(sizeof(char) * bextHeader.bextSize);
			fread(bextChunk, bextHeader.bextSize, 1, wave);

			for(int i = 0, n = bextHeader.bextSize; i < n; ++i) {
				if (bextChunk[i] == 0x00) {
					printf(" ");
				} else {
					printf("%c", bextChunk[i]);
				}
			}
			printf("\n");

		} else if ( cursor == 0x61746164 ) {
			// if data
			
			fread(&dataHeader.dataSize, 4, 1, wave);
			printf("%i\n", dataHeader.dataSize);

			flag = 1;

		} else {
			// if some junk chunk is trailing bext chunk
			
			fread(&cursor, 4, 1, wave); // read size of junk chunk
			fseek(wave, cursor, SEEK_CUR); // skip junk chunk
		}
	} 


	/* dataHeader.dataID = headID; // read data chunk
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

	free(bextChunk);
	fclose(wave);

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
