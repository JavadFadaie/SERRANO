import subprocess
import tkinter as tk
from tkinter import ttk

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

if __name__ == "__main__":
    use_case_providers = input("Enter the use case providers:  ")
    kernel_name = input("Enter the kernel name: ")
    num_signals = input("Enter the number of signals:  ")
    num_processes = input("Enter the number of processes: ")
    precision = input("Enter the precision: ")
    
    execute_serrano_script(use_case_providers, kernel_name, num_signals, num_processes, precision)
