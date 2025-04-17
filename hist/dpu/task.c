#include "common.h"
static void histogram(uint32_t* histo, uint32_t bins, int *input, uint32_t histo_id, unsigned int l_size){
    for(unsigned int j = 0; j < l_size; j++) {
        int d = (input[j] * bins);
        histo[d] += 1;
    }
}

int main() {
  
}
