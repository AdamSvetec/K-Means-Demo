//Adam Svetec
//Cameron Clifton
//CSE 375/475

#ifndef K_MEANS_PARALLEL_H
#define K_MEANS_PARALLEL_H

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>

#include "point.h"

template <typename T>
struct sum_and_count {
    sum_and_count(): sum(), count(0) {}
    point<T> sum;
    size_t count;
    void clear() {
        sum = point<T>();
        count = 0;
    }
    void tally( point<T> const & p ){
        sum += p;
        ++count;
    }
    point<T> mean() const {
        return sum/count;
    }
    void operator+=( sum_and_count<T> const & other ){
        sum += other.sum;
        count += other.count;
    }
};

template <typename T>
class view{
    view( view const & v) = delete;
    void operator=( view const & v ) = delete;
  public:
    sum_and_count<T> * array;
    size_t change;
    view( size_t k ): array(new sum_and_count<T>[k]), change(0) {}
    ~view() { delete [] array; }
};

typedef int cluster_id;
template <typename T> using tls_type = tbb::enumerable_thread_specific< view<T> >;

template <typename T> void reduce_local_counts_to_global_count( tls_type<T>& tls, view<T>& global );
template <typename T> void reduce_local_sums_to_global_sum( size_t k, tls_type<T>& tls, view<T>& global );
template <typename T> int reduce_min_ind( const point<T> centroid[], size_t k, point<T> value );
template <typename T> void repair_empty_clusters( size_t n, point<T> const points[], cluster_id id[], size_t k, point<T> centroids[], sum_and_count<T> const array[]); 

template <typename T>
void compute_k_means_parallel( size_t n, const point<T> points[], size_t k, cluster_id id[], point<T> centroids[] ){

    tls_type<T> tls([&]{return k;});
    view<T> global(k);

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
        //reduce local sums to global sum
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
                        if( j!=id[i] ){
                            id[i] = j;
                            ++v.change;
                        }
                        //sum step
                        v.array[j].tally(points[i]);
                    }
                }
        );

        //reduce local counts to global count
        reduce_local_counts_to_global_count( tls, global );
    }
    while( global.change != 0 );
}

template <typename T>
void reduce_local_counts_to_global_count( tls_type<T>& tls, view<T>& global ){
    global.change = 0;
    for( auto i=tls.begin(); i != tls.end(); ++i){
        view<T> & v = *i;
        global.change += v.change;
        v.change = 0;
    }   
}

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

template <typename T> 
int reduce_min_ind( const point<T> centroid[], size_t k, point<T> value ){
    int min = -1;
    T mind = std::numeric_limits<T>::max();
    for( int j = 0; j<k; ++j ){
        T d = centroid[j].distance( value );
        if ( d<mind ){
            mind = d;
            min = j;
        }
    }
    return min;
}

template <typename T> void repair_empty_clusters( size_t n, point<T> const points[], cluster_id id[], size_t k, point<T> centroids[], sum_and_count<T> const array[]){
    //todo still need to implement
    return;
}

#endif
