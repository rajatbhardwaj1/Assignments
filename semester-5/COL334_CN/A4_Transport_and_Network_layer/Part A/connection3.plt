set terminal png 
set output "task1plots/connection3.png"
set title "Connection 3"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window (cwnd)"
set style line 1 lc rgb '#ff0000' lt 1 lw 2 pt 7 ps 0.05


plot "connection3.cwnd" using 1:3 with lines ls 1 title "cw" 


 
