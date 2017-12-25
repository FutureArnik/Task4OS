#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

pthread_mutex_t region_mutex =
    PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t space_available =
    PTHREAD_COND_INITIALIZER;
pthread_cond_t data_available =
    PTHREAD_COND_INITIALIZER;

int studentInRoom = 0;
int const N = 10;
_Bool commandCondition = false;
_Bool end = false;

int main()
{
	pthread_t studentIn_thread;
	pthread_t commandant_thread;
	pthread_t studentOut_thread;

    void *studentIn();
	void *studentOut();
	void *commandant();
    pthread_create(&studentIn_thread, NULL,
                   studentIn, NULL);
	pthread_create(&studentOut_thread, NULL,
                   studentOut, NULL);
	pthread_create(&commandant_thread, NULL,
                   commandant, NULL);
    /*pthread_join(studentIn_thread,NULL);
	pthread_join(studentOut_thread, NULL);*/
	pthread_join(commandant_thread, NULL);
	return 0;
}

void *studentIn()
{
	int i = 0;
	int random;
	while(1)
	{
		pthread_mutex_lock(&region_mutex);
		if(commandCondition == true)
			pthread_cond_wait(&space_available,
                              &region_mutex);
		random = rand() % 50;
		if(i%1000000 == 0 && random > 25)		
		{
			if(studentInRoom == 0 && commandCondition == false)
			{
				printf("Студент зашел в комнату и включил музыку. Вечеринка началась. В комнате %d студент\n", ++studentInRoom);
			}
			else if(studentInRoom > 0 && commandCondition == false)
			{
				printf("Студент зашел в комнату. В комнате %d студент\n", ++studentInRoom);
			}
			else if(studentInRoom > 0 && commandCondition == true)
			{
				printf("Студент хотел зайти в комнату, но там был комендант, поэтому он ушел. В комнате: комендант и %d студентов.\n", studentInRoom);
			}
		}
		pthread_cond_signal(&data_available);
		pthread_mutex_unlock(&region_mutex);
		i++;
		if (i == 1000000)
			i = 0;	
	}
	pthread_exit(NULL);
}

void *studentOut()
{
	int i = 0;
	int random;
	while(1)
	{
		pthread_mutex_lock(&region_mutex);
		if(studentInRoom < 1)
			pthread_cond_wait(&data_available,
                              &region_mutex);
		if(commandCondition == false)
		{		
			random = rand() % 50;
		}
		else
		{
			random = rand() %50 + 15;
		}

		if(i%1000000 == 0 && random > 35)	
		{
			if((studentInRoom-1) > -1)
			{
				studentInRoom--;
			}
			if(studentInRoom == 0)
			{
				printf("Студент вышел из комнаты. В комнате никого\n");
				if(commandCondition == true)
				{
					end = true;
				}
			}
			else
			{
				printf("Студент вышел из комнаты. В комнате %d студент\n", studentInRoom);
			}
		}
		pthread_cond_signal(&space_available);
		pthread_mutex_unlock(&region_mutex);
		i++;
		if(i == 1000000)
			i = 0;
	}
	pthread_exit(NULL);
}

void *commandant()
{	
	int i = 0,random;
	while(1)
	{
		random = rand() % 100;
		if(i%1000000 == 0)
		{
			pthread_mutex_lock(&region_mutex);
			if((random > 67 && random < 71) && commandCondition == false && (studentInRoom > N || studentInRoom == 0))
			{
				if(studentInRoom == 0)
				{
					printf("Комендант зашел в комнату, чтобы совершить обыск. В комнате: комендант.\n");
					commandCondition = true;
					end = true;
				}
				else
				{
					printf("Комендант зашел в комнату, так как слишком шумно. В комнате: коммендант и %d студентов.\n", studentInRoom);		
					commandCondition = true;
				}
			}
			else if((random > 69 && random < 71) && commandCondition == false && (studentInRoom < N && studentInRoom > 0))
			{
				printf("Комендант прошел мимо комнаты и ушел по своим делам. \n");
			}
			pthread_mutex_unlock(&region_mutex);
		}
		if(i == 1000000)
			i = 0;

		if(end == true)
		{
			break;
		}
		i++;
	}
	pthread_exit(NULL);	
}
