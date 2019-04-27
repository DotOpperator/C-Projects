 // Author: Manohar Chitoda and Suraj Upadhyay
// Asst #: Asst2
// Date: Nobvember 21, 2018

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h> 
#include <unistd.h>

//keeps track of path
typedef struct _pathdata
{
	char name [1024];
	int currentID;
	
}pathData;

typedef struct _threadData
{
	int numOfThreadsSpawned;
	int threadIDs [999];
}threadData;

//Prototypes
void startTheSearch(DIR *, pathData * );
void* checkValidity(void*);
void* checkFile(void *);
void itsDirectory(pathData * , char *);
void itsFile(char *, threadData *);
void printThreadData(threadData *);
int generateTID();

//Global Varibales
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printMetaData = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread [110000000];
int threadCount = 0;
int initialPID;
int numOfTIDS = 0;

int main(int argc, char *argv[]) 
{
	initialPID = getpid();
	
	pathData *p = (pathData *)malloc(sizeof(pathData));
	strcpy(p->name, ".");
	 p->currentID = 0;
	checkValidity(p);
	
	
	int i;
	
	for (i = 0;i < threadCount;i++)
	{
		pthread_join(thread[i], NULL);
	}

	return 0;
}

// Checks for a valid directory address
void *checkValidity(void * startHere)
{
	pathData * a = ((pathData *)startHere);
	
	DIR *dir = opendir((const char *)(a->name));
	if (dir == NULL || a == NULL) 
	{
		fprintf(stderr,"%s Directory could not be opened!\n", a->name);
		fflush(stdout);
		pthread_exit(NULL);
		free(a);
		return NULL;
	}
	
	else 
	{
		fprintf(stderr, "Directory opened: %s\n",a->name);
		fflush(stdout);
		startTheSearch(dir, a);
		free(a);
		return NULL;
	}
	return NULL;
}

// Start the search
void startTheSearch(DIR *dir, pathData * startHere)
{
	struct dirent *entry; 	//Entry variable
	threadData * threadNode = (threadData*)malloc(sizeof(threadData));
	while ((entry = readdir(dir)) != NULL)
	{
		//Skip . and ..
		if(((strcmp(entry->d_name,".") != 0) && (strcmp(entry->d_name,"..") != 0)))
		{
			switch (entry->d_type) 
			{
				case DT_REG:
				{
					pthread_mutex_lock(&mutex);
					itsFile(entry->d_name, threadNode);
					pthread_mutex_unlock(&mutex);
					break;
				}
				
				case DT_DIR:
				{
					pthread_mutex_lock(&mutex);
					itsDirectory(startHere, entry->d_name);
					pthread_mutex_unlock(&mutex);
					break;
				}
			}//end switch
		}//end if
	}// End WHile
	pthread_mutex_trylock(&printMetaData);
	printThreadData(threadNode);
	pthread_mutex_unlock(&printMetaData);
	return;
}//End startTheSearch

//Handles files
void* checkFile(void * arguement)
{
	fprintf(stderr, "\n\nFile: %s\n\n", (char *)arguement);
	fflush(stdout);
	pthread_exit(NULL);
}

//Handle threads for files
void itsFile(char *filename, threadData * fileThread)
{	
	pthread_t t1 = NULL;
	pthread_create(&t1, NULL, &checkFile, filename);
	thread[threadCount++] = t1;
	printf("\n*********Finished!!!!********\n");
	fileThread->threadIDs[fileThread->numOfThreadsSpawned] = numOfTIDS++;
	
	fileThread->numOfThreadsSpawned++;
	
	pthread_mutex_trylock(&printMetaData);
	printThreadData(fileThread);
	pthread_mutex_unlock(&printMetaData);
	return ;
}

//Handle threads for a directory
void itsDirectory(pathData *startHere, char *dirname)
{
	pathData* path = (pathData *)malloc(sizeof(pathData));
	snprintf(path->name, 1024, "%s/%s",startHere->name,dirname);
	pthread_t t1 = NULL;
	pthread_create(&t1, NULL, checkValidity, path);
	thread[threadCount++] = t1;
	return;
}

//Print Thread metadata
void printThreadData(threadData * dataToPrint)
{
	int iterator;
	printf("\nInitial PID: %d\n", initialPID);
	fflush(stdout);
	printf("TIDS of all spawned threads: ");
	fflush(stdout);
	for (iterator = 0;iterator < (dataToPrint->numOfThreadsSpawned);iterator++)
	{
		printf("%d,",dataToPrint->threadIDs[iterator]);
		fflush(stdout);
	}
	printf("\nTotal number of threads: %d\n",dataToPrint->numOfThreadsSpawned);
	fflush(stdout);
	return;
}

int generateTID()
{
	return numOfTIDS++;
}