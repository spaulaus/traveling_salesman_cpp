reset
set terminal wxt
unset key
set xrange[0:40]

plot 'test.dat' u 2:3:4:5 w vector, 'test.dat' u 2:3 w points pt 4,\
     'salesman.dat' w points pt 2