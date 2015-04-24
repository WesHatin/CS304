//
//Project: csim.c
//Student: Wesley Hatin (wjhatin)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>
#include "cachelab.h"

typedef unsigned long long int memoryAddress;

typedef struct{
  int valid = 0;
  int tag;
  int set;
  int bit;
}lineData;


int bitMask(int highbit, int lowbit) 
{
  //Create masks for above, below, and most significant bit of chosen mask, bitOr to produce opposite of mask,
  //	flip to create chosen mask
  int high_mask, low_mask, high_bit_mask;
  
  high_mask = ~0<<highbit;
  high_bit_mask = ~(1<<highbit);
  low_mask = ~(~0<<lowbit);
  high_mask = high_mask & high_bit_mask;
  return ~(high_mask|low_mask);
}

int main()
{
  char **cache, traceName[16], *address = NULL;
  int hit_count, miss_count, eviction_count
  int verbose = 0, setBits, assoc, blockBits;
  int setNumber, blockSize;
  
  while ((opt = getopt(argc, argv, "v::s:E:b:t:")) != -1) 
  {
        switch (opt) 
	{
            case 'v':
                verbose = 1;
                break;
            case 's':
	      setBits = atoi(optarg);
                break;
            case 'E':
	      assoc = atoi(optarg);
                break;
	    case 'b':
	      blockBits = atoi(optarg);
                break;
	    case 't':
                traceName = optarg;
                break;
	    default: 
                
                exit(1);
        }
  }
  
  if (par.s == 0 || par.E == 0 || par.b == 0 || trace_file == NULL) 
  {
        printf("%s: Command line argument not found\n", argv[0]);
        printUsage(argv);
        exit(1);
  }
  
  read_trace  = fopen(traceName, "r");
  
  cache = (int *)malloc(cacheSize * sizeof(int));
  while ((bytes_read = getline (&address, &nbytes, *trace))!=(-1))
  {
  }
    
    printSummary(hit_count, miss_count, eviction_count);
    fclose(traceName);
    
    return 0;
}

