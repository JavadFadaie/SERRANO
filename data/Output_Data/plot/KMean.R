original_signal_0 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_0.txt")
original_signal_1 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_1.txt")
original_signal_2 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_2.txt")
original_signal_3 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_3.txt")
original_signal_4 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_4.txt")
original_signal_5 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_5.txt")
original_signal_6 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_6.txt")
original_signal_7 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_7.txt")
original_signal_8 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_8.txt")
original_signal_9 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_9.txt")
original_signal_10 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_10.txt")
original_signal_11 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_11.txt")
original_signal_12 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_12.txt")
original_signal_13 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_13.txt")
original_signal_14 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_14.txt")
original_signal_15 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_15.txt")
original_signal_16 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_16.txt")
original_signal_17 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_17.txt")
original_signal_18 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_18.txt")
original_signal_19 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_19.txt")
original_signal_20 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_20.txt")
original_signal_21 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_21.txt")
#original_signal_22 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_22.txt")
#original_signal_23 <- read.table("/home/javad/Desktop/Serrano/serrano/data/Input_Data/Double_Data_Type/signalFilter_plot/signal_23.txt")


index             <- original_signal_0[,1]
original_signal_0 <- original_signal_0[,2]
original_signal_1 <- original_signal_1[,2]
original_signal_2 <- original_signal_2[,2]
original_signal_3 <- original_signal_3[,2]
original_signal_4 <- original_signal_4[,2]
original_signal_5 <- original_signal_5[,2]
original_signal_6 <- original_signal_6[,2]
original_signal_7 <- original_signal_7[,2]
original_signal_8 <- original_signal_8[,2]
original_signal_9 <- original_signal_9[,2]
original_signal_10 <- original_signal_10[,2]
original_signal_11 <- original_signal_11[,2]
original_signal_12 <- original_signal_12[,2]
original_signal_13 <- original_signal_13[,2]
original_signal_14 <- original_signal_14[,2]
original_signal_15 <- original_signal_15[,2]
original_signal_16 <- original_signal_16[,2]
original_signal_17 <- original_signal_17[,2]
original_signal_18 <- original_signal_18[,2]
original_signal_19 <- original_signal_19[,2]
original_signal_20 <- original_signal_20[,2]
original_signal_21 <- original_signal_21[,2]
#original_signal_22 <- original_signal_22[,2]
#original_signal_23 <- original_signal_23[,2]


plot(index,original_signal_0, pch=16, xlab="index", ylim=c(-100,100), ylab=" signal ", type="l", col="red", main="Raw Position Data")

lines(index,original_signal_0, pch=16,  "l", col="grey")
lines(index,original_signal_1, pch=16,  "l", col="grey")
lines(index,original_signal_2, pch=16,  "l", col="grey")
lines(index,original_signal_3, pch=16,  "l", col="grey")
lines(index,original_signal_4, pch=16,  "l", col="grey")
lines(index,original_signal_5, pch=16,  "l", col="grey")
lines(index,original_signal_6, pch=16,  "l", col="grey")
lines(index,original_signal_7, pch=16,  "l", col="grey")
lines(index,original_signal_8, pch=16,  "l", col="grey")
lines(index,original_signal_9, pch=16,  "l", col="grey")
lines(index,original_signal_10, pch=16,  "l", col="grey")
lines(index,original_signal_11, pch=16,  "l", col="grey")
lines(index,original_signal_12, pch=16,  "l", col="grey")
lines(index,original_signal_13, pch=16,  "l", col="grey")
lines(index,original_signal_14, pch=16,  "l", col="grey")
lines(index,original_signal_15, pch=16,  "l", col="red")
lines(index,original_signal_16, pch=16,  "l", col="red")
lines(index,original_signal_17, pch=16,  "l", col="grey")
lines(index,original_signal_18, pch=16,  "l", col="grey")
lines(index,original_signal_19, pch=16,  "l", col="grey")
lines(index,original_signal_20, pch=16,  "l", col="grey")
lines(index,original_signal_21, pch=16,  "l", col="red")
#lines(index,original_signal_22, pch=16,  "l", col="red")
#lines(index,original_signal_23, pch=16,  "l", col="red")


