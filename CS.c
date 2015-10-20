#include "CS.h"
//======================================================================================================================================================//

void print_record(Record record)
{
	if (record.id != -1)
		printf("ID: \t\t %d \n", record.id);
	if (record.name[0] != '\0')
		printf("Name: \t\t %s \n",record.name);
	if (record.surname[0] != '\0')
		printf("Surname: \t %s \n", record.surname);
	if (record.status[0] != '\0')
		printf("Status: \t %c \n", record.status[0]);
	if (record.dateOfBirth[0] != '\0')
		printf("Birthday: \t %s \n", record.dateOfBirth);
	if (record.salary != -1)
		printf("Salary: \t %d \n", record.salary);
	if (record.section[0] != '\0')
		printf("Section: \t %c \n", record.section[0]);
	if (record.daysOff != -1)
		printf("DaysOff: \t %d \n", record.daysOff);
	if (record.prevYears != -1)
		printf("PrevYears: \t %d \n", record.prevYears);
	printf("\n");
}

//====================================================================================================================================================//

int CS_CreateFiles(char** fileNames)
{
	int i, fileDesc, data, reccount, temp;
	Header_Info info;
	void *block;
	char pr[] = "CSFile_";						/*prothema twn arxeiwn CS*/
	char *str;
	BF_Init();		
	if (BF_CreateFile("CSFile_info") < 0)		/*dimiourgia anagnwristikou arxeiou*/
	{
		BF_PrintError("Error creating the file");
	    return -1;
	}
	fileDesc = BF_OpenFile("CSFile_info");		/*anoigma tou arxeiou*/
	if (fileDesc < 0)							/*se periptwsi apotixias*/
	{
		BF_PrintError("Error opening file");
		return -1;
	}
	if (BF_AllocateBlock(fileDesc) < 0)		/*desmeusi block sto arxeio*/
	{
		BF_PrintError("Error allocating a block");
		return -1;
	}
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
	{
		BF_PrintError("Error reading first block of file");
		return -1;
	}
	info.fileDesc = -1;
	for (i = 0; i < 9; i++)
	{
		str = malloc((strlen(pr) + strlen(fileNames[i]) + 1) * sizeof(char));
		strcpy(str, pr);
		strcpy(str + strlen(pr), fileNames[i]);
		strcpy(info.fieldName, str);	/*grafw to header info gia kathe arxeio*/
		memcpy(block + i*sizeof(Header_Info), &info, sizeof(Header_Info));
		free(str);
	}
	if (BF_WriteBlock(fileDesc, 0) < 0)
	{
		BF_PrintError("Error writing first block of file");
		return -1;
	}
	if (BF_CloseFile(fileDesc) < 0)
	{
		BF_PrintError("Error closing file");
		return -1;
	}
	for (i = 0; i < 9; i++)							/*gia ta 9 arxeia column store*/
	{
		str = malloc((strlen(pr) + strlen(fileNames[i]) + 1) * sizeof(char));
		strcpy(str, pr);
		strcpy(str + strlen(pr), fileNames[i]);
		if (BF_CreateFile(str) < 0)		/*dimiourgia column store arxeiou*/
		{
			BF_PrintError("Error creating the file");
			return -1;
		}
		fileDesc = BF_OpenFile(str);		/*anoigma tou arxeiou*/
		if (fileDesc < 0)							/*se periptwsi apotixias*/
		{
			BF_PrintError("Error opening file");
			return -1;
		}
		if (BF_AllocateBlock(fileDesc) < 0)		/*desmeusi block sto arxeio*/
		{
			BF_PrintError("Error allocating a block");
			return -1;
		}
		if (BF_ReadBlock(fileDesc, 0, &block) < 0)
		{
			BF_PrintError("Error reading block of column store file");
			return -1;
		}
		if (i == 1)							/*periptwsi gia name*/
			temp = 15;
		else if (i == 2)					/*periptwsi gia surname*/
			temp = 20;
		else if (i == 3 || i == 6)			/*periptwsi gia section h status*/
			temp = 1;
		else if (i == 4)					/*periptwsi gia hmerominia*/
			temp = 11;
		else
			temp = 4;						/*periptwsi gia akeraio pedio*/
		reccount = (512 - 4) / (4 + temp);
		data = 0;							/*0 gia arxeio column store swrou*/
		memcpy(block, &data, 4);
		data = i + 1;						/*auksontas arithmos pediou*/
		memcpy(block + 4, &data, 4);
		memcpy(block + 8, &reccount, 4);	/*megisto plithos eggrafwn arxeiou*/
		memcpy(block + 12, &temp, 4);		/*megethos pediou*/
		data = -1;
		memcpy(block + 16, &data, 4);		/*rowid tis teleutaias eggragis tou arxeiou*/
		if (BF_WriteBlock(fileDesc, 0) < 0)
		{
			BF_PrintError("Error writing block of column store file");
			return -1;
		}
		if (BF_CloseFile(fileDesc) < 0)
		{
			BF_PrintError("Error closing file");
			return -1;
		}
		free(str);
	}
	return 0;
}

