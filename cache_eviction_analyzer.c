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

const char * result_sequence_param = "--cea_result_sequence";
const char * cea_execution_trace_fname_parameter = "--cea_execution_trace_fname";
const char * cea_cumulative_results_fname_parameter = "--cea_cumulative_results_fname";
const char * cea_algorithm_parameter = "--cea_algorithm";
const char * cea_cache_size_parameter = "--cea_cache_size";
const char * cea_k_timestamps_parameter = "--cea_k_timestamps";
const char * cea_d_recent_timestamps_parameter = "--cea_d_recent_timestamps";
const char * cea_a_items_per_timestamp_parameter = "--cea_a_items_per_timestamp";
const char * cea_operation_sequence_parameter = "--cea_operation_sequence_description";
const char * cea_freshness_expectation_parameter = "--cea_freshness_expectation";


char cea_execution_trace_fname[400];
FILE * fp, * log_fp;

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
  int64_t m = pc->highest_timestamp + 2;
  log_fp = fopen(cea_execution_trace_fname, "a");
  fprintf(log_fp, "\n====pseudo cache====\n");
  int64_t g;
  for(g = 0; g <= m; g++){
    fprintf(fp, "t[%ld]=%ld\tk[%ld]=%ld\n", g, pc->timestamp[g], g, pc->key[g]);
  }
  fprintf(fp, "====\n");
  fclose(log_fp);
}

int64_t is_empty_fast(int64_t * x){
  return (x == NULL) ? 1 : 0;
}

int64_t is_empty_slow(int64_t * x){
  return (*x == PSEUDO_CACHE_NON_VALUE) ? 1 : 0;
}

int64_t is_empty(int64_t * x){
  int64_t fast_result = is_empty_fast(x);
  int64_t slow_result = is_empty_slow(x);
  if(fast_result != slow_result){
    log_fp = fopen(cea_execution_trace_fname, "a");
    fprintf(log_fp, "cache_eviction_analyzer.is_empty fast and slow caches gave different results %ld & %ld\n",
         fast_result, slow_result);
    fclose(log_fp);
    fprintf(stderr, "cache_eviction_analyzer.is_empty fast and slow caches gave different results %ld & %ld\n",
         fast_result, slow_result);
    exit(1);
  }
  return fast_result;
}

void set_timestamp_fast(int64_t k, int64_t t){
  cache_write_long_int(&k, &t);
  skiplist_write_long_int(&t, &k);
}

void set_timestamp_slow(int64_t k, int64_t t){
  pseudo_cache_t * pc = get_pseudo_cache();
  pc->key[t] = k;
  pc->timestamp[k] = t;
  pc->highest_timestamp = (pc->highest_timestamp < t) ? t : pc->highest_timestamp;
  pc->largest_key = (pc->largest_key < k) ? k : pc->largest_key;
}

void set_timestamp(int64_t k, int64_t t){
  set_timestamp_fast(k, t);
  set_timestamp_slow(k, t);
}

int64_t * get_timestamp_fast(int64_t k){
  int64_t * t = cache_read_long_int(&k);
  return t;
}

int64_t * get_timestamp_slow(int64_t k){
  pseudo_cache_t * pc = get_pseudo_cache();
  return &(pc->timestamp[k]);
}

int64_t * get_timestamp(int64_t k){
  int64_t * fast_result = get_timestamp_fast(k);
  int64_t * slow_result = get_timestamp_slow(k);
  if(!is_empty_fast(fast_result) && !is_empty_slow(slow_result)){
    if(*fast_result != *slow_result){
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "cache_eviction_analyzer.get_timestamp fast and slow caches gave different results %ld & %ld\n",
           *fast_result, *slow_result);
      fclose(log_fp);
      fprintf(stderr, "cache_eviction_analyzer.get_timestamp fast and slow caches gave different results %ld & %ld\n",
           *fast_result, *slow_result);
      exit(1);
    }
  }
  return fast_result;
}

int64_t * get_key_fast(int64_t t){
  int64_t * k = skiplist_read_long_int(&t);
  return k;
}

int64_t * get_key_slow(int64_t t){
  pseudo_cache_t * pc = get_pseudo_cache();
  return &(pc->key[t]);
}

