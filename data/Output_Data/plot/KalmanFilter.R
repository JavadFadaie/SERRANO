original_signal_0 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_0.txt")
filtered_signal_1 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Output_Data/kalmanFilter/signal_filter_0.txt")

index_h             <- original_signal_0[,1]
original_signal_0 <- original_signal_0[,3]
filtered_signal_1 <- filtered_signal_1[,2]

start_index <- 0
end_index <- 100

index <- index_h[start_index:end_index]
original_signal <- original_signal_0[start_index:end_index]
filtered_signal <- filtered_signal_1[start_index:end_index]


jpeg("myplot.jpeg")

plot(index,original_signal, pch=8, xlab="index", ylab="signal value", type="l", col="blue", main="Kalman Filter")

lines(index,filtered_signal, pch=16,  "l", col="red")


legend("topright",legend=c( "original signal", "filtered signal"  ),
  text.col=c(),pch=c(16),col=c("blue", "red"))
dev.off()
