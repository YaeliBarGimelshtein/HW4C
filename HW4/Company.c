#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "Company.h"
#include "Airport.h"
#include "General.h"
#include "fileHelper.h"

static const char* sortOptStr[eNofSortOpt] = {
	"None","Hour", "Date", "Airport takeoff code", "Airport landing code" };


int	initCompanyFromFile(Company* pComp, AirportManager* pManaer, const char* fileName)
{
#ifdef COMPRESSION
	FILE* bFile = fopen(fileName,"rb");
	if (!bFile)
		return 0;
	unsigned char buffer[10];
	fread(buffer, sizeof(char), 2, bFile);
	pComp->flightCount = (buffer[0]<<1) | (buffer[1] >> 7);
	printf("%d\n", pComp->flightCount);
	pComp->sortOpt = (buffer[1] >>4) & 0x7;
	printf("%d\n", pComp->sortOpt);
	int nameLen = buffer[1] & 0xF;
	printf("%d\n", nameLen);
	pComp->name = (char*)malloc(nameLen);
	if (!pComp->name)
		return 0;
	fread(pComp->name, sizeof(char), nameLen, bFile);
	printf("%s\n", pComp->name);
	pComp->flightArr = (Flight**)malloc(pComp->flightCount * sizeof(Flight*));
	if (!pComp->flightArr)
		return 0;
	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		if (!pComp->flightArr[i])
			return 0;
		fread(buffer, sizeof(char), 10, bFile);
		for (int j = 0; j < CODE_LENGTH; j++)
		{
			pComp->flightArr[i]->originCode[j] = buffer[j];
			pComp->flightArr[i]->destCode[j] = buffer[j+3];
		}
		printf("%s\n", pComp->flightArr[i]->originCode);
		printf("%s\n", pComp->flightArr[i]->destCode);
		pComp->flightArr[i]->date.year = (buffer[6]) << 10;
		pComp->flightArr[i]->date.year = pComp->flightArr[i]->date.year | (buffer[7]) << 2;
		pComp->flightArr[i]->date.year = pComp->flightArr[i]->date.year | (buffer[8] >> 6);
		printf("%d\n", pComp->flightArr[i]->date.year);
		pComp->flightArr[i]->date.month = (buffer[8] >> 2) & 0xF;
		printf("%d\n", pComp->flightArr[i]->date.month);
		pComp->flightArr[i]->date.day = (buffer[8] & 0x3)<<3 | (buffer[9] >> 5);
		printf("%d\n", pComp->flightArr[i]->date.day);
		pComp->flightArr[i]->hour = buffer[9] & 0x1F;
		printf("%d\n", pComp->flightArr[i]->hour);

	}


	return 1;
#else
	L_init(&pComp->flighDateList);
	if (loadCompanyFromFile(pComp, pManaer, fileName))
	{
		initDateList(pComp);
		return 1;
	}
#endif // COMPRESSION
	return 0;
}

void	initCompany(Company* pComp, AirportManager* pManaer)
{
	printf("-----------  Init Airline Company\n");
	L_init(&pComp->flighDateList);

	pComp->name = getStrExactName("Enter company name");
	pComp->flightArr = NULL;
	pComp->flightCount = 0;
}

void	initDateList(Company* pComp)
{
	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (isUniqueDate(pComp, i))
		{
			char* sDate = createDateString(&pComp->flightArr[i]->date);
			L_insert(&(pComp->flighDateList.head), sDate);
		}
	}
}

int		isUniqueDate(const Company* pComp, int index)
{
	Date* pCheck = &pComp->flightArr[index]->date;
	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (i == index)
			continue;
		if (equalDate(&pComp->flightArr[i]->date, pCheck))
			return 0;
	}
	return 1;
}