int64_t * get_key(int64_t t){
  int64_t * fast_result = get_key_fast(t);
  int64_t * slow_result = get_key_slow(t);
  if(*fast_result != *slow_result){
    log_fp = fopen(cea_execution_trace_fname, "a");
    fprintf(log_fp, "cache_eviction_analyzer.get_key fast and slow caches gave different results %ld & %ld\n",
         *fast_result, *slow_result);
    fclose(log_fp);
    fprintf(stderr, "cache_eviction_analyzer.get_key fast and slow caches gave different results %ld & %ld\n",
         *fast_result, *slow_result);
    exit(1);
  } 
  return fast_result;
}

void evict_key_fast(int64_t * k){
  skiplist_delete_long_int(get_timestamp(*k));
  cache_delete_long_int(k);
}

void evict_key_slow(int64_t * k){
  pseudo_cache_t * pc = get_pseudo_cache();
  pc->key[pc->timestamp[*k]] = PSEUDO_CACHE_NON_VALUE;
  pc->timestamp[*k] = PSEUDO_CACHE_NON_VALUE;
}

void evict_key(int64_t * k){
  evict_key_fast(k);
  evict_key_slow(k);
}

int64_t absolute_age(age_t ages, int64_t k){
  return ages.current_time - *get_timestamp_slow(k) + 1;
}

int64_t relative_age_fast(age_t ages, int64_t k){
  return size_of_long_int() - index_of_long_int(&k);
}

int64_t relative_age_slow(age_t ages, int64_t k){
  int64_t d = ages.current_time;
  int64_t a = 1;
  while(d>=0){
    if(!is_empty_slow(get_key_slow(d))){
      if(*get_key_slow(d)==k){
        return a;
      }
      a++;
    }
    d--;
  }
  return a;
}

int64_t relative_age(age_t ages, int64_t k){
  int64_t fast_result = relative_age_fast(ages, k);
  int64_t slow_result = relative_age_slow(ages, k);
  if(fast_result != slow_result){
    log_fp = fopen(cea_execution_trace_fname, "a");
    fprintf(log_fp, "cache_eviction_analyzer.relative_age fast and slow caches gave different results %ld & %ld\n",
         fast_result, slow_result);
    fclose(log_fp);
    fprintf(stderr, "cache_eviction_analyzer.relative_age fast and slow caches gave different results %ld & %ld\n",
         fast_result, slow_result);
    exit(1);
  }
  return fast_result;
}


//=================================================
void process_insertion(age_t * ages, int64_t new_key){
  if(new_key!=-1){
    if(is_empty_slow(get_timestamp_slow(new_key))){
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "    inserting key %ld\n", new_key);
      fclose(log_fp);
      set_timestamp_slow(new_key, ages->current_time);
      ages->current_time++;
    } else {
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "    process_insertion, new_key %ld already in cache\nabort\n", new_key);
      fclose(log_fp);
      fprintf(stderr, "process_insertion, new_key %ld already in cache\n", new_key);
      exit(1);
    }
  }
}

void process_eviction(age_t * ages, int64_t evicted_key){
  if(evicted_key!=-1){
    if(!is_empty_slow(get_timestamp_slow(evicted_key))){
      int64_t abs_age = absolute_age(*ages,evicted_key);
      int64_t rel_age = relative_age_slow(*ages,evicted_key);
      int64_t prev_abs = ages->min_absolute_access;
      int64_t prev_rel = ages->min_relative_access;
      update_min_bound(&(ages->min_absolute_access), abs_age);
      update_min_bound(&(ages->min_relative_access), rel_age);
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp,"    evicting key %ld  previous min absolute age %ld  new absolute age %ld  previous min relative age %ld  new relative age %ld\n", 
                                evicted_key, prev_abs, abs_age, prev_rel, rel_age);
      fclose(log_fp);
      evict_key_slow(&evicted_key);
    } else {
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "    process_eviction, evicted_key %ld not in cache\nabort\n", evicted_key);
      fclose(log_fp);
      view_hashtable_long_int();
      fprintf(stderr, "process_eviction, evicted_key %ld not in cache\n", evicted_key);
      exit(1);
    }
  }
}

void process_timestamp_update(age_t * ages, int64_t updated_key, int64_t new_time){
  if(updated_key!=-1){
    log_fp = fopen(cea_execution_trace_fname, "a");
    fprintf(log_fp,"    update key %ld to time %ld (current time %ld)\n", updated_key, new_time, ages->current_time);
    fclose(log_fp);
    if(!is_empty_slow(get_timestamp_slow(updated_key))){
      evict_key_slow(&updated_key);
      set_timestamp_slow(updated_key,new_time);
    } else {
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "    process_timestamp_update, updated_key %ld not in cache\nabort\n", updated_key);
      fclose(log_fp);
      fprintf(stderr, "process_timestamp_update, updated_key %ld not in cache\n", updated_key);
      exit(1);
    }
  }
}

