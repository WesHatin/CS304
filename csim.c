//
//Project: csim.c
//Student: Wesley Hatin (wjhatin)
//
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>

#include "cachelab.h"

typedef unsigned long long int mem_addr;

typedef struct 
{
  int s; 
  int b; 
  int E; 
  int S; 
  int B; 

  int hits;
  int misses;
  int evicts;
} cache_values;


typedef struct {
	int last_used;
	int valid;
	mem_addr tag;
	char *block;
} set_line;

typedef struct {
	set_line *lines;
} cache_set;

typedef struct {
	 cache_set *sets;
} cache;


int verbose = 0;

long long bit_power(int exponent) 
{
	long long result = 1;
	result = result << exponent;
	return result;
}


void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


cache new_cache(long long total_sets, int total_lines, long long block_size) 
{
	cache newCache;	
	cache_set set;
	set_line line;
	int index;
	int lineIndex;

	newCache.sets = (cache_set *) malloc(sizeof(cache_set) * total_sets);

	for (index = 0; index < total_sets; index ++) 
	{
		
		set.lines =  (set_line *) malloc(sizeof(set_line) * total_lines);
		newCache.sets[index] = set;

		for (lineIndex = 0; lineIndex < total_lines; lineIndex ++) 
		{
			
			line.last_used = 0;
			line.valid = 0;
			line.tag = 0; 
			set.lines[lineIndex] = line;	
		}
		
	} 

	return newCache;
	
}

void clear_cache(cache sim_cache, long long total_sets, int total_lines, long long block_size) 
{
	int index;
	

	for (index = 0; index < total_sets; index ++) 
	{
		cache_set set = sim_cache.sets[index];
		
		if (set.lines != NULL) 
		{	
			free(set.lines);
		}
		
	} 
	if (sim_cache.sets != NULL) 
	{
		free(sim_cache.sets);
	}
}

int find_empty_line(cache_set query_set, cache_values val) 
{
	int total_lines = val.E;
	int index;
	set_line line;

	for (index = 0; index < total_lines; index ++) 
	{
		line = query_set.lines[index];
		if (line.valid == 0) 
		{
			return index;
		}
	}
	return -1;
}

int find_evict_line(cache_set query_set, cache_values val, int *used_lines) 
{
	int total_lines = val.E;
	int max_used = query_set.lines[0].last_used;
	int min_used = query_set.lines[0].last_used;
	int min_used_index = 0;

	set_line line; 
	int lineIndex;

	for (lineIndex = 1; lineIndex <total_lines; lineIndex ++) {
		line = query_set.lines[lineIndex];

		if (min_used > line.last_used) {
			min_used_index = lineIndex;	
			min_used = line.last_used;
		}

		if (max_used < line.last_used) {
			max_used = line.last_used;
		}
	}

	used_lines[0] = min_used;
	used_lines[1] = max_used;
	return min_used_index;
}

cache_values run_sim(cache sim_cache, cache_values val, mem_addr address) {
		
		int lineIndex;
		int cache_full = 1;

		int total_lines = val.E;
		int prev_hits = val.hits;

		int tag_size = (64 - (val.s + val.b));
		mem_addr input_tag = address >> (val.s + val.b);
		unsigned long long temp = address << (tag_size);
		unsigned long long index = temp >> (tag_size + val.b);
		
  		cache_set query_set = sim_cache.sets[index];

		for (lineIndex = 0; lineIndex < total_lines; lineIndex ++) 	
		{
			set_line line = query_set.lines[lineIndex];
			
			if (line.valid) 
			{
					
				if (line.tag == input_tag) 
				{
					line.last_used ++;
					val.hits ++;
					query_set.lines[lineIndex] = line;
				}

			} 
			
			else if (!(line.valid) && (cache_full)) 
			{
				cache_full = 0;		
			}

		}	

		if (prev_hits == val.hits) 
		{
			val.misses++;
		} 
		
		else 
		{
			return val;
		}
		
		int *used_lines = (int*) malloc(sizeof(int) * 2);
		int min_used_index = find_evict_line(query_set, val, used_lines);	

		if (cache_full) 
		{
			val.evicts++;

			//Found least-recently-used line, overwrite it.
			query_set.lines[min_used_index].tag = input_tag;
			query_set.lines[min_used_index].last_used = used_lines[1] + 1;
		
		}

		else
	        {
			int empty_index = find_empty_line(query_set, val);

			//Found first empty line, write to it.
			query_set.lines[empty_index].tag = input_tag;
			query_set.lines[empty_index].valid = 1;
			query_set.lines[empty_index].last_used = used_lines[1] + 1;
		}						

		free(used_lines);
		return val;
}

int main(int argc, char **argv)
{
  cache sim_cache;
  cache_values val;
  bzero(&val, sizeof(val));

  FILE *read_file;
  char trace_type;
  mem_addr address;
  int size;
  
  char *trace_file;
  char c;
  
  while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1)
  {
    switch(c)
    {
      case 's':
	  val.s = atoi(optarg);
	  break;
      case 'E':
	  val.E = atoi(optarg);
	  break;
      case 'b':
	  val.b = atoi(optarg);
	  break;
      case 't':
	  trace_file = optarg;
	  break;
      case 'v':
	  verbose = 1;
	  break;
      case 'h':
	  printUsage(argv);
	  exit(0);
      default:
	  printUsage(argv);
	  exit(1);
    }
  }
  
  if (val.s == 0 || val.E == 0 || val.b == 0 || trace_file == NULL) 
  {
  	printf("%s: Missing required command line argument\n", argv[0]);
  	printUsage(argv);
  	exit(1);
  }
  val.S = pow(2.0, val.s);
  val.B = bit_power(val.b);	
  val.hits = 0;
  val.misses = 0;
  val.evicts = 0;
	
  sim_cache = new_cache(val.S, val.E, val.B);
 	
  read_file  = fopen(trace_file, "r");
	
	if (read_file != NULL) 
	{
		while (fscanf(read_file, " %c %llx,%d", &trace_type, &address, &size) == 3) 
		{
		
			switch(trace_type) {
				case 'I':
					break;
				case 'L':
					val = run_sim(sim_cache, val, address);
					if (verbose == 1)
					{
						printf(" %c %llx,%d", trace_type, address, size);
					}
					break;
				case 'S':
					val = run_sim(sim_cache, val, address);
					if (verbose == 1)
					{
						printf(" %c %llx,%d", trace_type, address, size);
					}
					break;
				case 'M':
					val = run_sim(sim_cache, val, address);
					val = run_sim(sim_cache, val, address);	
					if (verbose == 1)
					{
						printf(" %c %llx,%d", trace_type, address, size);
					}
					break;
				default:
					break;
			  
			}
		}
	}
	
	
    printSummary(val.hits, val.misses, val.evicts);
	clear_cache(sim_cache, val.S, val.E, val.B);
	fclose(read_file);

    return 0;
}
