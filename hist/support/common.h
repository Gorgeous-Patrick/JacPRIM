#include <stdint.h>
// Structures used by both the host and the dpu to communicate information 
typedef struct {
    uint32_t size;
    uint32_t transfer_size;
    uint32_t bins;
	enum kernels {
	    kernel1 = 0,
	    nr_kernels = 1,
	} kernel;
} dpu_arguments_t;
