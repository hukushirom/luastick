all:
	gcc -Wall -W -g -lmba -DMSGNO -o errcmp errcmp.c
	./errcmp errno_SUSv3.tsv errno_aix51.tsv errno_hpux1122.tsv errno_sol9.tsv errno_linux2x.tsv errno_irix65.tsv errno_osf1.tsv errno_fbsd521.tsv errno_osx103.tsv errno_msvc6.tsv > /tmp/out.tsv
