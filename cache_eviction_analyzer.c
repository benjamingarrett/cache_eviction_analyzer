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

int64_t table_size;
char collision_resolution_type[LINE_BUF_LEN];
char caching_strategy[LINE_BUF_LEN];
char fname[LINE_BUF_LEN];
uint64_t queue_size;
uint64_t cache_miss_threshold;

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

void analyze_eviction_sequence(){
    
    FILE * fp;
    int n;
    int64_t operation_, eviction_, new_key_, old_key_;
    int64_t * operation, * eviction, * new_key, * old_key;
    int64_t g, num_lines, current_time, lowest_absolute_age, * last_access_time, * evicted_last_access_time;
    int64_t relative_age, lowest_relative_age, absolute_age, highest_relative_age, highest_absolute_age;
    
    printf("analyze eviction sequence\n");
    fp = fopen("event_log","r");
    if(fp == NULL){
        fprintf(stderr, "Unable to open event_log\n");
        exit(EXIT_FAILURE);
    }
    num_lines = 0;
    do {
        n = fscanf(fp, "%ld %ld %ld %ld\n", &operation_, &eviction_, &new_key_, &old_key_);
        num_lines++;
//        printf("%d items read %ld lines read: %ld %ld %ld %ld\n", n, num_lines, operation_, eviction_, new_key_, old_key_);
    } while( 0 < n );
    fclose(fp);
    num_lines--;
    operation = calloc(num_lines, sizeof(int64_t));
    eviction = calloc(num_lines, sizeof(int64_t));
    new_key = calloc(num_lines, sizeof(int64_t));
    old_key = calloc(num_lines, sizeof(int64_t));
    fp = fopen("event_log","r");
    g = 0;
    printf("num_lines: %ld\n", num_lines);
    while(g < num_lines){
        fscanf(fp, "%ld %ld %ld %ld\n", &operation[g], &eviction[g], &new_key[g], &old_key[g]);
        printf("item %ld: %ld %ld %ld %ld\n", g, operation[g], eviction[g], new_key[g], old_key[g]);
        g++;
    }
    fclose(fp);
    
    initialize_skiplist_long_int();
    initialize_long_int_cache(4096, 16, "hashing_linear_probe_with_deletions");
    lowest_absolute_age = LONG_MAX;
    highest_absolute_age = -1;
    lowest_relative_age = LONG_MAX;
    highest_relative_age = -1;
    current_time = 0;
    
    g = 0;
    while( g < num_lines ){
        switch(operation[g]){
            case READ:
                last_access_time = cache_read_long_int(&new_key[g]);
                if( last_access_time != NULL ){
//                    printf("deleting old last_access_time %ld key %ld\n", *last_access_time, new_key);
//                    cache_delete_long_int(&new_key[g]);
//                    skiplist_delete_long_int(last_access_time);
//                    printf("adding current_time %ld key %ld\n", current_time, new_key);
//                    cache_write_long_int(&new_key[g], &current_time);
//                    skiplist_write_long_int(&current_time, &new_key[g]);
//                    view_hashtable_long_int();
//                    skiplist_full_dump_long_int();
                }
                break;
            case WRITE:
                last_access_time = cache_read_long_int(&new_key[g]);
                if( last_access_time == NULL ){
                    //this means we're trying to to write something that's not there
                    evicted_last_access_time = cache_read_long_int(&old_key[g]);
                    if( evicted_last_access_time != NULL ){
                        relative_age = index_of_long_int(evicted_last_access_time);
                        absolute_age = current_time - *evicted_last_access_time;
//                        printf("lowest absolute %ld current absolute %ld\n", lowest_absolute_age, absolute_age);
//                        printf("lowest relative %ld current relative %ld\n", lowest_relative_age, relative_age);
                        lowest_absolute_age = (absolute_age<lowest_absolute_age) ? absolute_age : lowest_absolute_age;
                        highest_absolute_age = (absolute_age>highest_absolute_age) ? absolute_age : highest_absolute_age;
                        lowest_relative_age = (relative_age<lowest_relative_age && relative_age!=-1) ? relative_age : lowest_relative_age;
                        highest_relative_age = (relative_age>highest_relative_age && relative_age!=-1) ? relative_age : highest_relative_age;
//                        printf("deleting (evicting) evicted_last_access_time %ld key %ld\n", *evicted_last_access_time, old_key);
                        cache_delete_long_int(&old_key[g]);
                        skiplist_delete_long_int(evicted_last_access_time);
//                        view_hashtable_long_int();
//                        skiplist_full_dump_long_int();
                    }
//                    printf("adding new item with current_time %ld key %ld\n", current_time, new_key);
                    cache_write_long_int(&new_key[g], &current_time);
                    skiplist_write_long_int(&current_time, &new_key[g]);
                }
                break;
            case DELETE:
                break;
            default:
                fprintf(stderr, "Unknown operation %ld\n", (long int)operation);
                exit(EXIT_FAILURE);
        }
        current_time++;
        g++;
    }
    printf("Done: lowest_absolute_age  = %ld   lowest_relative_age  = %ld\n", lowest_absolute_age, lowest_relative_age);
    printf("      highest_absolute_age = %ld   highest_relative_age = %ld\n", highest_absolute_age, highest_relative_age);
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
    analyze_eviction_sequence();
}
