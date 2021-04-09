#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<omp.h>
#include<math.h>

#define TC 2138280
#define M_SIZE 20114
#define M_LEN 20
#define LEN 30
#define TNUM 8
#define cn_size 524288
#define cn_size2 1048576

char strings[TC][LEN+1];	//storing the strings of taxi walks in length LEN
char matrix[M_SIZE][M_LEN+1]; //storing the taxi walks at length M_LEN
bool hasTwoConsecTurns(char, char, char, char);		//check whether it made two consecutive turns
unsigned long long cn;	//storing the final count


int main()
{
	int** first_index =  (int**)malloc(sizeof(int*)*cn_size);	//pointers for starting strings
	int print_num[M_SIZE/2];	//storing indices of numbers to be printed


	int len_diff = LEN - M_LEN;	

	/* Read the strings from the matrix.txt file */
	FILE* mfp;
	if ((mfp = fopen("./matrix.txt", "r")) == NULL)
	{
		perror("Error opening file");
		return -1;
	}
	unsigned int i = 0;
	const size_t line_size = 100;
	char* line = malloc(line_size);
	while (fgets(line, line_size, mfp) != NULL) 
	{
		strncpy(matrix[i], line, M_LEN);
		matrix[i][M_LEN] = '\0';
		i++;
	}
	
	/* create arrays for ending string */
	for (int i=0; i<M_SIZE/2; i++)
	{
		char toDec[M_LEN];
		memcpy(&toDec[0],matrix[i],M_LEN);
		int taxi_walk_val = strtol(toDec, NULL,2);
		//printf("%d ",taxi_walk_val);
		first_index[taxi_walk_val] = (int*)malloc(cn_size * sizeof(int));
		for (int j=0; j<cn_size;j++)
		{
			first_index[taxi_walk_val][j] = 0;
		}
		print_num[i] = taxi_walk_val; 
	}
	free(line);
	fclose(mfp);
	
	
	/* Read the strings from the strings.txt file */
	FILE* fp;
	if ((fp = fopen("./strings.txt", "r")) == NULL)
	{
		perror("Error opening file");
		return -1;
	}
	i = 0;
	char* line2 = malloc(line_size);
	while (fgets(line2, line_size, fp) != NULL) 
	{
		strncpy(strings[i], line2, LEN);
		strings[i][LEN] = '\0';
		//printf("%lld\n",i);
		i++;
	}
	free(line2);
	fclose(fp);

	
	int len = 2*LEN;		//length of the new string
	int size = 2 *len + 1;	//size of the map



	/* create "the number of threads" arrays to store the visited positions */
	int*** visited_all = (int***)malloc(TNUM*sizeof(int**));
	

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
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				visited_all[k][i][j] = 0;
			}
		}
	}

	#pragma omp parallel num_threads(TNUM) 
	#pragma omp for schedule(dynamic) reduction(+:cn)
	/* Nested loop to append all strings and check it it's a valid taxi walk */
	for (unsigned int i=TC*11/24; i<TC*12/24;i++)
	{
			int id = omp_get_thread_num();	//thread_id
			//int id = 0;	//thread_id

			char new[M_LEN+1];
			memcpy(new,strings[i],M_LEN);
			new[M_LEN] = '\0';
			
			/*index for the starting M_LEN string */
			int* idx = first_index[strtoll(new, NULL,2)];		
			
			/* Initialize the coordinates */
			int x=0;
			int y=0;
			
			/* Compute the new coordinates accordingly */
			for (int ii=0;ii<LEN;ii++)	
			{
				if (strings[i][ii]=='0')
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
				//if (new[i]=='1')
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
				visited_all[id][size/2-y][x+size/2] = i+1;

			}
				visited_all[id][size/2][size/2] = i+1;		//origin is visited	

		for (unsigned int j=0; j<TC;j++)
		{
			/* If the string contains two consecutive turns, no need to continue */
			if (hasTwoConsecTurns(strings[i][LEN-2],strings[i][LEN-1],strings[j][0], strings[j][1]))
			{
				continue;
			}

			bool inc = true;	//flag for whether to increase the count
			
			/* Initialize the coordinates */
			int x2=x;
			int y2=y;

			/* Compute the new coordinates accordingly */
			for (int ii=0;ii<LEN;ii++)
			{
				if (strings[j][ii]=='0')
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
				//if (strings[j][ii]=='1')
				else
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
				if (visited_all[id][size/2-y2][x2+size/2] == i+1)
				{
					inc = false;
					break;
				}

			}
			


			/* If it's a valid taxi walk, increase the count */
	if (inc)
	{
		char new_2[M_LEN+1];
		int idx_j = 0;
		memcpy(new_2,&strings[j][LEN-M_LEN],M_LEN);
		new_2[M_LEN]='\0';
		/* if the starting char is 1, take its complement */
		if (strings[j][LEN-M_LEN] == '1')
		{
			idx_j = cn_size2 - 1 - strtol(new_2, NULL, 2);
		}
		else
		{
			idx_j = strtol(new_2, NULL, 2);
		}				
		#pragma omp atomic
		idx[idx_j]++;
		cn++;

	}
	}
}
	
	/* Compute and print the final count */
	printf("Number of taxi counts: %lld\n",cn*2);
	
		for (int i=0; i<M_SIZE/2;i++)
		{
			for (int j=0; j<M_SIZE/2;j++)
			{
				printf("%d ", first_index[print_num[i]][print_num[j]]);
			}
			printf("\n");
		}
		

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

