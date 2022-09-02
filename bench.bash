#!/usr/bin/env bash

/home/blepers/linux-huge/tools/perf/perf stat -e \
uncore_imc_0/event=0xE3/,\
uncore_imc_0/event=0xE3/,\
uncore_imc_1/event=0xE3/,\
uncore_imc_1/event=0xE3/,\
uncore_imc_2/event=0xE3/,\
uncore_imc_2/event=0xE3/,\
uncore_imc_3/event=0xE3/,\
uncore_imc_3/event=0xE3/,\
uncore_imc_4/event=0xE3/,\
uncore_imc_4/event=0xE3/,\
uncore_imc_5/event=0xE3/,\
uncore_imc_5/event=0xE3/,\
uncore_imc_6/event=0xE3/,\
uncore_imc_6/event=0xE3/,\
uncore_imc_7/event=0xE3/,\
uncore_imc_7/event=0xE3/ \
$1 $2