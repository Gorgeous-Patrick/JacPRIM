#include "common.h"
#include <dpu.h>
static void histogram(uint32_t* histo, uint32_t bins, T *input, uint32_t histo_id, unsigned int l_size){
    for(unsigned int j = 0; j < l_size; j++) {
        T d = (input[j] * bins) >> DEPTH;
        mutex_lock(my_mutex[histo_id]);
        histo[d] += 1;
        mutex_unlock(my_mutex[histo_id]);
    }
}

int main() {
  unsigned int tasklet_id = me();
}
