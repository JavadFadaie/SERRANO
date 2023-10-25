import numpy as np
from scipy.optimize import curve_fit

# Define a cubic polynomial function to fit the data
def nonlinear_function(x, a, b, c, d):
    return a * x**3 + b * x**2 + c * x + d

# Function to calculate approximated min execution time for an arbitrary file size
def approximation(file_size, normalized_file_size, normalized_measured_data):
    # Perform curve fitting to find the best-fitting parameters with custom initial guesses
    params, covariance_matrix = curve_fit(nonlinear_function, normalized_file_size, normalized_measured_data, p0=[1, 1, 1, 1])

    # Extract the parameters from the fit
    a_fit, b_fit, c_fit, d_fit = params

    # Print the nonlinear function formula
    #print(f"Nonlinear function: y = {a_fit:.3f} * x^3 + {b_fit:.3f} * x^2 + {c_fit:.3f} * x + {d_fit:.3f}")
    normalized_file_size = file_size / np.max(file_size_array)
    
    return nonlinear_function(normalized_file_size, a_fit, b_fit, c_fit, d_fit)

try:
    filter_type = input("Enter the type of filter (e.g., kalman filter, fft filter, sav gol filter, black scholes, kmeans, knn): ").strip().lower()
    arbitrary_file_size = float(input("Enter the arbitrary file size: "))
except ValueError:
    print("Invalid input! Please enter a valid file size as a number.")
    exit()

# Data for different filters
if filter_type == "kalman filter":
    file_size_array = np.array([100, 500, 1000, 2000, 4879])
    min_energy_array = np.array([23.36, 116.7, 198.1, 1324.5, 4476.98])
    min_exe_time_array = np.array([0.105, 0.178, 0.180, 0.530, 2.035])
    number_proc = np.array([128, 512, 1028, 2056, 3000])
elif filter_type == "sav gol filter":
    file_size_array = np.array([100, 500, 1000, 2000, 4879])
    min_energy_array = np.array([48.01,115.82,291.33,1813.51, 3760.48])  
    min_exe_time_array = np.array([0.21, 0.123, 0.155, 0.484, 2.035])  
    number_proc = np.array([100,512, 1028, 2056, 3000])  
elif filter_type == "black scholes":
    file_size_array = np.array([100, 500, 1000, 2000, 4879])
    min_energy_array = np.array([16.28, 153.59, 290.11, 505.89, 4476.98])  
    min_exe_time_array = np.array([0.07,0.16,0.15,0.13,0.18])  
    number_proc = np.array([100, 512, 1028, 2056, 3000])      
elif filter_type == "fft filter":
    # Data for FFT Filter
    file_size_array = np.array([26, 104, 156, 208, 234, 260])
    min_energy_array = np.array([33.21,23.76,33.21,79.82,35.48,64.33])  
    min_exe_time_array = np.array([0.072,0.053, 0.099, 0.079, 0.040, 0.057])  
    number_proc = np.array([32 , 64 , 128, 128, 128, 128])  
elif filter_type == "kmeans":
    # Data for KMeans Filter
    file_size_array = np.array([110, 330, 550, 770, 990, 1100])
    min_energy_array = np.array([4416.55, 5145.27, 9276.83, 12958, 15031.3, 27405.4])  
    min_exe_time_array = np.array([10.95, 7.59, 8.21, 8.19, 8.05, 13.10])  
    number_proc = np.array([128, 256, 512, 512, 1028, 1028])  
elif filter_type == "knn":
    # Data for KMeans Filter
    file_size_array = np.array([110, 330, 550, 770, 990, 1100])
    min_energy_array = np.array([96.42, 334.166, 580.295, 830.691, 1079.75, 1485.88])  
    min_exe_time_array = np.array([0.42, 0.49, 0.51, 0.52, 0.57, 0.71])  
    number_proc = np.array([128, 256, 512, 512, 1028, 1028])  
else:
    print("Invalid filter type! Please enter one of the valid filter types.")
    exit()

# Normalize the data
normalized_file_size = file_size_array / np.max(file_size_array)
normalized_min_exe_time = min_exe_time_array / np.max(min_exe_time_array)
normalized_energy_array = min_energy_array / np.max(min_energy_array)
normalized_number_proc = number_proc / np.max(number_proc)

# Approximate the min execution time, min energy, and number of processors for the arbitrary file size
approximated_min_exe_time = approximation(arbitrary_file_size, normalized_file_size, normalized_min_exe_time)
approximated_min_energy = approximation(arbitrary_file_size, normalized_file_size, normalized_energy_array)
approximated_number_proc = approximation(arbitrary_file_size, normalized_file_size, normalized_number_proc)


print("")
print("     Approximated values for optimal exeuction of this application in the HPC system:")
# Concatenate titles and results for the arbitrary file size
titles = ["Min Execution Time (sec)", "Min Energy (joule)", "Number of Processors"]
values = [
    f"{approximated_min_exe_time * np.max(min_exe_time_array):.3f}                       ",
    f"{approximated_min_energy * np.max(min_energy_array):.3f}              ",
    str(int(approximated_number_proc * np.max(number_proc)))
]

# Print titles and values in a tabular format
print("\t".join(titles))
print("\t".join(values))
