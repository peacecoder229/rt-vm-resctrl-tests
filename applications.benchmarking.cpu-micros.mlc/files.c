#include "files.h"

//
// Check numa balancing settings

/*

Copyright (c) 2014, Intel Corporation
All rights reserved.

 */
#include <stdio.h>

FILE* numafp;

int check_file_exists(char file_path[], char mode[])
{
	if ((numafp = fopen(file_path, mode)) == NULL) {
		//fprintf(stderr, "File does not exist\n");
		return 0;
	} else {
		//fprintf(stderr, "File exists, openeable with mode %s\n", mode);
		fclose(numafp);
		return 1;
	}
}


int read_int_from_file(char file_path[], int offset)
{
	int file_value=0;

	if ((numafp = fopen(file_path, "r")) == NULL) {
		//fprintf(stderr, "File does not exist\n");
		return -1;
	} else {
		if (fseek(numafp, offset, SEEK_SET) != -1) {
			if (fscanf(numafp, "%d", &file_value) == EOF) {
				fclose(numafp);
				return -1;
			}
		}
		fclose(numafp);
		//printf("value is %d\n", file_value);
		return file_value;
	}
}

int write_int_to_file(char file_path[], int offset, int value)
{
	if ((numafp = fopen(file_path, "w")) == NULL) {
		//fprintf(stderr, "File does not exist\n");
		return -1;
	} else {
		if (fseek(numafp, offset, SEEK_SET) != -1)
			fprintf(numafp, "%d", value);
		fclose(numafp);
		return value;
	}
}

