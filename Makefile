push:
	git commit -am "-AUTO- `date`" || true
	git push || true

download:
	#scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sda/xiaoxiang/RECIPE/P-Masstree/build/log_debug_print_*.txt' .
	#scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sdb/xiaoxiang/RECIPE/P-Masstree/build/*.png' .
#	scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sdb/xiaoxiang/RECIPE/P-Masstree/*.perf' .
#	scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sdb/xiaoxiang/RECIPE/P-Masstree/*.old' .
	#scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sdb/xiaoxiang/RECIPE/P-Masstree/build/*.csv' .
#	scp xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sda/xiaoxiang/RECIPE/P-Masstree/build/lookup.csv .
	scp 'xiaoxiang@labos2.cs.usyd.edu.au:/mnt/sdb/xiaoxiang/RECIPE/P-Masstree/build/*.png' .
