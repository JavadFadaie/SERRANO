original_signal_0 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_33.txt")
filtered_signal_1 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Output_Data/FFTFilter/signal_filter_0.txt")

index             <- original_signal_0[,1]
original_signal_0 <- original_signal_0[,2]
filtered_signal_1 <- filtered_signal_1[,2]

#plot(index,filtered_signal_1, pch=16, xlab="index", ylab=" Frequency signal ", type="l", col="red", main="FFT Transformation")
plot(index,original_signal_0, pch=16, xlab="time / s", ylab=" time series signal ", type="l", col="blue", main="Acceleration Signal")



