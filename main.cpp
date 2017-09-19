//Adam Svetec
//Cameron Clifton
//CSE 375/475

#include <stdlib.h>

#include "k_means_parallel.h"
#include "k_means_serial.h"
#include "point.h"

int main(int argc, char ** argv){

    size_t n = 0;
    point<float>* points = new point<float>[0];
    size_t k = 0;
    cluster_id* ids = new cluster_id[0];
    point<float> *centroids = new point<float>[0];
    
    compute_k_means_parallel( n, points, k, ids, centroids );

    compute_k_means_serial( n, points, k, ids, centroids );

    return 0;
}
