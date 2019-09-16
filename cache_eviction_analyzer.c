#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../memoization/memo_long_int.h"
#include "../indexed_skiplist/indexed_skiplist_long_int.h"
#include "cache_eviction_analyzer.h"

#define READ 0
#define WRITE 1
#define DELETE 2
#define EVICTION 3
#define INSERTION 4
#define REBUILD 5
#define TIMESTAMP_UPDATE 6
#define USAGE 7

#define LINE_BUF_LEN 200

#define FAST_CACHE  
//#define SLOW_CACHE  
//#define COMPARE_CACHES  

const char * result_sequence_param = "--cea_result_sequence";
const char * cea_execution_trace_fname = "cache_eviction_analyzer_execution_trace.log";
FILE * fp;

typedef struct {
  int64_t min_absolute_access;
  int64_t min_relative_access;
  int64_t current_time;
} age_t;
void initialize_ages(age_t * a){
  a->min_absolute_access = LONG_MAX;
  a->min_relative_access = LONG_MAX;
  a->current_time = 1;
}
void update_min_bound(int64_t * min, int64_t val){
  *min = (val < *min) ? val : *min;
}


/*========================PSEUDO CACHE==========================*/
const long int PSEUDO_CACHE_CAPACITY = 30000000;
const long int PSEUDO_CACHE_NON_VALUE = -1;
typedef struct {
  int64_t * key;
  int64_t * timestamp;
  int64_t largest_key;
  int64_t highest_timestamp;
  int64_t capacity;
} pseudo_cache_t;
void initialize_pseudo_cache(pseudo_cache_t * pc){
  pc->key = calloc(PSEUDO_CACHE_CAPACITY, sizeof(int64_t));
  pc->timestamp = calloc(PSEUDO_CACHE_CAPACITY, sizeof(int64_t));
  pc->largest_key = PSEUDO_CACHE_NON_VALUE;
  pc->highest_timestamp = PSEUDO_CACHE_NON_VALUE;
  int64_t g;
  for(g = 0; g < PSEUDO_CACHE_CAPACITY; g++){
    pc->key[g] = PSEUDO_CACHE_NON_VALUE;
    pc->timestamp[g] = PSEUDO_CACHE_NON_VALUE;
  }
  pc->capacity = PSEUDO_CACHE_CAPACITY;
}
pseudo_cache_t * pseudo_cache;
void set_pseudo_cache(pseudo_cache_t * pc){ pseudo_cache = pc; }
pseudo_cache_t * get_pseudo_cache(){ return pseudo_cache; }
void view_pseudo_cache(){
  pseudo_cache_t * pc = get_pseudo_cache();
  int64_t m = pc->highest_timestamp+2;
  fp=fopen(cea_execution_trace_fname,"a");
  fprintf(fp,"\n====pseudo cache====\n");
  int64_t g;
  for(g = 0; g <= m; g++){
    fprintf(fp,"t[%ld]=%ld\tk[%ld]=%ld\n", g, pc->timestamp[g], g, pc->key[g]);
  }
  fprintf(fp,"====\n");
  fclose(fp);
}
void set_timestamp(int64_t k, int64_t t){
  #ifdef SLOW_CACHE
  pseudo_cache_t * pc = get_pseudo_cache();
  pc->key[t] = k;
  pc->timestamp[k] = t;
  pc->highest_timestamp = (pc->highest_timestamp < t) ? t : pc->highest_timestamp;
  pc->largest_key = (pc->largest_key < k) ? k : pc->largest_key;
  #endif
  #ifdef FAST_CACHE
  cache_write_long_int(&k,&t);
  skiplist_write_long_int(&t,&k);
  #endif
}
int64_t * get_timestamp(int64_t k){
  #ifdef SLOW_CACHE
  pseudo_cache_t * pc = get_pseudo_cache();
  return &(pc->timestamp[k]);
  #endif
  #ifdef FAST_CACHE
  int64_t * t = cache_read_long_int(&k);
  return t;
  #endif
}
int64_t * get_key(int64_t t){
  #ifdef SLOW_CACHE
  pseudo_cache_t * pc = get_pseudo_cache();
  return &(pc->key[t]);
  #endif
  #ifdef FAST_CACHE
  int64_t * k = skiplist_read_long_int(&t);
  return k;
  #endif
}
int64_t is_empty(int64_t * x){
  #ifdef SLOW_CACHE
  return (*x == PSEUDO_CACHE_NON_VALUE) ? 1 : 0;
  #endif
  #ifdef FAST_CACHE
  return (x == NULL) ? 1 : 0;
  #endif
}
void evict_key(int64_t * k){
  #ifdef SLOW_CACHE
  pseudo_cache_t * pc = get_pseudo_cache();
  pc->key[pc->timestamp[k]] = PSEUDO_CACHE_NON_VALUE;
  pc->timestamp[k] = PSEUDO_CACHE_NON_VALUE;
  #endif
  #ifdef FAST_CACHE
  skiplist_delete_long_int(get_timestamp(*k));
  cache_delete_long_int(k);  
  #endif
}
int64_t absolute_age(age_t ages, int64_t k){
  return ages.current_time - *get_timestamp(k);
}
int64_t relative_age(age_t ages, int64_t k){
  #ifdef SLOW_CACHE
  int64_t d = ages.current_time;
  int64_t a = 1;
  while(d>=0){
    if(!is_empty(get_key(d))){
      if(*get_key(d)==k){
        return a;
      }
      a++;
    }
    d--;
  }
  return a;
  #endif
  #ifdef FAST_CACHE
  return size_of_long_int() - index_of_long_int(&k) - 1;
  #endif
}
//=================================================
void process_insertion(age_t * ages, int64_t new_key){
  if(new_key!=-1){
    if(is_empty(get_timestamp(new_key))){
      set_timestamp(new_key, ages->current_time);
      ages->current_time++;
    } else {
      fprintf(stderr, "key %ld already in cache\n", new_key);
      exit(1);
    }
  }
}
void process_eviction(age_t * ages, int64_t evicted_key){
  if(evicted_key!=-1){
    if(!is_empty(get_timestamp(evicted_key))){
      update_min_bound(&(ages->min_absolute_access), absolute_age(*ages,evicted_key));
      update_min_bound(&(ages->min_relative_access), relative_age(*ages,evicted_key));
      fp=fopen(cea_execution_trace_fname,"a");
      fprintf(fp,"evicting key %ld\n", evicted_key);
      fclose(fp);
      evict_key(&evicted_key);
    } else {
      fprintf(stderr, "key %ld not in cache\n", evicted_key);
      exit(1);
    }
  }
}
void process_timestamp_update(age_t * ages, int64_t updated_key, int64_t new_time){
  if(updated_key!=-1){
    fp=fopen(cea_execution_trace_fname,"a");
    fprintf(fp,"update key %ld to time %ld (current time %ld)\n", updated_key, new_time, ages->current_time);
    fclose(fp);
    if(!is_empty(get_timestamp(updated_key))){
      evict_key(&updated_key);
      set_timestamp(updated_key,new_time);
    } else {
      fprintf(stderr, "process_timestamp_update, key %ld not in cache\n", updated_key);
      exit(1);
    }
  }
}
void process_usage(age_t * ages, int64_t updated_key){
  if(updated_key!=-1){
    int64_t old_time = *get_timestamp(updated_key);
    fp=fopen(cea_execution_trace_fname,"a");
    fprintf(fp,"update key %ld from time %ld to current time %ld\n", updated_key, old_time, ages->current_time);
    fclose(fp);
    if(!is_empty(get_timestamp(updated_key))){
      evict_key(&updated_key);
      set_timestamp(updated_key, ages->current_time);
      ages->current_time++;
    } else {
      fprintf(stderr, "process_usage, key %ld not in cache\n", updated_key);
      exit(1);
    }
  }
}
void process_input_line(age_t * ages, int64_t op, int64_t event, int64_t new_key, int64_t old_key, 
                                                 int64_t result, int64_t new_time, int64_t old_time){
  if(result == 1){
    switch(event){
      case INSERTION:
        process_insertion(ages, new_key);
        break;
      case EVICTION:
        process_eviction(ages, old_key);
        break;
      case TIMESTAMP_UPDATE:
        process_timestamp_update(ages, old_key, new_time);
        break;
      case USAGE:
        process_usage(ages, old_key);
        break;
      default:
        fprintf(stderr, "Encountered unknown event type %ld\n", event);
        exit(1);
    }
  }
}
void cache_eviction_analyzer(int argc, char ** argv){
  FILE * fp2;
  fp2=fopen(cea_execution_trace_fname,"a");
  fprintf(fp2,"cache_eviction_analyzer\n");
  char result_sequence_fname[LINE_BUF_LEN];
  int64_t i;
  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], result_sequence_param) == 0){
      if(i + 1 < argc){
        strcpy(result_sequence_fname, &argv[++i][0]);
      }
    }
  }
  age_t ages;
  initialize_ages(&ages);
  pseudo_cache_t access_times;
  initialize_pseudo_cache(&access_times);
  set_pseudo_cache(&access_times);
  initialize_skiplist_long_int();
  initialize_long_int_cache(argc,argv);
  fp = fopen(result_sequence_fname, "r");
  if(fp == NULL){
    fprintf(stderr, "Failed to open file %s\n", result_sequence_fname);
    exit(1);
  }
  int64_t n;
  char buf[LINE_BUF_LEN];
  while((n = fscanf(fp, "%s", buf)) != EOF){
    fprintf(fp2,"Processing >%s<\n", buf);
    if(strcmp("OPERATIONS", buf) == 0){
      int64_t op, event, new_key, old_key, result, new_time, old_time;
      while((n = fscanf(fp, "%ld %ld %ld %ld %ld %ld %ld\n",
              &op, &event, &new_key, &old_key, &result, &new_time, &old_time)) != EOF){
        fprintf(fp2,"event %ld new_key %ld old_key %ld new_time %ld old_time %ld -- [%ld,%ld]\n",
              event, new_key, old_key, new_time, old_time, 
              ages.min_absolute_access, ages.min_relative_access);
        process_input_line(&ages, op, event, new_key, old_key, result, new_time, old_time);
      }
    }
  }
  fprintf(fp2,"access(abs,rel): [%ld,%ld]\n", ages.min_absolute_access, ages.min_relative_access);
  fclose(fp);
  fclose(fp2);
}

