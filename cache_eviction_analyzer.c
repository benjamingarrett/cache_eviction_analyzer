#include "cache_eviction_analyzer.h"

#define TRUE 1
#define FALSE 0

#define READ 0
#define WRITE 1
#define DELETE 2
#define EVICTION 3
#define NON_EVICTION 4
#define FAILURE 0
#define SUCCESS 1

#define LINE_BUF_LEN 200

#define MAX_KEY 100000
#define NON_KEY -1
#define NON_TIMESTAMP -1

//#define SHOW_EVENT_LOG  

//#define FAST_OPERATIONS  
#define SLOW_OPERATIONS  
//#define COMPARE_OPERATIONS  

typedef struct {
    int64_t num_results;
    int64_t num_arguments;
    int64_t ** arg;
} result_sequence;

int64_t table_size;
char collision_resolution_type[LINE_BUF_LEN];
char caching_strategy[LINE_BUF_LEN];
char fname[LINE_BUF_LEN];
uint64_t queue_size;
uint64_t cache_miss_threshold;
int64_t num_arguments;

int64_t * timestamp, * key, largest_key, highest_timestamp;
int64_t min_absolute_age, min_relative_age, max_absolute_age, max_relative_age;

int64_t ** get_int64_t_matrix( int64_t n, int64_t m ){
    
    int64_t i;
    int64_t ** matrix;
    
    if(( matrix = (int64_t **)malloc(sizeof(int64_t) * n * m +
                sizeof(int64_t *) * n )) == NULL){
        printf("Generate int64_t matrix error n = %d m = %d\n", (int)n, (int)m );
        exit( EXIT_FAILURE );
    }
    for ( i = 0 ; i < n ; i++ ) {
        matrix[i] = (int64_t *)(matrix + n) + i*m;
    }
    return matrix;
}

void parse_options(int argc, char ** argv){
    
    int i;
    
    for(i=1; i<argc; i++){
        if(strcmp(argv[i], "--table_size")==0){
            if(i+1 < argc){
                table_size = (uint64_t)atoi(argv[++i]);
            }
        } if(strcmp(argv[i], "--caching_strategy")==0){
            if(i+1 < argc){
                strcpy(caching_strategy, &argv[++i][0]);
            }
        } else if(strcmp(argv[i], "--result_sequence")==0){
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
        } else if(strcmp(argv[i], "--num_arguments")==0){
            if(i+1 < argc){
                num_arguments = (int64_t)atoi(argv[++i]);
            }
        }
    }
}

result_sequence * read_event_log(char fname[200]){
    
    FILE * fp;
    result_sequence * rs;
    char buf[200];
    int64_t n, m, g;
    
    fp = fopen(fname,"r");
    rs = calloc(1,sizeof(result_sequence));
    rs->num_arguments = 5;
    while((n = fscanf(fp, "%s", buf)) != EOF){
        #ifdef SHOW_EVENT_LOG
        printf("Processing >%s<\n", buf);
        #endif
        if(strcmp("NUM_OPERATIONS",buf)==0){
            n = fscanf(fp, "%ld", (long int *)&rs->num_results);
            #ifdef SHOW_EVENT_LOG
            printf("num_results = %ld\n", rs->num_results);
            #endif
            rs->arg = get_int64_t_matrix( rs->num_results, rs->num_arguments );
            for(g=0; g<rs->num_results; g++){
                n = fscanf(fp, "%ld %ld %ld %ld %ld\n", &rs->arg[g][0], &rs->arg[g][1], &rs->arg[g][2], &rs->arg[g][3], &rs->arg[g][4] );
                #ifdef SHOW_EVENT_LOG
                printf("read event line %ld: %ld %ld %ld %ld %ld\n", g, rs->arg[g][0], rs->arg[g][1], rs->arg[g][2], rs->arg[g][3], rs->arg[g][4] );
                #endif
            }
            break;
        }
    }
    fclose(fp);
    return rs;
}

void view_pseudo_cache(int64_t current_time){
    
    int64_t g, m;
    
    m = ( largest_key < highest_timestamp ) ? highest_timestamp : largest_key;
    printf("\n====pseudo cache - current_time %ld\n", current_time);
    for(g=0; g<=m; g++){
        printf("t[%ld]=%ld\tk[%ld]=%ld\n", g, timestamp[g], g, key[g]);
    }
    printf("====\n");
}

void initialize_ages(){
    
    min_absolute_age = LONG_MAX;
    min_relative_age = LONG_MAX;
    max_absolute_age = -1;
    max_relative_age = -1;
}

void update_ages(int64_t absolute_age, int64_t relative_age){
    
    min_absolute_age = ( absolute_age < min_absolute_age ) ? absolute_age : min_absolute_age;
    min_relative_age = ( relative_age < min_relative_age ) ? relative_age : min_relative_age;
    max_absolute_age = ( max_absolute_age < absolute_age ) ? absolute_age : max_absolute_age;
    max_relative_age = ( max_relative_age < relative_age ) ? relative_age : max_relative_age;
}

