#!/usr/bin/env bash

total_sizes=($(seq 48 16 1024))
gcc seq.c -ggdb3 -pthread -lpmem || exit
T=19

echo "T=$T," >seq.csv
echo "granularity,read(GB),read(GB/s),write(GB),write(GB/s),elapsed(s)" >>seq.csv

for t in "${total_sizes[@]}"; do
  rm -rf /pmem0/*
  taskset -c 0-$T ./a.out "$t" $T
done

#/mnt/sdb/xiaoxiang/linux/tools/perf stat -e \
#uncore_imc_0/unc_m_pmm_rpq_inserts/,\
#uncore_imc_0/unc_m_pmm_wpq_inserts/,\
#uncore_imc_1/unc_m_pmm_rpq_inserts/,\
#uncore_imc_1/unc_m_pmm_wpq_inserts/,\
#uncore_imc_2/unc_m_pmm_rpq_inserts/,\
#uncore_imc_2/unc_m_pmm_wpq_inserts/,\
#uncore_imc_3/unc_m_pmm_rpq_inserts/,\
#uncore_imc_3/unc_m_pmm_wpq_inserts/,\
#uncore_imc_4/unc_m_pmm_rpq_inserts/,\
#uncore_imc_4/unc_m_pmm_wpq_inserts/,\
#uncore_imc_5/unc_m_pmm_rpq_inserts/,\
#uncore_imc_5/unc_m_pmm_wpq_inserts/,\
#uncore_imc_6/unc_m_pmm_rpq_inserts/,\
#uncore_imc_6/unc_m_pmm_wpq_inserts/,\
#uncore_imc_7/unc_m_pmm_rpq_inserts/,\
#uncore_imc_7/unc_m_pmm_wpq_inserts/ \
#$1 $2
