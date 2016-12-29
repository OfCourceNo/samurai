make \
	&& echo -n -e '\e[38;5;219m こ、これでどうかな ... ?\e[m' \
	&& read \
	&& valgrind --leak-check=full --error-exitcode=222 --log-file='.valgrind.log' ./a.out | tee ^a \
	&& [ `grep "ERROR SUMMARY: 0 errors" .valgrind.log | wc -l` -ne 1 ] \
	&& echo -n -e '\e[38;5;202m --- valgrind error --- \e[m' \
	&& read \
	&& cat .valgrind.log
exit 0