void view_ages(){
    
    printf("absolute: [%ld,%ld]\trelative: [%ld,%ld]\n", min_absolute_age, max_absolute_age, min_relative_age, max_relative_age);
}

/*====================*/
void set_timestamp_slow(int64_t k, int64_t t){
    
    key[t] = k;
    timestamp[k] = t;
    highest_timestamp = ( highest_timestamp < t ) ? t : highest_timestamp;
    largest_key = ( largest_key < k ) ? k : largest_key;
}
void set_timestamp_fast(int64_t k, int64_t t){
    
    cache_write_long_int(&k,&t);
    skiplist_write_long_int(&t,&k);
}
void set_timestamp(int64_t k, int64_t t){
    
    #ifdef SLOW_OPERATIONS
    set_timestamp_slow(k,t);
    #endif
    #ifdef FAST_OPERATIONS
    set_timestamp_fast(k,t);
    #endif
}
/*====================*/
int64_t get_timestamp_of_key_slow(int64_t k){
    
    return timestamp[k];
}
int64_t get_timestamp_of_key_fast(int64_t k){
    
    return * cache_read_long_int(&k);
}
int64_t get_timestamp_of_key(int64_t k){
    
    int64_t s, f;
    
    #ifdef SLOW_OPERATIONS
    s = get_timestamp_of_key_slow(k);
    #endif
    #ifdef FAST_OPERATIONS
    f = get_timestamp_of_key_fast(k);
    #endif
    #ifdef COMPARE_OPERATIONS
    if(s!=f){
        fprintf(stderr, "error: fast and slow versions of get_timestamp don't match! %ld != %ld\n", f, s);
        exit(EXIT_FAILURE);
    }
    #endif
    #ifdef SLOW_OPERATIONS
    return s;
    #endif
    #ifdef FAST_OPERATIONS
    return f;
    #endif
}
/*====================*/
int64_t get_key_of_timestamp_slow(int64_t t){
    
    return key[t];
}
int64_t get_key_of_timestamp_fast(int64_t t){
    
    return * skiplist_read_long_int(&t);
}
int64_t get_key_of_timestamp(int64_t t){
    
    int64_t s, f;
    
    #ifdef SLOW_OPERATIONS
    s = get_key_of_timestamp_slow(t);
    #endif
    #ifdef FAST_OPERATIONS
    f = get_key_of_timestamp_fast(t);
    #endif
    #ifdef COMPARE_OPERATIONS
    if(s!=f){
        fprintf(stderr, "error: fast and slow versions of get_key_of_timestamp don't match! %ld != %ld\n", f, s);
        exit(EXIT_FAILURE);
    }
    #endif
    #ifdef SLOW_OPERATIONS
    return s;
    #endif
    #ifdef FAST_OPERATIONS
    return f;
    #endif
}
/*====================*/
int64_t get_absolute_age_of_eviction_slow(int64_t k, int64_t current_time){
    
    return current_time - get_timestamp_of_key_slow(k);
}
int64_t get_absolute_age_of_eviction_fast(int64_t k, int64_t current_time){
    
//    return get_absolute_age_of_eviction_slow(k, current_time);
    return current_time - get_timestamp_of_key_fast(k);
}
int64_t get_absolute_age_of_eviction(int64_t k, int64_t current_time){
    
    int64_t s, f;
    
    #ifdef SLOW_OPERATIONS
    s = get_absolute_age_of_eviction_slow(k,current_time);
    #endif
    #ifdef FAST_OPERATIONS
    f = get_absolute_age_of_eviction_fast(k,current_time);
    #endif
    #ifdef COMPARE_OPERATIONS
    if(s!=f){
        fprintf(stderr, "error: fast and slow versions of get_absolute_age_of_eviction don't match! %ld != %ld\n", f, s);
//        exit(EXIT_FAILURE);
    }
    #endif
    #ifdef SLOW_OPERATIONS
    return s;
    #endif
    #ifdef FAST_OPERATIONS
    return f;
    #endif
}
/*====================*/