void process_usage(age_t * ages, int64_t updated_key){
  if(updated_key!=-1){
    int64_t old_time = *get_timestamp_slow(updated_key);
    log_fp = fopen(cea_execution_trace_fname, "a");
    fprintf(log_fp,"    update key %ld from time %ld to current time %ld\n", updated_key, old_time, ages->current_time);
    fclose(log_fp);
    if(!is_empty_slow(get_timestamp_slow(updated_key))){
      evict_key_slow(&updated_key);
      set_timestamp_slow(updated_key, ages->current_time);
      ages->current_time++;
    } else {
      log_fp = fopen(cea_execution_trace_fname, "a");
      fprintf(log_fp, "    process_usage, updated_key %ld not in cache\nabort\n", updated_key);
      fclose(log_fp);
      fprintf(stderr, "process_usage, updated_key %ld not in cache\n", updated_key);
      exit(1);
    }
  }
}

void process_input_line(age_t * ages, int64_t op, int64_t event, int64_t new_key, int64_t old_key, 
                             int64_t result, int64_t new_time, int64_t old_time){
  if(result == 1){
    switch(event){
      case INSERTION:
        log_fp = fopen(cea_execution_trace_fname, "a");
        fprintf(log_fp, "  processing insertion of new key %ld  min absolute access age %ld  min relative access age %ld  current time %ld\n", 
            new_key, ages->min_absolute_access, ages->min_relative_access, ages->current_time);
        fclose(log_fp);
        process_insertion(ages, new_key);
        break;
      case EVICTION:
        log_fp = fopen(cea_execution_trace_fname, "a");
        fprintf(log_fp, "  processing eviction of old_key %ld  min absolute access age %ld  min relative access age %ld  current time %ld\n", 
            old_key, ages->min_absolute_access, ages->min_relative_access, ages->current_time);
        fclose(log_fp);
        process_eviction(ages, old_key);
        break;
      case TIMESTAMP_UPDATE:
        log_fp = fopen(cea_execution_trace_fname, "a");
        fprintf(log_fp, "  processing timestamp update of old_key %ld  to new time %ld  min absolute access age %ld  min relative access age %ld  current time %ld\n", 
            old_key, new_time, ages->min_absolute_access, ages->min_relative_access, ages->current_time);
        fclose(log_fp);
        process_timestamp_update(ages, old_key, new_time);
        break;
      case USAGE:
        log_fp = fopen(cea_execution_trace_fname, "a");
        fprintf(log_fp, "  processing usage of old_key %ld  min absolute access age %ld  min relative access age %ld  current time %ld\n", 
            old_key, ages->min_absolute_access, ages->min_relative_access, ages->current_time);
        fclose(log_fp);
        process_usage(ages, old_key);
        break;
      default:
        fprintf(stderr, "Encountered unknown event type %ld\n", event);
        exit(1);
    }
  } else {
    printf("  ignoring op %ld  event %ld  new key %ld  old key %ld\n", op, event, new_key, old_key);
  }
}

