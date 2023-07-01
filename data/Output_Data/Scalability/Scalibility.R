
Proc <- c(1,2,4,8)

#Read <- c(0.00569296, 0.016588, 0.058814, 0.0734541)

MinMax <- c( 0.604518, 0.301678 , 0.151565 , 0.076493  )

Kalman <- c(  0.523409, 0.260573, 0.048197, 0.0250406 )

#plot(Proc, MinMax, pch=15,  ylim=c(0,1), xlab="", ylab="", type="o",col="blue", main="Scalability Analysis")


## Allow a second plot on the same graph
#par(new=TRUE)

plot(Proc, Kalman, pch=15,  ylim=c(0,1), xlab="", ylab="", type="o",col="red",main="Strong Scalability of Kalman Filter ")

mtext("Elapsed Time",side=2,line=2.5)

#par(new=TRUE)

mtext("Number of Procs  ",side=1,col="black",line=2.5)  


#plot(Proc, Read, pch=15,  ylim=c(0,1), xlab="", ylab="", type="o",col="black")

#legend("topright",legend=c("Min Max", "Kalman Filter"),
#  text.col=c("blue","red"),pch=c(15,15),col=c("blue","red") )

legend("topright",legend=c( "Kalman Filter"),
  text.col=c("red"),pch=c(15),col=c("red") )

#legend("topright",legend=c("Min Max", "Kalman Filter", "Read File"),
#  text.col=c("blue","red","black"),pch=c(15,15,15),col=c("blue","red","black"))

