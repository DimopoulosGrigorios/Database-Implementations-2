#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "CS.h"

#define NUM_OF_RECORDS_IN_FILE 300		/* to plithos twn eggrafwn tou arxeiou records.txt */

int main(void) 
{
	int infoFile;
	Record record;
	char *fieldNames[] = {"id","name","surname","status","dateOfBirth","salary","section","daysOff","prevYears"};
	char header_info[] = "CSFile_info";
	
	/* metritis twn eggrafwn tou arxeiou record.txt */
	int i = 0;
	
	/* Create column-store files */
	if(CS_CreateFiles(fieldNames) < 0) 
	{
		fprintf(stderr, "Error creating column-store files.\n");
		exit(EXIT_FAILURE);
	}
	
	/* Open header_info file */
	if ((infoFile = CS_OpenFile(header_info)) < 0) 
	{ 
		fprintf(stderr, "Error opening header_info file.\n");
		exit(EXIT_FAILURE);
	}

	while (!feof(stdin)) 
	{ /* read line, until eof */

		scanf("%d %s %s %c %s %d %c %d %d", &record.id, record.name, record.surname, record.status, record.dateOfBirth, &record.salary, record.section, &record.daysOff, &record.prevYears);
				
		/* Insert record in cs files */
		if (CS_InsertEntry(infoFile, record) < 0) 
		{ 
			fprintf(stderr, "Error inserting entry in cs files.\n");
			CS_CloseFile(infoFile);
			exit(EXIT_FAILURE);
		}
		i++;
		/* gia na min mpainei i teleutaia eggrafi 2 fores */
		if (i == NUM_OF_RECORDS_IN_FILE)
			break;
	}
	
	/* **Print blocks with CS_GetAllEntries() */
	 char fieldName[] = "status";
	 char *printFields[] = {"id","name","surname","status","dateOfBirth","salary","section","daysOff","prevYears"};
	 /* an theloume na psaksoume gia akeraio pedio stin CS_GetAllEntries */
	 int num = 17;
	 /* an theloume na psaksoume gia pedio-xaraktira stin CS_GetAllEntries */
	 char ch = 'S';
	 //CS_GetAllEntries(infoFile, fieldName, "Papadopoulos", printFields, 9);
	 
	 //CS_GetAllEntries(infoFile, fieldName, &num, printFields, 9);
	 
	 CS_GetAllEntries(infoFile, fieldName, &ch, printFields, 9);
	 
	 //CS_GetAllEntries(infoFile, NULL, NULL, printFields, 9);
	
	/* Close cs file */
	if (CS_CloseFile(infoFile) < 0) 
	{ 
		fprintf(stderr, "Error closing header_info file.\n");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
