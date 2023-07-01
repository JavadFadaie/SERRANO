                                        #
library(stringr)

pdf("Top500-Performance.pdf",width = 6, height = 5, pointsize=9)

par(mar=c(5,7,2,2),family="sans",font=3,font.axis=3,font.lab=3,font.main=3)

data<-read.table(file="Top2.csv")
data<-data.frame(data,as.Date(paste(data[,1],str_pad(data[,2], 2, pad = "0"),"01", sep="/"),"%Y/%m/%d"))
##print(data)
names(data)<-c("Year","Month","Year","Sum","No.1","No.500","Date")

# Plot --------------------------------------------------------------
plot(data[,c(7,4)],log="y",xlab="",ylab="", ylim=c(10e07, 10e09),axes=FALSE,pch=21,
     bg="red",cex=0.7,lty=1,type="o",cex.lab=4.4, cex.axis=4.4, cex.main=4.4, cex.sub=4.4)
par(new=TRUE)
plot(data[,c(7,5)],log="y",xlab="",ylab="", ylim=c(10e07, 10e09),axes=FALSE,pch=22,
     bg="green",cex=0.7,lty=1,type="o",cex.lab=4.4, cex.axis=4.4, cex.main=4.4, cex.sub=4.4)
par(new=TRUE)
plot(data[,c(7,6)],log="y",xlab="",ylab="", ylim=c(10e07, 10e09),axes=FALSE,pch=23,
     bg="blue",cex=0.7,lty=1,type="o",cex.lab=4.4, cex.axis=4.4, cex.main=4.4, cex.sub=4.4)

# x-axis -------------------------------------------------------------
axis.Date(1,at=data[,7],cex.axis=1.25)

# y-axis -------------------------------------------------------------
axis(2,
   at  =c(0.1,          
          1          ,10          ,100          ,
          1000       ,10000       ,100000       ,
          1000000    ,10000000    ,100000000    ,
          1000000000 ,10000000000) ,
   labels=c("100 MFlop/s",
          "1 GFlop/s","10 GFlop/s","100 GFlop/s",
          "1 TFlop/s","10 TFlop/s","100 TFlop/s",
          "1 PFlop/s","10 PFlop/s","100 MFlop/s",
          "1 GFlop/s","10 GFlop/s"),
   las=2, pos=8500, cex.axis=1.00, cex.lab=1.25)

title(xlab="Number of Processors", cex.lab=1.25)
title(ylab="Performance", line=5, cex.lab=1.25)

legend("topleft",legend=c("Flops","Read Bandwidth","Total Bandwidth"),
  text.col=c("red","green","blue"),pch=c(21,22,23),col=c("red","green","blue"))

#abline(lm(log10(data[c(1:37),4])~data[c(1:37),7]),col="red",lwd=1,lty=1)
#abline(lm(log10(data[c(38:56),4])~data[c(38:56),7]),col="red",lwd=1,lty=2)
#abline(lm(log10(data[c(1:37),6])~data[c(1:37),7]),col="blue",lwd=1,lty=1)
#abline(lm(log10(data[c(38:56),6])~data[c(38:56),7]),col="blue",lwd=1,lty=2)

#abline(v=15210,col="gray",lty=2)

legend(x=11600,y=10.6E2,legend=c("Sum","#1","#500",
       "Regression Sum: Jun. 1993 - Jun. 2011",
       "Regression Sum: Nov. 2011 - Nov. 2020",
       "Regression #500: Jun. 1993 - Jun. 2011",
       "Regression #500: Nov. 2011 - Nov. 2020" ),
       pch=c(21,22,23,1,1,1,1),lty=c(1,1,1,1,2,1,2),pt.bg=c("red","green","blue",
       "red","red","blue","blue"),pt.cex=c(1,1,1,0,0,0,0),
       col=c(1,1,1,"red","red","blue","blue"),cex=1.15)

dev.off()

