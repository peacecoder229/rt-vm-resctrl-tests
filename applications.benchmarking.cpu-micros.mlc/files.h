// (C) 2012-2014 Intel Corporation
// //
// // functions that perform file operations, to check if a file exists and write and read values to a file
//

int check_file_exists(char file_path[], char mode[]);
int read_int_from_file(char file_path[], int offset);
int write_int_to_file(char file_path[], int offset, int value);
