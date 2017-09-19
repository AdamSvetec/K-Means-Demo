//Adam Svetec
//Cameron Clifton
//CSE 375/475

#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>
#include <assert.h>



typedef int cluster_id;

template <typename T>
struct point {
  T x;
  T y;

  point(): x(0), y(0){}
  point(T x1, T y1): x(x1), y(y1){}

  void operator+=( point<T> const & p ){
    x += p.x;
    y += p.y;
  }

  void operator-=( point<T> const & p ){
    x -= p.x;
    y -= p.y;
  }

  point<T> operator/( size_t const & s ) const {
    assert(s != 0);
    return point<T>(x/s, y/s);
  }

  T distance(point const & p) const{
    return sqrt( pow((p.x-x),2) + pow((p.y-y),2));
  }
};



//holds a sum of points and the number of points added to it
//can add other sum_and_counts to itself


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
  void remove( point<T> const & p){
    sum -= p;
    --count;
  }
  point<T> mean() const {
    return sum/count;
  }
  void operator+=( sum_and_count<T> const & other ){
    sum += other.sum;
    count += other.count;
  }
};


//view for each thread
//holds a sum_and_count for each centroid
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


//find closest centroid for a given point
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

template <typename T>
int find_furthest_point( const point<T> centroid, size_t n, point<T> const points[] ){
  int max = -1;
  T maxd = std::numeric_limits<T>::min();
  for( int j = 0; j<n; ++j ){
    T d = centroid.distance( points[j] );
    if ( d>maxd ){
      maxd = d;
      max = j;
    }
  }
  return max;
}

//reassign a centroid if it does not have any points assigned to it
//definitely some edge cases missed
template <typename T> void repair_empty_clusters( size_t n, point<T> const points[], cluster_id id[], size_t k, point<T> centroids[], sum_and_count<T> array[]){
  for( int i=0; i < k; ++i){
    if(array[i].count == 0){
      int max_index = find_furthest_point( centroids[i], n, points );
      id[max_index] = i;
      array[i].tally(points[max_index]);
      array[id[k]].remove(points[max_index]);
    }
  }
  return;
}
#endif