int		addFlight(Company* pComp, const AirportManager* pManager)
{

	if (pManager->count < 2)
	{
		printf("There are not enoght airport to set a flight\n");
		return 0;
	}
	pComp->flightArr = (Flight**)realloc(pComp->flightArr, (pComp->flightCount + 1) * sizeof(Flight*));
	if (!pComp->flightArr)
		return 0;
	pComp->flightArr[pComp->flightCount] = (Flight*)calloc(1, sizeof(Flight));
	if (!pComp->flightArr[pComp->flightCount])
		return 0;
	initFlight(pComp->flightArr[pComp->flightCount], pManager);
	if (isUniqueDate(pComp, pComp->flightCount))
	{
		char* sDate = createDateString(&pComp->flightArr[pComp->flightCount]->date);
		L_insert(&(pComp->flighDateList.head), sDate);
	}
	pComp->flightCount++;
	return 1;
}

void	printCompany(const Company* pComp,  ...) //check
{
	if (pComp == NULL)
	{
		printf("Error reading company");
		return;
	}

	va_list   strings;
	va_start(strings, pComp);

	/*pComp->name = (char*)realloc(pComp->name, strlen(word) + strlen(pComp->name) + 3);
	if (!pComp->name)
		return;
	strcat(pComp->name, "_");

	char* check = va_arg(strings, char*);
	strcat(pComp->name, word);

	while (check != NULL)
	{
		pComp->name = (char*)realloc(pComp->name, (strlen(pComp->name) + strlen(check) + 3));
		if (!pComp->name)
			return;
		strcat(pComp->name, "_");
		strcat(pComp->name, check);
		check = va_arg(strings, char*);
	}*/
	va_end(strings);
#ifdef DETAIL_PRINT
	printf("Company %s:\n", pComp->name);
	printf("Has %d flights\n", pComp->flightCount);
	generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), printFlightV);
	printf("\nFlight Date List:");
	L_print(&pComp->flighDateList, printStr);
#else
	printf("Company %s:\n", pComp->name);
	printf("Has %d flights\n", pComp->flightCount);
#endif
}

void	printFlightsCount(const Company* pComp)
{
	char codeOrigin[CODE_LENGTH + 1];
	char codeDestination[CODE_LENGTH + 1];

	if (pComp->flightCount == 0)
	{
		printf("No flight to search\n");
		return;
	}

	printf("Origin Airport\n");
	getAirportCode(codeOrigin);
	printf("Destination Airport\n");
	getAirportCode(codeDestination);

	int count = countFlightsInRoute(pComp->flightArr, pComp->flightCount, codeOrigin, codeDestination);
	if (count != 0)
		printf("There are %d flights ", count);
	else
		printf("There are No flights ");

	printf("from %s to %s\n", codeOrigin, codeDestination);
}

unsigned int createMask(int left, int right)
{
	int temp;
	if (left < right)
	{
		temp = left;
		left = right;
		right = temp;
	}
	return (1 << (left + 1)) - (1 << right);
}

int		saveCompanyToFile(const Company* pComp, const char* fileName)
{
#ifdef COMPRESSION
	int size = 2 + strlen(pComp->name) + 1;
	int nameSize = strlen(pComp->name) + 1;
	unsigned char* data = (unsigned char*)calloc(sizeof(unsigned char), size);
	data[0] = (pComp->flightCount >> 1);
	data[1] = (pComp->flightCount << 7) | (pComp->sortOpt << 4) | (nameSize);
	for (int i = 2; i < size; i++)
	{
		data[i] = pComp->name[i-2];
	}
	FILE* bFile=fopen(fileName,"wb");
	if (!bFile)
		return 0;
	fwrite(data, sizeof(char), size, bFile);

	data = (char*)realloc(data, 10);
	for (int i = 0; i < pComp->flightCount; i++)
	{
		for (int j = 0; j < CODE_LENGTH; j++)
			{
			data[j] = pComp->flightArr[i]->originCode[j];
			data[j+3] = pComp->flightArr[i]->destCode[j];
			}
		data[6] = pComp->flightArr[i]->date.year >> 10;
		data[7] = (pComp->flightArr[i]->date.year >> 2)&0xFF;
		data[8] = (pComp->flightArr[i]->date.year & 0x3) << 6 | (pComp->flightArr[i]->date.month << 2) | (pComp->flightArr[i]->date.day >> 3);
		data[9] = (pComp->flightArr[i]->date.day << 5) | (pComp->flightArr[i]->hour);
		fwrite(data, sizeof(char), 10, bFile);
	}
	
	free(data);
	fclose(bFile);
	/*unsigned int mask = createMask(7, 0);
	mask = mask | pComp->flightCount;
	data[size - 1] = data[size - 1] | mask;
	mask = createMask(8, 8);
	mask = mask | pComp->flightCount;
	data[size - 2] = data[size - 2] | mask;
	data<<*/
#else
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp) {
		printf("Error open copmpany file to write\n");
		return 0;
	}

	if (!writeStringToFile(pComp->name, fp, "Error write comapny name\n"))
		return 0;

	if (!writeIntToFile(pComp->flightCount, fp, "Error write flight count\n"))
		return 0;

	if (!writeIntToFile((int)pComp->sortOpt, fp, "Error write sort option\n"))
		return 0;

	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (!saveFlightToFile(pComp->flightArr[i], fp))
			return 0;
	}

	fclose(fp);
	return 1;
