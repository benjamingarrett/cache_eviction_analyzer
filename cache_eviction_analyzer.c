#include "cache_eviction_analyzer.h"

#define TRUE 1
#define FALSE 0

#define READ 0
#define WRITE 1
#define DELETE 2
#define FAILURE 0
#define SUCCESS 1

#define GENERATE_EVICTION_SEQUENCE 0
#define ANALYZE_EVICTIONS 1
#define LINE_BUF_LEN 200

int8_t utility;
int64_t table_size;
char collision_resolution_type[LINE_BUF_LEN];
char caching_strategy[LINE_BUF_LEN];
char fname[LINE_BUF_LEN];
uint64_t queue_size;
uint64_t cache_miss_threshold;

void parse_options(int argc, char ** argv){
    
    int i;
    
    /* options 
     triage to generate eviction sequence or analyze eviction sequence
     */
    for(i=1; i<argc; i++){
        if(strcmp(argv[i], "--generate")==0){
            utility = GENERATE_EVICTION_SEQUENCE;
        } else if(strcmp(argv[i], "--analyze")==0){
            utility = ANALYZE_EVICTIONS;
        } else if(strcmp(argv[i], "--table_size")==0){
            if(i+1 < argc){
                table_size = (uint64_t)atoi(argv[++i]);
            }
        } else if(strcmp(argv[i], "--collision_resolution_type")==0){
            if(i+1 < argc){
                strcpy(collision_resolution_type, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "--caching_strategy")==0){
            if(i+1 < argc){
                strcpy(caching_strategy, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "-i")==0){
            if(i+1 < argc){
                strcpy(fname, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "--queue_size")==0){
            if(i+1 < argc){
                queue_size = (uint64_t)atoi(argv[++i]);
            }
        } else if(strcmp(argv[i], "--threshold")==0){
            if(i+1 < argc){
                cache_miss_threshold = (uint64_t)atoi(argv[++i]);
            }
        }
    }
}

void generate_eviction_sequence(){
    
    printf("generate eviction sequence\n");
}

void analyze_eviction_sequence(){
    
    printf("analyze eviction sequence\n");
}

void cache_eviction_analyzer(int argc, char** argv){
    
    printf("cache eviction analyzer\n");
    utility = GENERATE_EVICTION_SEQUENCE;
    parse_options(argc, argv);
    switch(utility){
        case GENERATE_EVICTION_SEQUENCE:
            generate_eviction_sequence();
            break;
        case ANALYZE_EVICTIONS:
            analyze_eviction_sequence();
            break;
        default:
            fprintf(stderr, "Unknown utility option.\n");
            exit(EXIT_FAILURE);
    }
}
