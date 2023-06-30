#!/bin/bash
#SBATCH --job-name=test
#SBATCH --time=10:00
#SBATCH --cpus-per-task=1 # Request that ncpus be allocated per process.
#SBATCH --ntasks-per-node=128 # Max number of processes
     #icase for filtering
icase=3

     #Signal ID for Filtering
id=1

     #Kalman Filter Parameters
R=10
R_range_offset=0;
R_range_endset=100;

    #Parallelization Parameters
numiMPI_Procs=
numCommunicatoi=4
num_Thread=1


    #Hardware Configuration
num_numa=4
num_core_numa=16

module load compiler/gcc/11.2.0
module load mpi/openmpi/4.1.1-gcc-11.2.0
module list
PROJECT_DIR=/nfs_home/hpcjfada/SERRANO/serrano
EXE=${PROJECT_DIR}/build/Seranno

echo start
for freq_stp in {0..3..1}
do 
echo freq_stp $freq_stp
	sudo /opt/power/rome-freq/bin/set_node01_user_frequency.sh $freq_stp	

	for np in {1,2,4,8,16,32,64}
	do echo np $np	
         sleep 2		
mpirun --mca pml ob1 --mca btl tcp,self --bind-to core -n $np $EXE $icase $id $R $num_Thread $num_numa $num_core_numa
done
done

sudo /opt/power/rome-freq/bin/set_node01_default_frequency.sh

