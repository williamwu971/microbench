#!/usr/bin/env bash

/home/blepers/linux-huge/tools/perf/perf stat -e \
uncore_imc_0/unc_m_pmm_rpq_inserts/,\
uncore_imc_0/unc_m_pmm_wpq_inserts/,\
uncore_imc_1/unc_m_pmm_rpq_inserts/,\
uncore_imc_1/unc_m_pmm_wpq_inserts/,\
uncore_imc_2/unc_m_pmm_rpq_inserts/,\
uncore_imc_2/unc_m_pmm_wpq_inserts/,\
uncore_imc_3/unc_m_pmm_rpq_inserts/,\
uncore_imc_3/unc_m_pmm_wpq_inserts/,\
uncore_imc_4/unc_m_pmm_rpq_inserts/,\
uncore_imc_4/unc_m_pmm_wpq_inserts/,\
uncore_imc_5/unc_m_pmm_rpq_inserts/,\
uncore_imc_5/unc_m_pmm_wpq_inserts/,\
uncore_imc_6/unc_m_pmm_rpq_inserts/,\
uncore_imc_6/unc_m_pmm_wpq_inserts/,\
uncore_imc_7/unc_m_pmm_rpq_inserts/,\
uncore_imc_7/unc_m_pmm_wpq_inserts/ \
$1 $2