#endif // COMPRESSION


}

int loadCompanyFromFile(Company* pComp, const AirportManager* pManager, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	pComp->flightArr = NULL;


	pComp->name = readStringFromFile(fp, "Error reading company name\n");
	if (!pComp->name)
		return 0;

	if (!readIntFromFile(&pComp->flightCount, fp, "Error reading flight count name\n"))
		return 0;

	int opt;
	if (!readIntFromFile(&opt, fp, "Error reading sort option\n"))
		return 0;

	pComp->sortOpt = (eSortOption)opt;

	if (pComp->flightCount > 0)
	{
		pComp->flightArr = (Flight**)malloc(pComp->flightCount * sizeof(Flight*));
		if (!pComp->flightArr)
		{
			printf("Alocation error\n");
			fclose(fp);
			return 0;
		}
	}
	else
		pComp->flightArr = NULL;

	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		if (!pComp->flightArr[i])
		{
			printf("Alocation error\n");
			fclose(fp);
			return 0;
		}
		if (!loadFlightFromFile(pComp->flightArr[i], pManager, fp))
			return 0;
	}

	fclose(fp);
	return 1;
}

void	sortFlight(Company* pComp)
{
	pComp->sortOpt = showSortMenu();
	int(*compare)(const void* air1, const void* air2) = NULL;

	switch (pComp->sortOpt)
	{
	case eHour:
		compare = compareByHour;
		break;
	case eDate:
		compare = compareByDate;
		break;
	case eSorceCode:
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		compare = compareByCodeDest;
		break;

	}

	if (compare != NULL)
		qsort(pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);

}

void	findFlight(const Company* pComp)
{
	int(*compare)(const void* air1, const void* air2) = NULL;
	Flight f = { 0 };
	Flight* pFlight = &f;


	switch (pComp->sortOpt)
	{
	case eHour:
		f.hour = getFlightHour();
		compare = compareByHour;
		break;
	case eDate:
		getchar();
		getCorrectDate(&f.date);
		compare = compareByDate;
		break;
	case eSorceCode:
		getchar();
		getAirportCode(f.originCode);
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		getchar();
		getAirportCode(f.destCode);
		compare = compareByCodeDest;
		break;
	}

	if (compare != NULL)
	{
		Flight** pF = bsearch(&pFlight, pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);
		if (pF == NULL)
			printf("Flight was not found\n");
		else {
			printf("Flight found, ");
			printFlight(*pF);
		}
	}
	else {
		printf("The search cannot be performed, array not sorted\n");
	}

}

eSortOption showSortMenu()
{
	int opt;
	printf("Base on what field do you want to sort?\n");
	do {
		for (int i = 1; i < eNofSortOpt; i++)
			printf("Enter %d for %s\n", i, sortOptStr[i]);
		scanf("%d", &opt);
	} while (opt < 0 || opt >eNofSortOpt);

	return (eSortOption)opt;
}

void	freeCompany(Company* pComp)
{
	generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), freeFlight);
	free(pComp->flightArr);
	free(pComp->name);
	L_free(&pComp->flighDateList, freePtr);
}
