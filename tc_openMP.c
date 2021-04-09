#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<omp.h>

#define TC 2138280
#define LEN 30
#define TNUM 8


char strings[TC][LEN];	//storing the strings of taxi walks in length LEN
unsigned long long cn;	//storing the final count
bool hasTwoConsecTurns(char, char, char, char);		//check whether it made two consecutive turns

int main()
{
	cn = 0; 	//initialize the count

	/* Read the strings from the strings.txt file */
	FILE* fp;
	if ((fp = fopen("./strings.txt", "r")) == NULL)
	{
		perror("Error opening file");
		return -1;
	}
	unsigned int i = 0;
	const size_t line_size = 100;
	char* line = malloc(line_size);
	while (fgets(line, line_size, fp) != NULL) 
	{
		strncpy(strings[i], line, LEN);
		strings[i][LEN] = '\0';
		i++;
	}
	free(line);

	int len = 2*LEN;		//length of the new string
	int size = 2 *len + 1;	//size of the map
	
	int*** visited_all = (int***)malloc(TNUM*sizeof(int*));
	

	for (int i=0;i<TNUM;i++)
	{	
		int** visited = (int**)malloc(size*sizeof(int*));
		visited_all[i] = visited;
		for (int i=0; i<size; i++)
		{	
			visited[i] = malloc(size*sizeof(int));
		}
	}

	/* Initialize the arrays to all false (non-visited) */
	for (int k = 0; k<TNUM; k++)
	{ 
		//tcn[k] = 0;
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				visited_all[k][i][j] = 0;
			}
		}
	}


	#pragma omp parallel num_threads(TNUM)
	//#pragma omp for schedule(guided) reduction(+:cn)
        
	#pragma omp for schedule(dynamic) reduction(+:cn)

	
	
	/* Nested loop to append all strings and check it it's a valid taxi walk */
	for (unsigned int i=TC*11/24; i<TC/2;i++)
	{
			int id = omp_get_thread_num();	//thread_id

			/* Create the string by concatenating two old strings */
			
			char new[len];
			
			memcpy(new,strings[i],LEN);

			/* Initialize the coordinates */
			int x=0;
			int y=0;
			

			//char * new = strings[i];
			
			int** visited = visited_all[id];
			
			/* Compute the new coordinates accordingly */
			for (int ii=0;ii<LEN;ii++)
			{
				if (new[ii]=='0')
			//	if (strings[i][ii]=='0')

				{
					if (x%2 == 0)
					{
						y++;
					}
					else
					{
						y--;
					}
				}
				//if (new[ii]=='1')
				else
				{
					if(y%2==0)
					{
						x++;
					}
					else
					{
						x--;
					}
				}			
				visited[size/2-y][x+size/2] = i+1;

			}
			visited[size/2][size/2] = i+1;		
			
			//origin is visited	
			/*
			visited_all[id][size/2-y][x+size/2] = i+1;

			}
			visited_all[id][size/2][size/2] = i+1;		//origin is visited	
			*/
			
			
			
		for (unsigned int j=0; j<TC;j++)
		{			
			
			/* If the string contains two consecutive turns, no need to continue */
			
			//if (hasTwoConsecTurns(strings[i][LEN-2],strings[i][LEN-1],strings[j][0], strings[j][1]))
			
			if(hasTwoConsecTurns(new[LEN-2],new[LEN-1],strings[j][0], strings[j][1]))
			{
				continue;
			}

			else
			{
				memcpy(&new[LEN],strings[j],LEN);			
			
			

			bool inc = true;	//flag for whether to increase the count
			
			/* Initialize the coordinates */
			int x2=x;
			int y2=y;

			/* Compute the new coordinates accordingly */
			for (int ii=LEN;ii<len;ii++)
			{
				if (new[ii]=='0')
				//	if (new2[ii]=='0')
				//if (strings[j][ii]=='0')
				{
					if (x2%2 == 0)
					{
						y2++;
					}
					else
					{
						y2--;
					}
				}
				else
				//if (new[ii]=='1')
				{
					if(y2%2==0)
					{
						x2++;
					}
					else
					{
						x2--;
					}
				}

			/* If the new pos is already visited, not a valid taxi walk */				
			if(visited[size / 2 - y2][x2 + size / 2]==i+1)
			//if(visited_all[id][size / 2 - y2][x2 + size / 2]==i+1)
				{
					inc = false;
					break;
				}
			}
		
			/* If it's a valid taxi walk, increase the count */
			if (inc)
			{
				cn++;
			}
			}
		}
			
	}
	
	
	/* Compute and print the final count */
	printf("Number of taxi counts: %lld\n",cn*2);



	/* Clean the heap */
	for (int i=0;i<TNUM;i++)
	{	
		for (int j=0; j<size; j++)
		{	
			free(visited_all[i][j]);
		}
		free(visited_all[i]);
	}
	free(visited_all);


	return 0;
}

/* Check whether there is a 101 or 010 in the string, which means two consecutive turns made */
bool hasTwoConsecTurns(char a, char b, char c, char d)
{
	if (a=='0' && b=='1' && c=='0')
	{
		return true;
	}
	else if (a=='1' && b=='0' && c=='1')
	{
		return true;
	}
	else if (b=='0' && c=='1' && d=='0')

	{
		return true;
	}
	else if (b=='1' && c=='0' && d=='1')
	{
		return true;
	}
	else
	{
		return false;
	}
}
	
