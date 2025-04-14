#include <dpu.h>
#include "common.h"
int main() {
    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));
    DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
    printf("Allocated %d DPU(s)\n", nr_of_dpus);
    dpu_arguments_t input_arguments[NR_DPUS];
    for(i=0; i<nr_of_dpus-1; i++) {
        input_arguments[i].size=input_size_dpu_8bytes * sizeof(T); 
        input_arguments[i].transfer_size=input_size_dpu_8bytes * sizeof(T); 
        input_arguments[i].bins=p.bins;
        input_arguments[i].kernel=kernel;
    }
    
    i = 0;
    DPU_FOREACH(dpu_set, dpu, i) {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments[i]));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));

    const unsigned int input_size_8bytes = 
        ((input_size * sizeof(T)) % 8) != 0 ? roundup(input_size, 8) : input_size; // Input size per DPU (max.), 8-byte aligned
    const unsigned int input_size_dpu = divceil(input_size, nr_of_dpus); // Input size per DPU (max.)
    const unsigned int input_size_dpu_8bytes = 
        ((input_size_dpu * sizeof(T)) % 8) != 0 ? roundup(input_size_dpu, 8) : input_size_dpu; // Input size per DPU (max.), 8-byte aligned

    DPU_FOREACH(dpu_set, dpu, i) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, bufferA + input_size_dpu_8bytes * i));
        }
        DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, 0, input_size_dpu_8bytes * sizeof(T), DPU_XFER_DEFAULT));

    printf("Run program on DPU(s) \n");
    DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));
    

    for(i = 1; i < nr_of_dpus; i++){
            for(unsigned int j = 0; j < p.bins; j++){
                histo[j] += histo[j + i * p.bins];
            }			
        }

}
