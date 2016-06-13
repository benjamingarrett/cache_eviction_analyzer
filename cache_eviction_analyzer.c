#include "cache_eviction_analyzer.h"

#define TRUE 1
#define FALSE 0

enum UTILITY_TO_RUN { GENERATE_EVICTION_SEQUENCE, ANALYZE_EVICTIONS, BOTH };
char * generate = "generate";

void parse_options(int argc, char ** argv){
    
    int i;
    char utility_to_run[100];
    
    
    
    /* options 
     triage to generate eviction sequence or analyze eviction sequence
     */
    for(i=1; i<argc; i++){
        if(strcmp(argv[i], "-p")==0){
            if(i+1 < argc){
                strcpy(utility_to_run, &argv[++i][0]);
            }
            
        } else if(strcmp(argv[i], "-t")==0){
            if(i+1 < argc){
                k = (uint64_t)atoi(argv[++i]);
                table_size = 2<<(k-1);
            }
        } else if(strcmp(argv[i], "-c")==0){
            if(i+1 < argc){
                strcpy(collision_resolution_type, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "-s")==0){
            if(i+1 < argc){
                strcpy(caching_strategy, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "-i")==0){
            if(i+1 < argc){
                strcpy(fname, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "-q")==0){
            if(i+1 < argc){
                queue_size = (uint64_t)atoi(argv[++i]);
            }
        } else if(strcmp(argv[i], "-threshold")==0){
            if(i+1 < argc){
                cache_miss_threshold = (uint64_t)atoi(argv[++i]);
            }
        }
    }
}

void cache_eviction_analyzer(int argc, char** argv){
    printf("cache eviction analyzer\n");
    parse_options(argc, argv);
}
