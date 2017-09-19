//Adam Svetec
//Cameron Clifton
//CSE 375/475

#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <random>
#include <fstream>

#include "k_means_parallel.h"
#include "k_means_serial.h"
#include "point.h"

//can we find a way to automatically graph results?

namespace chrono = ::std::chrono;

template <typename T>
double test( size_t num_points, size_t clusters, bool parallel, std::string write_to_file = "" ){

    point<float>* points = new point<float>[num_points];
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,1000);
    for(int i=0; i<num_points; ++i){
        points[i] = point<float>((float) distribution(generator), (float) distribution(generator));
    }
    cluster_id* ids = new cluster_id[num_points];
    point<float>* centroids = new point<float>[clusters];

    chrono::high_resolution_clock::time_point start;
    chrono::high_resolution_clock::time_point end; 
    if( parallel ){
        start = chrono::high_resolution_clock::now();
        compute_k_means_parallel( num_points, points, clusters, ids, centroids );
        end = chrono::high_resolution_clock::now();
    }else{
        start = chrono::high_resolution_clock::now();
        compute_k_means_serial( num_points, points, clusters, ids, centroids );
        end = chrono::high_resolution_clock::now();
    }
    
    double time = chrono::duration_cast<T>( end - start ).count();

    if(write_to_file != ""){
        std::ofstream output_file;
        output_file.open (write_to_file);
        output_file << "#" << (parallel ? "Parallel" : "Serial") << ", Execution Time: " << time;
        output_file << ", Points(#): " << num_points << ", Clusters(#): " << clusters << "\n";
        output_file << "## Points (x, y, cluster id)\n";
        for(int i=0; i<num_points; ++i){
            output_file << points[i].x << ", " << points[i].y << ", " << ids[i] << "\n";
        }
        output_file << "## Centroids (x, y, cluster id)\n";
        for(int i=0; i<clusters; ++i){
            output_file << centroids[i].x << ", " << centroids[i].y << ", " << i << "\n";
        }
        output_file.close();
    }

    return time;
}

int main(int argc, char ** argv){

    double time = test<chrono::milliseconds>( 10000, 10, true, "test_output.csv" );
    return 0;
}
