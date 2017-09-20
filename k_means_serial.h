//Adam Svetec
////Cameron Clifton
////CSE 375/475

#ifndef K_MEANS_SERIAL_H
#define K_MEANS_SERIAL_H

#include <iostream>
#include "utility.h"

typedef int cluster_id;

template <typename T>
void compute_k_means_serial( size_t n, const point<T> points[], size_t k, cluster_id id[], point<T> centroids[] ){

  view<T> global(k);
  for(size_t i = 0; i < n;++i){
    id[i] = i % k;
    global.array[id[i]].tally(points[i]);
  }

  size_t change=0;
  do{
    change=0;
    //std::cout<<"gothere \n";
 
    repair_empty_clusters(n,points,id,k,centroids,global.array);

    for(size_t j = 0; j<k; ++j){
      centroids[j]= global.array[j].mean();
      global.array[j].clear();
    }

    for(size_t i=0; i<n; ++i){
      //getting closest centroid to the point
      cluster_id j = reduce_min_ind(centroids, k , points[i]);
      if(j!=id[i]){
	id[i]= j;
	++change;
      }
      global.array[j].tally(points[i]);
    }
  }
  while(change!=0);
 return;
}

#endif
