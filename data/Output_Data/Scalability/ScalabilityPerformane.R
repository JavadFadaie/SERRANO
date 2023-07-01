
Proc <- c(1,2,4,8,16,24)


MPI_Elapsed <- c( 9.97172, 4.989, 2.41784, 1.22697 ,0.654328, 0.53174 )

#MPI_Flops <- c(  4.62107e+08, 9.23633e+08, 1.90584e+09, 3.75562e+09, 7.04237e+09, 8.66591e+09)
MPI_Flops <- c(  8.66474254726, 8.9654994412, 9.28008643773, 9.57468164316, 9.84771883879, 9.93781417435)


#MPI_rbytes <- c(  3.08071e+07, 6.15755e+07, 1.27056e+08,  2.50375e+08, 4.69491e+08, 5.77727e+08)
MPI_rbytes <- c(  7.4886508183, 7.78940794705,  8.10399517868,  8.3985909623, 8.67162727139, 8.76172266471)


MPI_bandwidth <- c(  7.96577254293, 8.26653037727, 8.58111757277, 8.87571163883, 9.14874760107, 9.23884366886)

plot(Proc,MPI_Elapsed, pch=15, xlab="", ylab="per sec", type="o",col="blue",xaxt="n", main="Elapsed time")
axis(1, at = seq(0, 24, by = 1), las=2)
mtext("Number of Procs  ",side=1,col="black",line=2.5)  


plot(Proc, MPI_Flops, pch=15, xlab="", ylab="log10 Flops", type="o",col="red", xaxt="n", main="Flops ")
axis(1, at = seq(0, 24, by = 1), las=2)
mtext("Number of Procs  ",side=1,col="black",line=2.5)  


plot(Proc, MPI_rbytes, pch=15, xlab="", ylab="log10 Read Bytes", type="o",col="blue", xaxt="n", main="Read Bytes")
axis(1, at = seq(0, 24, by = 1), las=2)
mtext("Number of Procs  ",side=1,col="black",line=2.5)

plot(Proc, MPI_bandwidth, pch=15, xlab="", ylab="log10 Bandwidth", type="o",col="red", xaxt="n", main="Bandwidth")
axis(1, at = seq(0, 24, by = 1), las=2)
mtext("Number of Procs  ",side=1,col="black",line=2.5)


