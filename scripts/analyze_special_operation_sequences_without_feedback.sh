output_directory=../../misc_phd/output/logs
results_directory=../../misc_phd/output/results/nru
rm -f $results_directory/special

echo absolute_001_k4 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_001_k4 >> $results_directory/special
echo relative_001_k4 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_001_k4 >> $results_directory/special

echo absolute_001_k5 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_001_k5 >> $results_directory/special
echo relative_001_k5 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_001_k5 >> $results_directory/special

echo absolute_002_k4 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_002_k4 >> $results_directory/special
echo relative_002_k4 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_002_k4 >> $results_directory/special

echo absolute_002_k5 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_002_k5 >> $results_directory/special
echo relative_002_k5 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_002_k5 >> $results_directory/special

echo absolute_003_k8 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_003_k8 >> $results_directory/special
echo relative_003_k8 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_003_k8 >> $results_directory/special

echo absolute_003_k16 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_003_k16 >> $results_directory/special
echo relative_003_k16 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_003_k16 >> $results_directory/special

echo absolute_004_k8 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_004_k8 >> $results_directory/special
echo relative_004_k8 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_004_k8 >> $results_directory/special

echo absolute_004_k16 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_absolute_004_k16 >> $results_directory/special
echo relative_004_k16 >> $results_directory/special
../dist/Debug/GNU-Linux/cache_eviction_analyzer --result_sequence $output_directory/special_nru_clock_relative_004_k16 >> $results_directory/special
