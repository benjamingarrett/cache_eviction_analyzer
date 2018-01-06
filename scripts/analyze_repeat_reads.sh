output_directory=../../misc_phd/output/logs
results_directory=../../misc_phd/output/results/nru
rm $results_directory/repeat_reads
#../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nri_clock_repeat_reads_001 >> $results_directory/repeat_reads
#../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nri_clock_repeat_reads_002 >> $results_directory/repeat_reads
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nru_clock_absolute_repeat_reads_001 >> $results_directory/repeat_reads
#../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nru_clock_absolute_repeat_reads_002 >> $results_directory/repeat_reads
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nru_clock_relative_repeat_reads_001 >> $results_directory/repeat_reads
#../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/nru_clock_relative_repeat_reads_002 >> $results_directory/repeat_reads