int64_t get_relative_age_of_eviction_slow(int64_t k, int64_t current_time){
    
    int64_t g, relative_age;
    
    relative_age = 0;
    for(g=current_time; 0<=g; g--){
        if(key[g] != NON_KEY){
            if(key[g] == k){
                break;
            } else {
                relative_age++;
            }
        }
    }
    return relative_age;
}
int64_t get_relative_age_of_eviction_fast(int64_t k, int64_t current_time){
    
//    return get_relative_age_of_eviction_slow(k, current_time);
    return size_of_long_int() - 1 - index_of_long_int(cache_read_long_int(&k));
}
int64_t get_relative_age_of_eviction(int64_t k, int64_t current_time){
    
    int64_t s, f;
    
    #ifdef SLOW_OPERATIONS
    s = get_relative_age_of_eviction_slow(k,current_time);
    #endif
    #ifdef FAST_OPERATIONS
    f = get_relative_age_of_eviction_fast(k,current_time);
    #endif
    #ifdef COMPARE_OPERATIONS
    if(s!=f){
        fprintf(stderr, "error: fast and slow versions of get_relative_age_of_eviction don't match! %ld != %ld\n", f, s);
        printf("index of %ld = %ld, size = %ld\n", 
                *cache_read_long_int(&k), index_of_long_int(cache_read_long_int(&k)), size_of_long_int());
        skiplist_premium_dump_long_int();
        view_pseudo_cache(current_time);
//        exit(EXIT_FAILURE);
    }
    #endif
    #ifdef SLOW_OPERATIONS
    return s;
    #endif
    #ifdef FAST_OPERATIONS
    return f;
    #endif
}
/*====================*/

void evict_key_slow(int64_t k){
    
    key[timestamp[k]] = NON_KEY;
    timestamp[k] = NON_TIMESTAMP;
//    printf("evict_key %ld\n", k);
}
void evict_key_fast(int64_t k){
    
    cache_delete_long_int(&k);
    skiplist_delete_long_int(&k);
}
void evict_key(int64_t k){
    
    #ifdef FAST_OPERATIONS
    evict_key_fast(k);
    #endif
    #ifdef SLOW_OPERATIONS
    evict_key_slow(k);
    #endif
}

/*====================*/

void analyze_sequence_of_evictions(){
    
    int64_t g, evicted_key, inserted_key, absolute_age, relative_age, current_time;
    result_sequence * rs;
    
    printf("analyze_sequence_of_evictions, result sequence:>%s<\n", fname);
    rs = read_event_log(fname);
    initialize_skiplist_long_int();
    initialize_long_int_cache(100000, 16, "hashing_linear_probe_with_deletions");
    timestamp = calloc(MAX_KEY,sizeof(int64_t));
    key = calloc(MAX_KEY,sizeof(int64_t));
    current_time = 0;
    largest_key = highest_timestamp = -1;
    for(g=0; g<MAX_KEY; g++){
        timestamp[g] = NON_TIMESTAMP;
        key[g] = NON_KEY;
    }
    g = 0;
    current_time = 1;
    initialize_ages();
//    printf("num results: %ld\n", rs->num_results);
    while( g < rs->num_results ){
//        printf("result: %ld/%ld\n", g, rs->num_results);
        if(rs->arg[g][4]==SUCCESS){
            switch( rs->arg[g][0] ){
                case READ:
//                    printf("successful read\n");
                    break;
                case WRITE:
                    if(rs->arg[g][1]==EVICTION){
//                        printf("eviction detected\n");
                        evicted_key = rs->arg[g][3];
                        g++;
                        inserted_key = rs->arg[g][2];
                        set_timestamp(inserted_key, current_time);
                        absolute_age = get_absolute_age_of_eviction(evicted_key, current_time);
                        relative_age = get_relative_age_of_eviction(evicted_key, current_time);
                        update_ages(absolute_age, relative_age);
                        evict_key(evicted_key);
//                        printf("time %ld: insert key %ld, evict key %ld, absolute age %ld, relative age %ld\n", 
//                                current_time, inserted_key, evicted_key, 
//                                absolute_age,
//                                relative_age );
                        current_time++;
                    } else if(rs->arg[g][1]==NON_EVICTION){
                        inserted_key = rs->arg[g][2];
//                        printf("time %ld: insert key %ld (no eviction)\n", current_time, inserted_key);
                        set_timestamp(inserted_key, current_time);
                        current_time++;
                    } else {
                        printf("result sequence error!\n");
                        exit(EXIT_FAILURE);
                    }
                    break;
                default:
                    printf("result sequence error: unknown operation %ld\n", rs->arg[g][0]);
                    exit(EXIT_FAILURE);
            }
//            printf("successful operation %ld done\n", g);
        }
        g++;
    }
    view_ages();
}


//hashtable: (key, time)
//skiplist: (time, key)

//{EVICTION/NON-EVICTION},new_key,old_key
//{READ/WRITE/DELETE},key,result

//for g=1 to N
//  switch(op){
//    case read:
//      if (key[g] not in skiplist AND result==true) OR (key[g] in skiplist AND result==FALSE)
//        log an error
//      if version_two
//        delete key from skiplist and hashtable
//        insert same key with new timestamp
//    case write:
//      if (key[g] not in skiplist AND result==true) OR (key[g] in skiplist AND result==FALSE)
//        log an error
//      delete the evicted key from both skiplist and hashtable
//      update the youngest key
//      insert new eviction into both skiplist and hashtable

void cache_eviction_analyzer(int argc, char** argv){
    
    printf("cache eviction analyzer\n");
    parse_options(argc, argv);
    analyze_sequence_of_evictions();
}
