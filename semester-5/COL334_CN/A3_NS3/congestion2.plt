set terminal png 
set output "Task 2.png"
set title "Congestion Window Calculation"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window (cwnd)"
set terminal png size 1400 , 400
plot "task2.cwnd" using 1:3 with lines title "Congestion1"