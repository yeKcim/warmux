#!/usr/bin/gnuplot
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set terminal png


set output "daily.png"
set format x "%d/%m"
plot "daily.csv" using 1:($3+$4) title "players per day" with lines, "hourly.csv" using 1:3 title "games per day" with lines

set output "hourly.png"
set format x "%H:%M"
plot "hourly.csv" using 1:($3+$4) title "players per hour" with lines, "hourly.csv" using 1:3 title "games per hour" with lines

