

# obtain event log
p1=../../memo_batch_tester/dist/Debug/GNU-Linux/memo_batch_tester
ptp=--mbt_problem_type
pt=test
osp=--mbt_operation_sequence
os=../../misc_phd/input/operation_sequences/without_feedback/without_deletions/operation_sequence_without_feedback-30-30
csp=--memo_caching_strategy
cs=lru
czp=--memo_cache_size
cz=64
klp=--memo_key_length
kl=8
vlp=--memo_value_length
vl=8
ckp=--memo_cuckoo_k
ck=4
lqp=--memo_lru_queue_size
lq=3
elfp=--memo_event_log_fname
elf=event_log
$p1 $ptp $pt $osp $os $csp $cs $czp $cz $klp $kl $vlp $vl $ckp $ck $lqp $lq $elfp $elf

mv event_log event_log_

# check ages of evictions
p2=../dist/Debug/GNU-Linux/cache_eviction_analyzer
rsp=--cea_result_sequence
rs=event_log_
csp=--memo_caching_strategy
cs=linear_probe_with_deletions
czp=--memo_cache_size
cz=64
klp=--memo_key_length
kl=8
vlp=--memo_value_length
vl=8
$p2 $rsp $rs $csp $cs $czp $cz $klp $kl $vlp $vl
