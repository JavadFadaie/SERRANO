#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>



void linearTransformation(std::vector<double> & x);

void linearApprox(std::vector<double> x, std::vector<double> Y );

// Define the function to be fitted
double my_func(double x, std::vector<double> params);

// Define the objective function to be minimized
double objective_function( std::vector<double> const & params, std::vector<double> const & x, std::vector<double> const & y);

// Define the gradient of the objective function
std::vector<double> gradient(std::vector<double> const & params, std::vector<double> const & x, std::vector<double> const & y);


void NonLinearApprox(std::vector<double> x, std::vector<double> y );

// Define the main function
int main() {


    std::vector<double> x = {26, 104, 156, 208, 234, 260};

    std::vector<double> y = {1.25, 1.37, 1.84, 1.86, 2.03, 2.09};
    
     for(int i(0); i<x.size(); i++){
      x[i] /= 10.;
      //std::cout << x[i] << std::endl;
     }
  
  
  

     linearApprox(x, y );
   
    // Set the initial parameters
    
    NonLinearApprox( x,y );
  

    return 0;
}


void NonLinearApprox(std::vector<double> x, std::vector<double> y ){

      std::vector<double> params = {1.0, 0.01, 0.0};

    // Set the step size for the gradient descent algorithm
    double step_size = 0.0001;

    // Perform gradient descent
    for (int i = 0; i < 1000; i++) {
        std::vector<double> grad = gradient(params, x, y);

        for (size_t j = 0; j < params.size(); j++) {
            params[j] -= step_size * grad[j];
        }

        double obj_val = objective_function(params, x, y);
        std::cout << "Iteration " << i << ": objective function value = " << obj_val << std::endl;
    }

    // Print the final parameters
   // std::cout << " " << params[0] << "*exp( " << -params[1] << "*x) + " << params[2] << std::endl;

}


double my_func(double x, std::vector<double> params) {
    double a = params[0];
    double b = params[1];
    double c = params[2];

    return a * std::exp(-b * x) + c;
}

// Define the objective function to be minimized
double objective_function( std::vector<double> const & params, std::vector<double> const & x, std::vector<double> const & y) {
    double sum = 0.0;

    for (size_t i = 0; i < x.size(); i++) {
        double y_pred = my_func(x[i], params);
        double error = y[i] - y_pred;
        sum += pow(error, 2.0);
    }

    return sum;
}

// Define the gradient of the objective function
std::vector<double> gradient(std::vector<double> const & params, std::vector<double> const & x, std::vector<double> const & y) {
    
    std::vector<double> grad(3);

    for (size_t i = 0; i < x.size(); i++) {
        double y_pred = my_func(x[i], params);
        double error = y[i] - y_pred;

        grad[0] += -exp(-params[1] * x[i]) * error;
        grad[1] += params[0] * x[i] * exp(-params[1] * x[i]) * error;
        grad[2] += error;
    }

    return grad;
}

void linearApprox(std::vector<double> x, std::vector<double> Y ){
  // Calculate the least squares estimates of m and b
    size_t n = x.size();
    double sum_x = accumulate(x.begin(), x.end(), 0.0);
    double sum_Y = accumulate(Y.begin(), Y.end(), 0.0);
    double sum_xx = inner_product(x.begin(), x.end(), x.begin(), 0.0);
    double sum_xy = inner_product(x.begin(), x.end(), Y.begin(), 0.0);
    double m = (n*sum_xy - sum_x*sum_Y) / (n*sum_xx - sum_x*sum_x);
    double b = (sum_Y - m*sum_x) / n;

    std::cout << " Print the equation of the line" << std::endl;
    std::cout << "Y = " << m << "x + " << b << std::endl;
    std::cout << std::endl;
}




void linearTransformation(std::vector<double> & x) {
      // Find the minimum and maximum values of x
    double min_x = *min_element(x.begin(), x.end());
    double max_x = *max_element(x.begin(), x.end());

    // Transform x into the interval [0, 32]
    for (size_t i = 0; i < x.size(); i++) {
        x[i] = (x[i] - min_x) * (32.0 / (max_x - min_x));
    }
}



