original_signal_0 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_0.txt")
filtered_signal_1 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Output_Data/kalmanFilter/signal_filter_0.txt")

index             <- original_signal_0[,1]
original_signal_0 <- original_signal_0[,2]
filtered_signal_1 <- filtered_signal_1[,2]


plot(index,original_signal_0, pch=16, xlab="index", ylim=c(-50,50), ylab=" signal ", type="l", col="blue", main="Initial Signal")

lines(index,filtered_signal_1, pch=16,  "l", col="red")