/*====================================================================================================================================================*/

int CS_OpenFile(char* header_info)
{
	int Desc,i,tempDesc,error=0,heapflag,clmn;       /*Desc=anagnwristiko anoigmatos arxeiou,tempDesc=anagnwristhko anoigmatos arxeiwn id,name... k.o.k*/
        void* block,* tempblock;           /*clmn=arithmos pediou pou apothikeuetai se kathe file.p.x 1=id,2=name,3=surname ...k.o.k */
        Header_Info hi;				
	Desc=BF_OpenFile(header_info);
        if      (Desc < 0)
        {
                BF_PrintError("Error opening info file");
                return -1;
        }
        if (BF_ReadBlock(Desc, 0, &block) < 0)
        {
                BF_PrintError("Error in readind  first block of file");
                return -1;
        }
	for(i=0;i<=8;i++)
	{
        	memcpy(&hi, block+i*sizeof(Header_Info), sizeof(Header_Info));		/*diabase tis eggrafes tou anagnristikou arxeiou */  	
		if (!strcmp((hi.fieldName),"CSFile_id"))
		{
			tempDesc=BF_OpenFile("CSFile_id");
			if      (tempDesc < 0)
        		{
                		BF_PrintError("Error opening file");
                		return -1;
        		}
			if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
        		{
                		BF_PrintError("Error in readind  first block of file");
                		return -1;
        		}
			memcpy(&heapflag, tempblock,4);				/*des an einai typou heap*/
			memcpy(&clmn,tempblock+4,4);				/*des an einai to swsto arxeio gia to swsto pedio*/
			if(heapflag)
			{
				BF_PrintError("Error not a Heap file");
                 		return -1;
			}
			if (clmn!=1)
			{
				BF_PrintError("Error not a Heap id-file");
        		        tempDesc=-1;
				error=1;
	
			}
			hi.fileDesc=tempDesc;						/*perna sto anagnwristiko arxeio to fileDesc tou arxeiou pediou*/
			memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
			BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
      			{
                		BF_PrintError("Error in readind  first block of file");
                		return -1;
        		}
		}
		else if (!strcmp((hi.fieldName),"CSFile_name"))			/*kane ta idia me parapanw gia kathe periptwsh arxeio-pediou sto anagnwristiko arxeio */
		{
			tempDesc=BF_OpenFile("CSFile_name");
			if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
                        if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=2)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
		}
		else if (!strcmp((hi.fieldName),"CSFile_surname"))
        	{
			tempDesc=BF_OpenFile("CSFile_surname");
			if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
                        if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=3)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if (!strcmp((hi.fieldName),"CSFile_status"))
        	{
			tempDesc=BF_OpenFile("CSFile_status");
			if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
                        if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=4)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if (!strcmp((hi.fieldName),"CSFile_dateOfBirth"))
        	{
			tempDesc=BF_OpenFile("CSFile_dateOfBirth");
                        if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
			if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=5)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if	(!strcmp((hi.fieldName),"CSFile_salary"))
        	{
			tempDesc=BF_OpenFile("CSFile_salary");
                        if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
			if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=6)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if (!strcmp((hi.fieldName),"CSFile_section"))
        	{	
			tempDesc=BF_OpenFile("CSFile_section");
			if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
                        if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=7)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if (!strcmp((hi.fieldName),"CSFile_daysOff"))
        	{
			tempDesc=BF_OpenFile("CSFile_daysOff");
			if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
                   	if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=8)
                        {

                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
		else if (!strcmp((hi.fieldName),"CSFile_prevYears"))
        	{
			tempDesc=BF_OpenFile("CSFile_prevYears");
                        if      (tempDesc < 0)
                        {
                                BF_PrintError("Error opening file");
                                return -1;
                        }
			if (BF_ReadBlock(tempDesc, 0, &tempblock) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
                        memcpy(&heapflag, tempblock,4);
                        memcpy(&clmn,tempblock+4,4);
                        if(heapflag)
                        {
                                BF_PrintError("Error not a Heap file");
                                return -1;
                        }
                        if (clmn!=9)
                        {
                                BF_PrintError("Error not a Heap id-file");
                                tempDesc=-1;
                                error=1;

                        }
                        hi.fileDesc=tempDesc;
                        memcpy(block+i*sizeof(Header_Info),&hi, sizeof(Header_Info));
                        BF_WriteBlock(Desc,0);
			if (BF_ReadBlock(Desc, 0, &block) < 0)
                        {
                                BF_PrintError("Error in readind  first block of file");
                                return -1;
                        }
        	}
	}
	if (error==1)			/*an yphrkse error epestrepse -1*/
	{
		printf("An error had occured \n");
		return -1;
	}
	return Desc;		/*epestrepese to fileDesc tou anagnwristikou arxeiou*/
}

/*===================================================================================================================================================*/

int CS_CloseFile(int fileDesc)
{
	void* block;
	Header_Info hi;
	int i;
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
        {
        	BF_PrintError("Error in readind  first block of file");
                return -1;
        }
	for(i=0;i<=8;i++)
	{
		memcpy(&hi,block+i*sizeof(Header_Info),sizeof(Header_Info));
		if(BF_CloseFile(hi.fileDesc)<0)
		{
			BF_PrintError("Error closing file");
			return -1;
		}
	}
	if(BF_CloseFile(fileDesc)<0)
	{
		BF_PrintError("Error closing file");
		return -1;
	}
	return 0;
}

/*==================================================================================================================================================*/

int CS_InsertEntry (int fileDesc,Record record)
{
	void* block;
	Header_Info hi;
	int i, Descs[9];
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
	{								/*diavazw to block tou anagnwristikou arxeiou*/
		BF_PrintError("Error in readind  first block of file");
		return -1;
	}
	for(i = 0; i <= 8; i++)
	{								/*antigrafw sta filedescs twn arxeiwn ston pinaka Descs*/
		memcpy(&hi, block + i * sizeof(Header_Info), sizeof(Header_Info));
		Descs[i] = hi.fileDesc;
	}
	for(i = 0; i <= 8; i++)
	{								/*gia kathe arxeio*/
		if (access(Descs[i], &record, 0, 1) < 0)	/*kanw eisagwgi to antistoixo pedio*/
			printf("An error occured at insert function \n");
	}
	return 0;	
}

/*=================================================================================================================================================*/

void CS_GetAllEntries(int fileDesc, char* fieldName, void* value, char** fieldNames, int n)
{
	Header_Info info;
	unsigned int block_counter = 1, record_counter = 0, bytes_to_compare;
	int reccount, i, j, k, flag = 0, size, c_id, found = 0;
	int print_field[9], pinakas[500];
	void* block;
	void* reg;
	Record temp;
	for (i = 0; i < 9; i++)
		print_field[i] = -1;
	/*------ARXIKOPOIHSH TOY RECORD------*/
	temp.id = -1;
	temp.name[0] = '\0';
	temp.surname[0] = '\0';
	temp.status[0] = '\0';
	temp.dateOfBirth[0] = '\0';
	temp.salary = -1;
	temp.section[0] = '\0';
	temp.daysOff = -1;
	temp.prevYears = -1;
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
	{											/*diavazw to block tou anagnwristikou arxeiou*/
		BF_PrintError("Error in readind first block of file");
		return;
	}
	if (n <= 0 || n >= 10)
	{
		printf("In Fuction GetAllEntries: Wrong values given.\n");
		return;
	}
	for (i = 0; i < n; i++)
	{											/*filaw ston pinaka print_field*/
		if (strcmp(fieldNames[i], "id") == 0)	/*ta filedescs twn arxeiwn-pediwn*/
		{										/*pou prepei na ektipwsw*/
			memcpy(&info, block, sizeof(Header_Info));
			print_field[0] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "name") == 0)
		{
			memcpy(&info, block + 1 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[1] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "surname") == 0)
		{
			memcpy(&info, block + 2 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[2] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "status") == 0)
		{
			memcpy(&info, block + 3 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[3] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "dateOfBirth") == 0)
		{
			memcpy(&info, block + 4 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[4] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "salary") == 0)
		{
			memcpy(&info, block + 5 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[5] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "section") == 0)
		{
			memcpy(&info, block + 6 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[6] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "daysOff") == 0)
		{
			memcpy(&info, block + 7 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[7] = info.fileDesc;
		}
		else if (strcmp(fieldNames[i], "prevYears") == 0)
		{
			memcpy(&info, block + 8 * sizeof(Header_Info), sizeof(Header_Info));
			print_field[8] = info.fileDesc;
		}
		else
		{
			printf("In Fuction GetAllEntries: Wrong values given.\n");
			return;
		}
	}
	if (fieldName == NULL && value == NULL)	
	{													/*periptwsi pou ektipwnontai oles oi eggrafes*/
		memcpy(&info, block, sizeof(Header_Info));		/*dialegw to prwto arxeio me ta id*/
		if(BF_ReadBlock(info.fileDesc, 0, &block) < 0)
		{												/*diavazw to prwto block*/
			BF_PrintError("Error reading a block");
			return;
		}
		block_counter++;
		memcpy(&flag, block + 16, 4);					/*pairnw to rowid tis teleutaias eggrafis*/
		for (j = 0; j <= flag; j++)						/*kai gemizw ton pinaka pinakas me arithmous seiriaka*/
			pinakas[j] = j;								/*apo to 0 mexri to rowid tis teleutaias eggrafis*/
		flag = j;										/*filaw sto flag to teleutaio rowid*/
	}												
	else if (fieldName != NULL && value != NULL)	
	{													/*periptwsi pou kanw anazitisi*/
		if (strcmp(fieldName, "id") == 0)
		{
			memcpy(&info, block, sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "name") == 0)
		{
			memcpy(&info, block + 1 * sizeof(Header_Info), sizeof(Header_Info));
			flag = 1;
		}
		else if (strcmp(fieldName, "surname") == 0)
		{
			memcpy(&info, block + 2 * sizeof(Header_Info), sizeof(Header_Info));
			flag = 1;
		}
		else if (strcmp(fieldName, "status") == 0)
		{
			memcpy(&info, block + 3 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "dateOfBirth") == 0)
		{
			memcpy(&info, block + 4 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "salary") == 0)
		{
			memcpy(&info, block + 5 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "section") == 0)
		{
			memcpy(&info, block + 6 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "daysOff") == 0)
		{
			memcpy(&info, block + 7 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else if (strcmp(fieldName, "prevYears") == 0)
		{
			memcpy(&info, block + 8 * sizeof(Header_Info), sizeof(Header_Info));
		}
		else		/*sta parapanw if pernw sto info to filedesc tou arxeiou pou tha prepei na psaksw*/
		{
			printf("In Fuction GetAllEntries: Wrong values given.\n");
			return;
		}
		if(BF_ReadBlock(info.fileDesc, 0, &block) < 0)
		{
			BF_PrintError("Error reading a block");
			return;
		}												/*anoigw to arxeio sto opoio tha psaksw*/
		memcpy(&size, block + 12, 4);					/*pairnw to megethos twn eggrafwn tou arxeiou autou*/
		if (flag == 1)									/*an prokeitai gia arxeio pou exei mesa strings*/
			bytes_to_compare = strlen(value) + 1;		/*filaw ta bytes ta opoia tha sigrinw*/
		else
			bytes_to_compare = size;
		k = 0;
		for (i = 1; i <= BF_GetBlockCounter(info.fileDesc) - 1; i++)
		{												/*gia ola ta blocks tou arxeiou anazitisis*/
			block_counter++;
			if(BF_ReadBlock(info.fileDesc, i, &block) < 0)
			{											/*ta diavazw*/
				BF_PrintError("Error reading a block");
				return;
			}
			memcpy(&reccount, block, 4);				/*pairnw to plithos twn eggrafwn tous*/
			for (j = 0; j < reccount; j++)
			{											/*kai gia kathe eggrafi, pairnw to rowid tis*/
				memcpy(&c_id, block + 4 + (4 + size) * j, 4);
				reg = malloc(size);						/*desmeuw ton katallilo xwro sto rec*/
				if (reg == NULL)
				{
					printf("In Fuction GetAllEntries: Cannot Allocate Memory!\n");
					return;
				}
				memcpy(reg, block + 4 + 4 + (size + 4) * j, size);	/*kai antigrafw se auto tin eggrafi*/
				if (memcmp(reg, value, bytes_to_compare) == 0)
				{													/*an einai isi me autin pou psaxnw*/
					pinakas[k] = c_id;								/*filaw ston pinaka pinakas to rwid tis*/
					k++;
					found = 1;										/*filaw 1 sto found gia na kserw*/
				}													/*oti vrika toulaxiston 1 eggrafi stin anazitisi*/
				free(reg);											/*kai kanw free to rec*/
			}
		}
		flag = k;										/*filaw sto flag to plithos twn stoixeiwn tou pinaka pinakas*/
		if (found == 0)
			printf("No matched records in file: 'CSFile_%s' with such key.\n", fieldName); 
	}
	else
	{													/*an ta orismata einai lathos, ektipwnw error*/
		printf("In Fuction GetAllEntries: Wrong values given\n");
		return;
	}
	for (j = 0; j < flag; j++)
	{													/*gia ola ta rowid tou pinaka pinakas*/
		for (k = 0; k < 9; k++)
		{
			if (print_field[k] != -1)
			{											/*an prepei na ektipwsw to pedio*/
				i = access(print_field[k], &temp, pinakas[j], 0);	/*kalw tin access*/
				if (i < 0)								/*kai grafw to antistoixo pedio tou record*/
					return;
				block_counter += i;						/*prosthetw ta block pou diavastikan*/
			}											/*kai afou grapsw sto record*/
		}												/*osa pedia mou zitithikan*/
		print_record(temp);								/*to ektipwnw*/
		temp.id = -1;								/*kai to epanarxikopoiw*/
		temp.name[0] = '\0';
		temp.surname[0] = '\0';
		temp.status[0] = '\0';
		temp.dateOfBirth[0] = '\0';
		temp.salary = -1;
		temp.section[0] = '\0';
		temp.daysOff = -1;
		temp.prevYears = -1;
		record_counter++;
	}
	printf("%d blocks read - ", block_counter);
	printf("%d records printed.\n", record_counter);
}

/*=================================================================================================================================================*/

int access(int fileDesc, Record *record, int c_id, int use)
{
	int i, j, flag, max_reccount, size, reccount, num, temp, block_counter = 0;
	char str[20], ch;
	void* block;
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
    {
		BF_PrintError("Error in reading first block of file");
		return -1;
	}
	block_counter++;
	memcpy(&flag, block + 4 , 4);		/*pairnw ton arithmo tou pediou*/
	memcpy(&max_reccount, block + 8, 4);	/*pairnw to megisto plithos eggrafwn*/
	memcpy(&size, block + 12, 4);		/*pairnw to megethos tou pediou*/
	memcpy(&i, block + 16, 4);			/*pairnw to rowid tis teleutaias eggrafis*/
	if (use == 0)						/*periptwsi klisis apo get*/
	{
			if (BF_ReadBlock(fileDesc, c_id / max_reccount + 1, &block) < 0)
			{							/*diavazw to block pou periexei tin eggrafi me rowid = c_id*/
				BF_PrintError("Error in readind block of file");
				return -1;
			}
			block_counter++;
			memcpy(&reccount, block, 4);/*pairnw to plithos twn eggrafwn*/
			for (j = 0; j < reccount; j++)
			{							
				memcpy(&num, block + 4 + (4 + size) * j, 4);
				if (num == c_id)		/*pairnw to rowid*/
				{						/*kai ama einai iso me to rowid*/
					if (flag == 1)		/*pou exw parei apo tin get*/
					{					/*pernw to antistoixo pedio sto record*/
						memcpy(&temp, block + 8 + (size + 4) * j, size);
						record->id = temp;
					}
					else if (flag == 2)
					{
						memcpy(str, block + 8 + (size + 4) * j, size);
						strcpy(record->name, str);
					}
					else if (flag == 3)
					{
						memcpy(str, block + 8 + (size + 4) * j, size);
						strcpy(record->surname, str);
					}
					else if (flag == 4)
					{
						memcpy(&ch, block + 8 + (size + 4) * j, size);
						memcpy(record->status, &ch, size);
					}
					else if (flag == 5)
					{
						memcpy(str, block + 8 + (size + 4) * j, size);
						strcpy(record->dateOfBirth, str);
					}
					else if (flag == 6)
					{
						memcpy(&temp, block + 8 + (size + 4) * j, size);
						record->salary = temp;
					}
					else if (flag == 7)
					{
						memcpy(&ch, block + 8 + (size + 4) * j, size);
						memcpy(record->section, &ch, size);
					}
					else if (flag == 8)
					{
						memcpy(&temp, block + 8 + (size + 4) * j, size);
						record->daysOff = temp;
					}
					else if (flag == 9)
					{
						memcpy(&temp, block + 8 + (size + 4) * j, size);
						record->prevYears = temp;
					}
					return block_counter;			/*epistrefw to plithos twn block pou diavastikan*/
				}
			}
	}
	else if (use == 1)								/*periptwsi klisis apo insert*/
	{												/*eisagwgi eggrafis*/
		i++;										/*auksanw tin timi tou megistou rowid*/
		memcpy(block + 16, &i, 4);					/*kai tin grafw sto prwto block tou arxeiou*/			
		if (BF_WriteBlock(fileDesc, 0) < 0)
		{
			BF_PrintError("Error writing block of column store file");
			return -1;
		}
		if (BF_GetBlockCounter(fileDesc) == 1)		/*an iparxei ena block sto arxeio*/
		{
			if (BF_AllocateBlock(fileDesc) < 0)		/*desmeusi neou block sto arxeio*/
			{
				BF_PrintError("Error allocating a block");
				return -1;
			}
			if (BF_ReadBlock(fileDesc, 1, &block) < 0)
			{
				BF_PrintError("Error in readind block of file");
				return -1;
			}
			reccount = 0;							/*me plithos eggrafwn 0*/
			memcpy(block, &reccount, 4);
			if (BF_WriteBlock(fileDesc, 1) < 0)
			{
				BF_PrintError("Error writing block of column store file");
				return -1;
			}
		}
		if (BF_ReadBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1, &block) < 0)
		{											/*diavazw to teleutaio block tou arxeiou*/
			BF_PrintError("Error in readind block of file");
			return -1;
		}
		memcpy(&reccount, block, 4);				/*kai pairnw to plithos twn eggrafwn tou*/
		if (reccount == max_reccount)				/*an to block einai gemato*/
		{
			if (BF_AllocateBlock(fileDesc) < 0)		/*desmeusi neou block sto arxeio*/
			{
				BF_PrintError("Error allocating a block");
				return -1;
			}
			if (BF_ReadBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1, &block) < 0)
			{										/*diavazw to neo block*/
				BF_PrintError("Error in readind block of file");
				return -1;
			}
			reccount = 0;							/*me plithos eggrafwn 0*/
			memcpy(block, &reccount, 4);
			if (BF_WriteBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1) < 0)
			{										
				BF_PrintError("Error writing block of column store file");
				return -1;
			}
		}
		memcpy(block + 4 + (4 + size) * reccount, &i, 4);	/*grafw sto block to rowid*/
		if (flag == 1)										/*kai epeita tin eggrafi*/
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, &record->id, size);
		}
		else if (flag == 2)						
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, record->name, size);
		}
		else if (flag == 3)						
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, record->surname, size);
		}
		else if (flag == 4)						
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, record->status, size);
		}
		else if (flag == 5)									
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, record->dateOfBirth, size);
		}
		else if (flag == 6)									
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, &record->salary, size);
		}
		else if (flag == 7)									
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, record->section, size);
		}
		else if (flag == 8)									
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, &record->daysOff, size);
		}
		else if (flag == 9)									
		{
			memcpy(block + 4 + 4 + (size + 4) * reccount, &record->prevYears, size);
		}
		reccount++;										/*auksanw tis eggrafes tou block kata 1*/
		memcpy(block, &reccount, 4);					/*kai tis grafw sto block*/
		if (BF_WriteBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1) < 0)
		{
			BF_PrintError("Error writing block of column store file");
			return -1;
		}
	}
	return 0;
}

/*===============================================================================================================================================*/
