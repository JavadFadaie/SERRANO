import subprocess
import tkinter as tk
from tkinter import ttk

# Define the list of filter options
filter_options = ["kalman filter", "fft filter", "sav gol", "k-means", "knn"]
use_case_provider_option = ["ideko", "inbestme"]

def execute_serrano_script(use_case_providers, kernel_name, num_signals, num_processes, precision):

    icase = []
    
    # Applying Filter (1: Filter apply, 0: Filter does not apply)

    Kalman_Filter = 0
    FFT_Filter = 0
    BlackScholes = 0
    SavitzkyGoly_Transform = 0
    Wavelet_Filter = 0

    if kernel_name == "kalman filter":
        print("this is kalman filter")
        Kalman_Filter = 1
        icase = [-1, 0, 1, -2]
    elif kernel_name == "fft filter":
        print("this is fft filter")
        FFT_Filter = 1
        icase = [-1, 0, 1, -2]
    elif kernel_name == "black scholes":
        print("this is black scholes")
        BlackScholes = 1
        icase = [-1, 0, 1, -2]
    elif kernel_name == "k-means": 
        print("this is k-means")
        FFT_Filter = 1
        icase = [-1, 0, 2]
    elif kernel_name == "knn": 
        print("this is k-means")
        FFT_Filter = 1
        icase = [-1, 0, 2]    
    elif kernel_name == "sav gol":
        print("this is sav gol")
        SavitzkyGoly_Transform = 1
        icase = [-1, 0, 1, -2]
    elif kernel_name == "wavelet ":
        print("this is sav gol")
        Wavelet_Filter = 1
        icase = [-1, 0, 1, -2]
   
    

     


    # Parallelization Parameters
    num_MPI_Procs = num_processes
        
    
    # Transprecision Techniques
    precision_senario = precision


    # Choosing the Kernel Type
    IDEKO_Kernel = 0
    INBestMe_Kernel = 0
    
    if use_case_providers == "ideko":
        IDEKO_Kernel = 1
        readInputData = "/Init_Data/position/raw_data/position_"+str(num_signals) + ".csv" 
    elif use_case_providers == "inbestme":     
        readInputData = "/Init_Data/raw_data_InBestMe/"+str(num_signals) +"_asset_prices.csv"
        INBestMe_Kernel = 1

    # Packing the into CSV format
    packing2CSVformat = 0

    # Benchmark Status
    BenchmarkState = 0


    # Kalman Filter Parameters
    R = "1"

    # KNN Parameters
    K_nearest_neighbor = 7
    Cluster_number_KNN = 2

    # Kmean Parameters
    number_cluster_kmean = 2
    epsilon_criteria = 0.0001

    num_Thread = 1

    # Approximation Computing techniques
    perforation_stride = 1


    # Hardware Configuration
    num_numa = 8
    num_core_numa = 16

    # Setting up the workspace
    workspace = "/home/javad/Desktop/Serrano_code_hlrs/serrano/SERRANO/data"
    profillgWorkSpace = "/home/javad/Desktop/Serrano_code_hlrs/serrano/SERRANO/profile"

    # Path for input and output data
    #readInputData="/Init_Data/sample/sample.csv"
    #readInputData = "/Init_Data/position/raw_data/position_110.csv"
    #readInputData="/Init_Data/raw_data_InBestMe/10_asset_prices.csv"
    inputDataDouble = "/Input_Data/Double_Data_Type/signalFilter"
    inputDataFloat = "/Input_Data/Float_Data_Type/signalFilter"
    inferenceKNNPath = "/Init_Data/position/inference_data_sample/"
    clustring_label = "/Output_Data/KMean/KMean_cluster.csv"
    outputDataCSV = "data/outputData/FFTFilter/FFT.csv"

    EXE = "build/SERRANO"

    # Command to execute
    for case in icase:
        print(case)
        command = [
            "mpirun",
            "--oversubscribe",
            "-np",
            str(num_MPI_Procs),
            EXE,
            str(case),
            str(BenchmarkState),
            str(Kalman_Filter),
            str(FFT_Filter),
            str(BlackScholes),
            str(SavitzkyGoly_Transform),
            str(R),
            str(K_nearest_neighbor),
            str(Cluster_number_KNN),
            str(number_cluster_kmean),
            str(epsilon_criteria),
            str(perforation_stride),
            str(precision_senario),
            str(num_numa),
            str(num_core_numa),
            str(num_Thread),
            workspace,
            profillgWorkSpace,
            readInputData,
            inputDataDouble,
            inputDataFloat,
            inferenceKNNPath,
            str(IDEKO_Kernel),
            str(INBestMe_Kernel),
            str(packing2CSVformat),
            str(Wavelet_Filter),
            clustring_label,
        ]
        subprocess.run(command, check=True)


