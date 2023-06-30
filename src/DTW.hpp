#ifndef DTW_HPP
#define DTW_HPP

#include <limits.h>
#include "ReadWriteMPI_IO.hpp"
//https://tslearn.readthedocs.io/en/stable/user_guide/dtw.html
//https://tslearn.readthedocs.io/en/stable/gen_modules/clustering/tslearn.clustering.TimeSeriesKMeans.html
//https://github.com/ict-serrano/ideko-algorithm-vibrations-kmeans/tree/main/dataset
//https://github.com/ict-serrano/ideko-algorithm-vibrations-kmeans/tree/main/dataset

//this is the new DTW
//https://github.com/Dustin21/DynamicTimeWarping/blob/master/dtw.cpp
//https://stackoverflow.com/questions/36675738/dtw-algorithm-oct-file

//Implemetation for classic and fast version
//https://towardsdatascience.com/dynamic-time-warping-3933f25fcdd
template<typename I, typename O>
class DTW{
   
    public :
    
     class Matrix 
     {
       public : 
    
         Matrix(int const _n, int const _m)
         :n(_n+1)
         ,m(_m+1)
         ,vec(n*m,0)
         {} 
        
         Matrix()
         :n(0)
         ,m(0)
         ,vec(0)
         {} 
                
        I & operator()(int i, int j){
           return vec[i*m+j];
        }

        I const & operator()(int i, int j) const {
           return vec[i*m+j];
        }

        I & insert(int i, int j){
           return vec[i*m+j];
        }
       
        int const getNumRow(){
            return n;
        }

        int const getNumCol(){
            return m;
        }

        int const getNumRow() const{
            return n;
        }


        int const getNumCol() const{
            return m;
        }
        public:
        
        int const n;
        int const m;
        
        std::vector<I> vec; 
    };

  public :

   DTW( std::vector<I> const & _x
      , std::vector<I> const & _y);

  
   DTW();

   void test();

   I const DTW_classic( );
  
   I const lBKeoghLowerBound( std::vector<I> const & x
                            , std::vector<I> const & y
                            , int r);                              
   
   I const 
   DTW_Dist_fast( std::vector<I> const & x
                , std::vector<I> const & y
                , int w);
   
   I const Dist( I const x, I const y);

   I const 
   DTW_Dist_Optimal_W( std::vector<I> const & x
                               , std::vector<I> const & y
                               , int w);                
  
  
   void traceBackAlignment( Matrix const & D
                          , std::vector<I> const & x 
                          , std::vector<I> const & y);

   void displayDenseMatrix(Matrix const & D);

   void DisplayXY( std::vector<I> const & x
                 , std::vector<I> const & y);

   private:
   
   struct allign{
       allign(int _x, int _mx)
       :x(_x)
       ,mx(_mx)
       {}

      int x;
      int mx;
   };

  bool DTW_CLASSIC;
  bool L2_METRIC;

  std::vector<I> const  x;
  std::vector<I> const  y;
  I distance;
 
};


//////////////////////////////////////////////
///////////IMPLEMANTATION
/////////////////////////////////////////////

template<typename I, typename O>
DTW<I,O>::DTW( std::vector<I> const & _x
             , std::vector<I> const & _y)
:x(_x)
,y(_y)
,DTW_CLASSIC(true)
,L2_METRIC(false)
{}

template<typename I, typename O>
DTW<I,O>::DTW()
:DTW_CLASSIC(true)
,L2_METRIC(false)
,x(0)
,y(0)
{}


template<typename I,typename O>
void
DTW<I,O>::test()
{

  
    std::cout << DTW_classic() << std::endl;

/*
    int w = 15;
   I lowerBound =  lBKeoghLowerBound( x,y,w);
   I distance = DTW_Dist_fast(x,y,w); 
   I distance_W = DTW_Dist_Optimal_W(x,y,w); 
  std::cout << distance << "  " << distance_W << "  " << lowerBound<< std::endl;
 */  
/*
   searchFileInDirectory();
   signalReading();
   KmeanClustering();   
   //randomCentroid();
*/   
 

} 

template<typename I, typename O>
I const 
DTW<I,O>::DTW_classic()
{
  
 Matrix D(x.size(),y.size());
 
  //DisplayXY( x, y);

//Initialize the Dense Matrix
for(int i(1); i<D.getNumRow(); i++)
     D.insert(i,0) = std::numeric_limits<I>::max();

  
for(int j(1); j<D.getNumCol(); j++)
     D.insert(0,j) = std::numeric_limits<I>::max();  


 D(0,0) = 0;

//Computing the Dense matrix
for(int i(1); i<D.getNumRow(); i++)
{  
   for(int j(1); j<D.getNumCol(); j++)
    {    
          std::vector<I> v = {D(i-1,j-1),D(i-1,j),D(i,j-1)};
          auto result = std::min_element(std::begin(v), std::end(v));
          I dist = Dist( x[i-1], y[j-1] );
          D.insert(i,j) = dist + *result;
    }
 }
 
 distance = D(D.getNumRow()-1,D.getNumCol()-1);
 if(L2_METRIC){
  distance = sqrt(distance);
 }
 //displayDenseMatrix(D);
 

 return (distance);
   
}

