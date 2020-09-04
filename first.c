#include "first.h"

//create struct forever
struct forever
{
	long int touch;
	int reg;
};
//create struct create
struct create 
{
	int count_line; 
	struct forever **line; 
};

int bits(int a) 
{
	return (a & (a - 1)) == 0;
}

void cacheSim(char* fname, int sets, int assoc,int bitsize, int pfetch, struct create **cache,char* simulator)
{
    	FILE *f1 = fopen(fname, "rw");
	int bigbits = log(bitsize) / log(2);
    	int setbits = log(sets) / log(2);
    	int reads = 0;//reads
    	int bighit = 0;//bighit
    	int writes = 0;//writes
    	int reg = 0;//reg
    	int misses = 0;//misses
    	int hits = 0;//hits
    	int i;//index

	fscanf(f1, " %14s:", simulator);   
	while (strcmp(simulator, "#eof") != 0) 
	{
        	char r;
        	fscanf(f1, " %c", &r);
        	if (r == 'W') 
		{
	         	writes++;
        	} 
        	char * temp;
        	temp = malloc(sizeof(char) * 3);
        	fscanf(f1, " %2s", temp);
        	long int address;
        	fscanf(f1, " %lx", &address);
        	long int addrprefetch = address + bitsize;
        	address = address >> bigbits;
        	addrprefetch = addrprefetch >> bigbits;
        	int index = ((1 << setbits) - 1) & address;
        	address = address >> setbits;
        	int indxprefetch = ((1 << setbits) - 1) & addrprefetch;
        	addrprefetch = addrprefetch >> setbits;
        	
		for (i = 0; i < cache[index]->count_line; i++) 
		{
			if (cache[index]->line[i]->touch == address) 
			{
                		bighit = 1;
                		cache[index]->line[i]->reg = reg;
                		reg++;
        		}	
	//free(temp);
		}
        if (bighit == 1) 
	{
           	bighit = 0;
           	hits++;
        } 
	else 
	{ 
           	misses++;
           	reads++;
            	if (cache[index]->count_line == assoc) 
		//LRU
	    	{ 
                	int min = 0;
                	for (i = 1; i < assoc; i++) 
			{ 	
                   		if (cache[index]->line[i]->reg < cache[index]->line[min]->reg) 
				{
                        		min = i;
                    		}
                	}
                	cache[index]->line[min]->touch = address;
                	cache[index]->line[min]->reg = reg;
                	reg++;
            	} 
		else 
		{
                	cache[index]->line[cache[index]->count_line]->touch = address;
                	cache[index]->count_line++;
            	}
            	if (pfetch) 
		{
                	for (i = 0; i < cache[indxprefetch]->count_line; i++) 
			{
                    		if (cache[indxprefetch]->line[i]->touch == addrprefetch) 
				{
                        		bighit = 1;
                    	}
                } 
		//checks if prefetch address is still in cache
                if (bighit == 1) 
		{
                    	bighit = 0;
                } 
		else 
		{
                    	reads++;
                    	if (cache[indxprefetch]->count_line == assoc) 
			{ 	//LRU pt2
                        	int min = 0;
                        	for (i = 1; i < assoc; i++) 
				{
                            	if (cache[indxprefetch]->line[i]->reg < cache[indxprefetch]->line[min]->reg) 
				{
  	                         	min = i;
                            	}
                        }
                        cache[indxprefetch]->line[min]->touch = addrprefetch;
                        cache[indxprefetch]->line[min]->reg = reg;
                        reg++;
                } 
		else 
		{
                        cache[indxprefetch]->line[cache[indxprefetch]->count_line]->touch = addrprefetch;
			cache[indxprefetch]->count_line++;
                }
             	}
          	}
      		}
      		fscanf(f1, " %14s:", simulator);
		free(temp);
    	}
    	fclose(f1);
    	printf("Memory reads: %d\n", reads);
    	printf("Memory writes: %d\n", writes);
    	printf("Cache hits: %d\n", hits);
    	printf("Cache misses: %d\n", misses);
}
int main(int argc, char ** argv) 
{
	int cacheSize = atoi(argv[1]);
	int pfetch = 0;
	int bitsize = atoi(argv[4]);
	if (cacheSize == 0 || !bits(cacheSize) || bitsize == 0 || !bits(bitsize)) 
	{
		return 1;
	}
    	//cache policy
    	if (strcmp(argv[3], "lru") != 0) 
	{
        	printf("wrong\n");
    	}
    	char* fname = argv[5];
	int sets = 0; 
	int assoc = 0;
    	//associativity
	if (strcmp(argv[2], "assoc") == 0) 
	{
		sets = 1;
		assoc = cacheSize / bitsize;
	} 
	else if (strcmp(argv[2], "direct") == 0) 
	{
		assoc = 1;
		sets = cacheSize / bitsize;
	} 
	else 
	{
        	//assoc:n
		char *num = argv[2] + 6; 
		int a = atoi(num);
		if (a == 0 || !bits(a)) 
		{
			return 1;
		} 
		else 
		{
			assoc = a;
			sets = cacheSize / (bitsize * assoc);
		}
}
struct create **cache;
int i,j;
cache = malloc(sizeof(struct create *) * sets);
for (i = 0; i < sets; i++) 
{
        cache[i] = malloc(sizeof(struct create));
        cache[i]->line = malloc(sizeof(struct forever *) * assoc);
        for (j = 0; j < assoc; j++) 
	{
            	cache[i]->line[j] = malloc(sizeof(struct forever));
            	cache[i]->line[j]->touch = -1;//0
            	cache[i]->line[j]->reg = 0;
        }
	
}
char *simulator;
simulator = malloc(sizeof(char) * 30);//15
printf("no-prefetch\n");
cacheSim(fname,sets,assoc,bitsize, pfetch, cache, simulator);
cache = realloc(cache,sizeof(struct create *) * sets);
for (i = 0; i < sets; i++) 
{
        cache[i] = realloc(cache[i],sizeof(struct create));
        cache[i]->line = realloc(cache[i]->line, sizeof(struct forever *) * assoc);
        for (j = 0; j < assoc; j++) 
	{
            	cache[i]->line[j] = realloc(cache[i]->line[j], sizeof(struct forever));
            	cache[i]->line[j]->touch = -1;//0
            	cache[i]->line[j]->reg = 0;
        }
}
simulator = realloc(simulator,sizeof(char) * 30);//15
pfetch = 1;
printf("with-prefetch\n");
cacheSim(fname,sets,assoc,bitsize, pfetch, cache, simulator);
free(cache);
free(simulator);
return 0;
}
