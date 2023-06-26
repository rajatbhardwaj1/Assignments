set terminal png 
set output "Task 1.png"
set title "Congestion Window Calculation"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window (cwnd)"
plot "task1.cwnd" using 1:3 with lines title "Congestion1"