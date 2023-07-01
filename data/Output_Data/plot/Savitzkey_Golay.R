original_signal_0 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_21.txt")
filtered_signal_1 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Output_Data/SavitzkeyGolay/signal_filter_21.txt")

index             <- original_signal_0[,1]
original_signal_0 <- original_signal_0[,2]
filtered_signal_1 <- filtered_signal_1[,2]


plot(index,original_signal_0, pch=16, xlab="index", ylab=" signal ", type="l", col="blue", main="Savitzkey Golay Filter")

lines(index,filtered_signal_1, pch=16,  "l", col="red")


