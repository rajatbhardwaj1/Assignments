set terminal png 
set output "Task 3.png"
set title "Congestion Window Calculation"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window (cwnd)"
set style line 1 lc rgb '#ff0000' lt 1 lw 2 pt 7 ps 0.05


plot "task3.cwnd" using 1:3 with lines ls 1 title "Congestion3" 


 