template<typename I, typename O>
void 
DTW<I,O>::DisplayXY( std::vector<I> const & x
                    , std::vector<I> const & y){

for(int i(0); i<x.size(); i++)
  std::cout << x[i] << "  ";

 std::cout << std::endl;


for(int i(0); i<y.size(); i++)
  std::cout << y[i] << "  ";

 std::cout << std::endl; 

}

template<typename I, typename O>
I const 
DTW<I,O>::lBKeoghLowerBound( std::vector<I> const & x
                           , std::vector<I> const & y
                           , int r){
  
  I lowerBound; I upperBound; std::vector<I> yBound;
  I lbSum = 0;
    
  for(int i = 0; i < x.size(); ++i)
  {
        for(int j = i - r; j < i + r; ++j)
        {
            if (j >= 0)
            {
                yBound.push_back(y[j]);
            }
        }
        
        lowerBound = *std::min_element(yBound.begin(), yBound.end());
        upperBound = *std::max_element(yBound.begin(), yBound.end());
        
        if (i > upperBound)
        {
            lbSum = lbSum + std::pow(x[i] - upperBound, 2);
        } else {
            lbSum = lbSum + std::pow(x[i] - lowerBound, 2);
        }
    }
    
    std::cout << std::sqrt(lbSum) << "\n";
    return std::sqrt(lbSum);
}



template<typename I, typename O>
I const 
DTW<I,O>::DTW_Dist_fast( std::vector<I> const & x
                           , std::vector<I> const & y
                           , int w){

    Matrix D(x.size(),y.size());

//Initialize the Dense Matrix

for(int i(1); i<D.getNumRow(); i++)
     D.insert(i,0) = std::numeric_limits<I>::max();

  
for(int j(1); j<D.getNumCol(); j++)
     D.insert(0,j) = std::numeric_limits<I>::max();  

 D(0,0) = 0;

 w = std::max( w , static_cast<int>(fabs(x.size()-y.size())) );
//Computing the Dense matrix
for(int i(1); i<D.getNumRow(); i++)
{  
   for(int j = std::max(1,i-w); j<std::min( static_cast<int>(D.getNumCol()), i + w); j++)
    {
          std::vector<I> v = {D(i-1,j-1),D(i-1,j),D(i,j-1)};
          auto result = std::min_element(std::begin(v), std::end(v));
          D.insert(i,j) = std::pow(x[i] - y[j], 2) + *result; 
    }
 }
 I distance = D(D.getNumRow()-1,D.getNumCol()-1);
 
  //displayDenseMatrix(D);
 return sqrt(distance);

}                            

template<typename I, typename O>
I const 
DTW<I,O>::Dist( I const x, I const y){
    
    if(L2_METRIC){
     return std::pow(x - y, 2);   
    }else{
      return std::abs(x-y);   
    }
}

template<typename I, typename O>
I const  
DTW<I,O>::DTW_Dist_Optimal_W( std::vector<I> const & x
                           , std::vector<I> const & y
                           , int w){
using std::vector;
    
    // initialize constants
    const double inf_ = std::numeric_limits<double>::infinity();
    const int x_length_ = static_cast<int>(x.size());
    const int y_length_ = static_cast<int>(y.size());
    
    vector< vector<double> > dtw(x_length_, vector<double>(y_length_));
    double dist;
    
    // Initialize all dtw matrix elements to infnty
    for(int i = 0; i < x_length_; ++i)
    {
        dtw[i][0] = inf_;
    }
    
    for(int i = 0; i < y_length_; ++i)
    {
        dtw[0][i] = inf_;
    }
    dtw[0][0] = 0;
    
    // add locality constraint
    w = std::max(w, std::abs(x_length_ - y_length_));
    
    // Determine the optimal dtw path via dynamic programming,
    // specifically the recursive function outlined on wiki:
    // https://en.wikipedia.org/wiki/Dynamic_time_warping
    for(int i = 1; i < x_length_; ++i)
    {
        // adapt inner loop to window size
        for(int j = std::max(1, i - w); j < std::min(y_length_, i + w); ++j)
        {
            dist = std::pow(x[i] - y[j], 2);
            dtw[i][j] = dist + std::min({dtw[i - 1][j], dtw[i][j - 1], dtw[i - 1][j - 1]});
        }
    }
   /* 
   for(int i(1); i<x_length_; i++){
    for(int j(1); j<y_length_; j++){
      std::cout << dtw[i][j]<< "  " ; 
    }
    std::cout << std::endl;
   }
*/
   double dtwDist = std::sqrt(dtw[x_length_ - 1][y_length_ - 1]);
  
 return dtwDist;

}                            