void cache_eviction_analyzer(int argc, char ** argv){
  int64_t g;
  int found_log_file = 0;
  char algorithm[20];
  int64_t cache_size, k, d;
  double a, freshness_expectation;
  char operation_sequence_description[100];
  char cumulative_results_fname[400];
  char result_sequence_fname[400];
  for(g = 1; g < argc; g++){
    if(strcmp(argv[g], cea_execution_trace_fname_parameter) == 0){
      if(g+1 < argc){
        strcpy(cea_execution_trace_fname, &argv[++g][0]);
        printf("cea_execution_trace_fname: %s\n", cea_execution_trace_fname);
        found_log_file = 1;
      }
    }
    if(strcmp(argv[g], result_sequence_param) == 0){
      if(g+1 < argc){
        strcpy(result_sequence_fname, &argv[++g][0]);
      }
    }
    if(strcmp(argv[g], cea_cumulative_results_fname_parameter) == 0){
      if(g+1 < argc){
        strcpy(cumulative_results_fname, &argv[++g][0]);
      }
    }
    if(strcmp(argv[g], cea_algorithm_parameter) == 0){
      if(g+1 < argc){
        strcpy(algorithm, &argv[++g][0]);
      }
    }
    if(strcmp(argv[g], cea_cache_size_parameter) == 0){
      if(g+1 < argc){
        cache_size = (int64_t)atoi(argv[++g]);
      }
    }
    if(strcmp(argv[g], cea_k_timestamps_parameter) == 0){
      if(g+1 < argc){
        k = (int64_t)atoi(argv[++g]);
      }
    }
    if(strcmp(argv[g], cea_d_recent_timestamps_parameter) == 0){
      if(g+1 < argc){
        d = (int64_t)atoi(argv[++g]);
      }
    }
    if(strcmp(argv[g], cea_a_items_per_timestamp_parameter) == 0){
      if(g+1 < argc){
        a = (double)atof(argv[++g]);
      }
    }
    if(strcmp(argv[g], cea_operation_sequence_parameter) == 0){
      if(g+1 < argc){
        strcpy(operation_sequence_description, &argv[++g][0]);
      }
    }
    if(strcmp(argv[g], cea_freshness_expectation_parameter) == 0){
      if(g+1 < argc){
        freshness_expectation = (double)atof(argv[++g]);
      }
    }
  }
  if(! found_log_file){
    fprintf(stderr, "cache_eviction_analyzer: Please set the log file in cache_eviction_analyzer using parameter %s. Aborting.\n",
        cea_execution_trace_fname_parameter);
    exit(1);
  }
  FILE * fp2;
  fp2 = fopen(cea_execution_trace_fname, "a");
  fprintf(fp2, "cache_eviction_analyzer\n");
  fprintf(fp2, "analyzing: %s\n", result_sequence_fname);
  fclose(fp2);

  age_t ages;
  initialize_ages(&ages);
  pseudo_cache_t access_times;

  initialize_pseudo_cache(&access_times);
  set_pseudo_cache(&access_times);

  initialize_skiplist_long_int();
  initialize_long_int_cache(argc, argv);

  fp = fopen(result_sequence_fname, "r");
  if(fp == NULL){
    fprintf(stderr, "Failed to open file %s\n", result_sequence_fname);
    exit(1);
  }
  int64_t n;
  int64_t line_cnt = 1;
  char buf[LINE_BUF_LEN];
  int64_t op, event, new_key, old_key, result, new_time, old_time;
  while((n = fscanf(fp, "%s", buf)) != EOF){
    fp2 = fopen(cea_execution_trace_fname, "a"); fprintf(fp2,"Processing >%s<\n", buf); fclose(fp2);
    if(strcmp("OPERATIONS", buf) == 0){
      while((n = fscanf(fp, "%ld %ld %ld %ld %ld %ld %ld",
                &op, &event, &new_key, &old_key, &result, &new_time, &old_time)) != EOF){
        fp2 = fopen(cea_execution_trace_fname, "a");
        fprintf(fp2, "%ld items read\n", n);
        fprintf(fp2, "(%ld) event %ld new_key %ld old_key %ld new_time %ld old_time %ld -- [%ld,%ld]\n",
              line_cnt, event, new_key, old_key, new_time, old_time, 
              ages.min_absolute_access, ages.min_relative_access);
        fclose(fp2);
        process_input_line(&ages, op, event, new_key, old_key, result, new_time, old_time);
        fp2 = fopen(cea_execution_trace_fname, "a");
        fprintf(fp2, "\n");
        fclose(fp2);
        line_cnt++;
      }
      fp2 = fopen(cea_execution_trace_fname, "a");
      fprintf(fp2, "Done reading lines\n");
      fclose(fp2);
    }
  }
  fp2 = fopen(cea_execution_trace_fname, "a");
  fprintf(fp2, "access(abs,rel): [%ld,%ld]\n", ages.min_absolute_access, ages.min_relative_access);
  fclose(fp2);
  fclose(fp);
  printf("access(abs,rel): [%ld,%ld]\n", ages.min_absolute_access, ages.min_relative_access);
  fp2 = fopen(cumulative_results_fname, "a");
  fprintf(fp2, "%s,%ld,%ld,%ld,%f,%s,%f,%ld,%ld\n", 
       algorithm, cache_size, k, d, a, operation_sequence_description, 
       freshness_expectation,
       ages.min_absolute_access, ages.min_relative_access);
  fclose(fp2);
}

