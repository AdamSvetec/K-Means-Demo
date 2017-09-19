//Adam Svetec
//Cameron Clifton
//CSE 375/475

#ifndef K_MEANS_PARALLEL_H
#define K_MEANS_PARALLEL_H

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>


#include "utility.h"




template <typename T> using tls_type = tbb::enumerable_thread_specific< view<T> >;

template <typename T> void reduce_local_counts_to_global_count( tls_type<T>& tls, view<T>& global );
template <typename T> void reduce_local_sums_to_global_sum( size_t k, tls_type<T>& tls, view<T>& global );
template <typename T> int reduce_min_ind( const point<T> centroid[], size_t k, point<T> value );
template <typename T> void repair_empty_clusters( size_t n, point<T> const points[], cluster_id id[], size_t k, point<T> centroids[], sum_and_count<T> const array[]); 

//n: number of points
//points: array of points of size n
//k: number of clusters
//id: array of cluster ids, one for each point, size n
//centroids: array of centroid points of size k
template <typename T>
void compute_k_means_parallel( size_t n, const point<T> points[], size_t k, cluster_id id[], point<T> centroids[] ){

    tls_type<T> tls([&]{return k;});
    view<T> global(k);

    //'randomly' assign points to cluster and sum the points of each cluster 
    tbb::parallel_for( tbb::blocked_range<size_t>(0,n), 
            [=,&tls,&global]( tbb::blocked_range<size_t> r ){
                view<T>& v = tls.local();
                for( size_t i=r.begin(); i!=r.end(); ++i ){
                    id[i] = i % k;
                    //Peeled "Sum Step"
                    v.array[id[i]].tally(points[i]);
                }
            }
    );

    //Loop until ids do not change
    size_t change;
    do {
        //reduce local sums to global sums
        reduce_local_sums_to_global_sum( k, tls, global );

        //repair any empty clusters
        repair_empty_clusters( n, points, id, k, centroids, global.array );

        //divide step: compute centroids from global sums
        for( size_t j=0; j<k; ++j ){
            centroids[j] = global.array[j].mean();
            global.array[j].clear();
        }

        //compute new clusters and their local sums
        tbb::parallel_for( tbb::blocked_range<size_t>(0,n),
                [=,&tls,&global]( tbb::blocked_range<size_t> r ){
                    view<T>& v = tls.local();
                    for( size_t i=r.begin(); i!=r.end(); ++i){
                        //reassign step: find index closest to points[i]
                        cluster_id j = reduce_min_ind(centroids, k, points[i]);
                        //if point is assigned to new cluster, update its id and indicate change
                        if( j!=id[i] ){
                            id[i] = j;
                            ++v.change;
                        }
                        //add point to centroid sum and count
                        v.array[j].tally(points[i]);
                    }
                }
        );

        //reduce local change count to a single global change count
        reduce_local_counts_to_global_count( tls, global );
    }
    while( global.change != 0 ); //if any points have switched centroids
}

//iterates through all changes to find total amount of changes
//resets local changes to 0
template <typename T>
void reduce_local_counts_to_global_count( tls_type<T>& tls, view<T>& global ){
    global.change = 0;
    for( auto i=tls.begin(); i != tls.end(); ++i){
        view<T> & v = *i;
        global.change += v.change;
        v.change = 0;
    }   
}

//goes through each view and adds sum_and_counts for each centroid to global array of sum_and_counts
template <typename T>
void reduce_local_sums_to_global_sum( size_t k, tls_type<T>& tls, view<T>& global ){
    for( auto i=tls.begin(); i!=tls.end(); ++i ){
        view<T> & v = *i;
        for( size_t j=0; j<k; ++j){
            global.array[j] += v.array[j];
            v.array[j].clear();
        }
    }
}




#endif
