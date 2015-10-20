#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"

typedef struct
{ 
	int id; 
	char name[15]; 
	char surname[20];
	char status[1];
	char dateOfBirth[10];
	int salary;
	char section[1];
	int daysOff;
	int prevYears;
} Record;

typedef struct
{
	char fieldName[20];
	int fileDesc;
} Header_Info;

void print_record(Record record);

int CS_CreateFiles(char** fileNames);

int CS_OpenFile(char* header_info);

int CS_CloseFile(int fileDesc);

int CS_InsertEntry (int fileDesc,Record record);

int access(int fileDesc, Record *record, int c_id, int use);

void CS_GetAllEntries(int fileDesc, char* fieldName, void* value, char** fieldNames, int n);