def execute_script():
    use_case_provider = use_case_combo.get()     
    kernel_name = filter_combo.get()
    num_signals = size_entry.get()
    num_processes = proc_entry.get()
    precision = precision_entry.get()
    try:
        print("this is the case", use_case_provider, kernel_name, num_signals, num_processes, precision )
        execute_serrano_script(use_case_provider, kernel_name, num_signals, num_processes, precision)
        result_label.config(text="Execution completed.")
    except ValueError:
        result_label.config(text="Invalid input. Please enter an integer.")
    except FileNotFoundError:
        result_label.config(text="The 'student' script was not found.")

# Create the main window
window = tk.Tk()
window.title("Script Execution GUI")

# Create and place widgets

# Create a custom style for the interface
style = ttk.Style()

# Set the background color for the window
style.configure("TFrame", background="white")  # Use your preferred color

# Set the text color and font for labels
style.configure("TLabel", foreground="black", font=("Helvetica", 12))

# Set the background color for buttons
style.configure("TButton", background="lightblue", foreground="black", font=("Helvetica", 12))

# Set the background color for the entry field
style.configure("TEntry", fieldbackground="white", font=("Helvetica", 12))

# Set the background color for combobox dropdowns
style.map("TCombobox", fieldbackground=[("readonly", "lightblue")])

use_case_provider = ttk.Label(window, text="Select a use case provider:")
use_case_provider.grid(row=1, column=0, padx=10, pady=10, sticky="w")
use_case_combo = ttk.Combobox(window, values=use_case_provider_option)
use_case_combo.grid(row=1, column=1, padx=10, pady=10, sticky="w")

filter_label = ttk.Label(window, text="Select a kernel:")
filter_label.grid(row=2, column=0, padx=10, pady=10, sticky="w")
filter_combo = ttk.Combobox(window, values=filter_options)
filter_combo.grid(row=2, column=1, padx=10, pady=10, sticky="w")

label = ttk.Label(window, text="Enter the signal size:")
label.grid(row=3, column=0, padx=10, pady=10, sticky="w")
size_entry = ttk.Entry(window)
size_entry.grid(row=3, column=1, padx=10, pady=10, sticky="w")

label = ttk.Label(window, text="Enter the number of process:")
label.grid(row=4, column=0, padx=10, pady=10, sticky="w")
proc_entry = ttk.Entry(window)
proc_entry.grid(row=4, column=1, padx=10, pady=10, sticky="w")

label = ttk.Label(window, text="Enter the precision :")
label.grid(row=5, column=0, padx=10, pady=10, sticky="w")
precision_entry = ttk.Entry(window)
precision_entry.grid(row=5, column=1, padx=10, pady=10, sticky="w")

execute_button = ttk.Button(window, text="Execute Script", command=execute_script)
execute_button.grid(row=6, column=0, columnspan=2, padx=10, pady=10)

result_label = ttk.Label(window, text="")
result_label.grid(row=7, column=0, columnspan=2, padx=10, pady=10)

# Start the GUI event loop
window.mainloop()
