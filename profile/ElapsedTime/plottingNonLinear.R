# Define a function
myfunc <- function(x) {
   0.0382379*x + 1.11035
}



myfuncPol <- function(x) {
   1.03247*exp( 0.0294826*x)  - 0.0302112
}


# Generate some data
x <- seq(2, 27, length.out = 100)

y <- myfunc(x)
y2 <- myfuncPol(x)


ac <- read.table("perforamnceApproximation.txt")
file_size <- ac[,2]

elapsed <- ac[,3]

# Generate some data
options(bitmapType="jpeg")

#jpeg("myplot.jpg")
# Plot the function

plot(x, y, type = "l", col = "red", lwd = 2, xlab = "signal batch x 100", ylab = "execution time/sec", main = "Execution time approximation")

lines(x,y2, type = "l", col = "blue", lwd = 2)

lines(file_size, elapsed, type = "o", col = "black", lwd = 2)

legend("topleft",legend=c("linear approximation ", "nonlinear approximation","original meaurment" ),
  text.col=c(),pch=c(16,16,16),col=c("red","blue","black"))


dev.off()
