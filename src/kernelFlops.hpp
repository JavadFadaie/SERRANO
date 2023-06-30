#ifndef KERNELFLOPS_HPP
#define KERNELFLOPS_HPP
 

 struct flops {
 
  long long mult;
  long long div;
  long long add;
  long long rbyte;
  long long wbyte;
 
 };
 
void affinity_set_aff(int core_id)
{
 // int thread_num = omp_get_thread_num();
        int err;
        cpu_set_t processor_mask;
        cpu_set_t new_mask;
        unsigned int len = sizeof(new_mask);
  cpu_set_t cur_mask;
        int ret;
  pid_t p = 0;
        CPU_ZERO(&processor_mask);
        ret = sched_getaffinity(p, len, &cur_mask);
        CPU_SET(core_id,&processor_mask);
  //printf("%d:current sched_getaffinity = %d, cur_mask = %08lx\n",thread_num, ret, processor_mask);
        err = sched_setaffinity( 0, sizeof(cpu_set_t), &processor_mask );
        //ret = sched_getaffinity(p, len, &new_mask);
  //printf("%d:new sched_getaffinity = %d, cur_mask = %08lx\n", thread_num,ret, new_mask);

                                                                                            
}



enum class TypeInputData { SPX, IDEKO };
#endif