template<typename I, typename O>
void
DTW<I,O>::traceBackAlignment( Matrix const & D  
                            , std::vector<I> const & x 
                            , std::vector<I> const & y)
{
 

  std::vector<allign> allignVec;

  int N = x.size();
  int M = y.size();
  allignVec.push_back(allign(N,M));
  
  bool Pstatus= true;
  while(Pstatus){
     
     if(N==1){
         allignVec.push_back(allign(1,M-1));
         N = 1;
         M = M-1;
     }else if(M==1){
        allignVec.push_back(allign(N-1,M));
        N = N-1;
        M = M;
     }else{

           std::vector<I> v = {D(N-1,M-1),D(N-1,M),D(N,M-1)};
          
            int Index = std::min_element(v.begin(),v.end()) - v.begin();
            if(Index == 0){
                allignVec.push_back(allign(N-1,M-1));
                N = N-1;
                M = M-1;
            }
            if(Index == 1){
                allignVec.push_back(allign(N-1,M));
                N = N-1;
                M = M;
            }
            if(Index == 2){
                allignVec.push_back(allign(N,M-1));
                N = N;
                M = M-1;
            }
      }

      if((N==1)&&(M==1)){
        Pstatus= false;
      }      
  }


}




template<typename I, typename O>
void
DTW<I,O>::displayDenseMatrix(Matrix const & D){

  for(int i(0); i<D.getNumRow(); i++){
      for(int j(0); j<D.getNumCol(); j++){
        std::cout <<  D(i,j) <<"  ";
      }
      std::cout << std::endl;
  }

  for(int i(0); i<D.getNumRow(); i++){
      for(int j(0); j<D.getNumCol(); j++){
      //  std::cout <<  D.min_i(i,j) <<","<< D.min_j(i,j)<< "    ";
      }
     // std::cout << std::endl;
  }
  std::cout << std::endl;

}

/*
void k_means_clust(data_t * data, int num_iter, int w, data_t * centroid1, data_t * centroid2, int * memberships_golden) {

  int assignments[N_CLUSTERS][N_SERIES];

  int assignments_cnt[N_CLUSTERS] = {-1,-1};

  memset(assignments, 0, sizeof(assignments[0][0]) * N_CLUSTERS * N_SERIES);

  for (int n = 0; n < num_iter; ++n) {
    std::cout << "Starting iteration " << n + 1 << " on CPU" << std::endl;
    assignments_cnt[0] = -1;
    assignments_cnt[1] = -1;

    //assign data points to centroids
    for (int i = 0; i < N_SERIES; ++i) {

      data_t * curr_series = & data[i * N_FEATURES];
      data_t min_dist = PLUS_INFINITY;
      int closest_clust = -1;

      //centroid1
      if (LB_Keogh(curr_series, centroid1, w) < min_dist) {

        data_t cur_dist = DTWDistance(curr_series, centroid1, w);
        if (cur_dist < min_dist) {
          min_dist = cur_dist;
          closest_clust = 0; //make centroid1 the closest centroid
        }

      }

      //centroid2
      if (LB_Keogh(curr_series, centroid2, w) < min_dist) {
        data_t cur_dist = DTWDistance(curr_series, centroid2, w);
        if (cur_dist < min_dist) {
          min_dist = cur_dist;
          closest_clust = 1; //make centroid2 the closest centroid
        }
      }

      if (closest_clust != -1) {
        assignments_cnt[closest_clust]++;
        int index = assignments_cnt[closest_clust];
        assignments[closest_clust][index] = i;
      }

    }

    //recalculate centroids of centroids
    for (int centroid = 0; centroid < N_CLUSTERS; ++centroid) {
      if (assignments_cnt[centroid] != -1) {

        data_t clust_sum[N_FEATURES];

        memset(clust_sum, 0, sizeof(clust_sum[0]) * N_FEATURES);

        for (int k = 0; k < assignments_cnt[centroid] + 1; ++k) {
          int series_id = assignments[centroid][k];
          for (int t = 0; t < N_FEATURES; ++t) {
            clust_sum[t] = clust_sum[t] + data[series_id * N_FEATURES + t];
          }
        }

        if (centroid == 0) {
          for (int t = 0; t < N_FEATURES; ++t)
            centroid1[t] = clust_sum[t] / (assignments_cnt[0] + 1.0);
        } else {
          for (int t = 0; t < N_FEATURES; ++t)
            centroid2[t] = clust_sum[t] / (assignments_cnt[1] + 1.0);
        }

      }
    }

  }
  
  for (int i = 0; i < N_CLUSTERS; ++i) {
    for (int j = 0; j <= assignments_cnt[i]; ++j) {
      memberships_golden[assignments[i][j]] = i;
    }
  }
  
   // Force labels to be 0 to the group with the largest number of individuals and 1 with the least
  int ones_count = 0;
  for(int i = 0; i < N_SERIES; ++i)
    if(memberships_golden[i]) ones_count++;
  
  if(ones_count > N_SERIES-ones_count)
    std::for_each (&memberships_golden[0], &memberships_golden[N_SERIES-1],[](int &n){n ^= 1;}  );
    
  return;
}

*/
#endif