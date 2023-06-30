#ifndef BLACKSCHOLES_HPP
#define BLACKSCHOLES_HPP

#include<vector>
#include<cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "signal_Distributor.hpp"
#include "signalContainer.hpp"
#include "handelReadWrite.hpp"

template<typename I, typename O>
class BlackScholes{

 public :

  struct priceContainer{
     priceContainer
        ( std::string _price_date
        , I _price_close)
        :price_date( _price_date)
        ,price_close(_price_close)
        {}
     
    std::string price_date;
    I price_close;
   };

   struct signalContainerDate{
         
          signalContainerDate
            ( int _signalId
            , std::vector<priceContainer> const & _signalVal
            , int _classID)
            :signalId(_signalId)
            ,signalVal(_signalVal)
            ,classID(_classID)
            {}             
        
          signalContainerDate()
          :signalId(0)
          ,signalVal(0,0)
          ,classID(0)
          {}

          
          
          int  signalId;
          std::vector<priceContainer> signalVal;
          int classID; 
};

 struct indexDateContainer{
     indexDateContainer
        ( int _signalIndex
        , std::string _date)
        :signalIndex(_signalIndex)
        ,date(_date)
        {}

     int signalIndex;
     std::string date; 
   };

  BlackScholes( int const & _rank
              , int const & _size);



O const d1(I const S, I const K, I const T, I const r, I const sigma);

O const d2(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_price(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_price(I const S, I const K, I const T, I const r, I const sigma) ;

O const calculate_call_delta(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_gamma(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_vega(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_theta(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_rho(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_delta(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_gamma(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_vega(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_theta(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_put_rho(I const S, I const K, I const T, I const r, I const sigma);

O const calculate_call_implied_volatility(I const Price, I const S, I const K, I const T, I const r);

O const calculate_put_implied_volatility(I const Price, I const S, I const K, I const T, I const r);

O const norm_cdf(I const x);

O const norm_pdf(I const x);

I const compute_years_untill_expiry(std::string last_date_str, std::string expiry_date );

O const calculate_std_dev( std::vector<I> const & prices);
  
void blackscholes_apply(std::vector<I> const & prices, int const signal_Index );

void blackScholesFunction( I const last_closing_price
                         , I const call_strike_price
                         , I const put_strike_price
                         , I const years_until_expiry
                         , I const risk_free_return
                         , I const sigma);


std::vector<priceContainer> readTxtFile(std::string const & path);

std::vector<O> const & apply_BlackScholes(std::vector<I> const & prices
                                         , int const & signal_Index);

void writeOutputTxtFile();


void Data_IO_Path( std::string const & _inputDataPath);

void Data_WorkSpace( std::string const & _data_Workspace);

void Profile_WorkSpace( std::string const & _profile_workspace);

void setFilterSignalPath();

void signalDistribution();

void loading_Signal();

void writeVecSignal();

std::vector<O> const & getOutPut();

std::vector<indexDateContainer> readTransfIndex();

std::string findSensorNameId(std::vector<indexDateContainer> const & Index2Date
                            ,int const signal_Index);

  
 private :

  int rank;
  int size;

  
 
 std::vector<signalContainerDate>  inputSignalContainerVec;
 std::vector<typename::signalContainer<O>>  outputSignalContainerVec;
 std::unique_ptr<signalDistributor<I>> distribute;
 std::string inputDataPath;
 std::string data_workspace;
 std::string profile_workspace;
 std::string blackScholes_path;
 
 readWriteTool<I,O> tool;
 std::vector<O> OutputData;
 int signal_Index; 
};

///////////////////////////////////
///////////////////IMPLEMENTATION
////////////////////////////////////

template<typename I, typename O>
BlackScholes<I,O>::BlackScholes
 ( int const & _rank, int const & _size)
:rank(_rank)
,blackScholes_path("/Output_Data/BlackScholes")
,size(_size)
{}

template<typename I,typename O>
void
BlackScholes<I,O>::Data_IO_Path
               ( std::string const & _inputDataPath){
  inputDataPath  = _inputDataPath; 
  inputDataPath = "/Input_Data/Double_Data_Type/signalFilter_plot";
}


template<typename I,typename O>
void
BlackScholes<I,O>::Data_WorkSpace( std::string const & _data_workspace){
  data_workspace = _data_workspace;
}

template<typename I,typename O>
void
BlackScholes<I,O>::Profile_WorkSpace( std::string const & _profile_workspace){
  profile_workspace = _profile_workspace;
}

template<typename I,typename O>
void
BlackScholes<I,O>::setFilterSignalPath()
{
   blackScholes_path = data_workspace + blackScholes_path;
   inputDataPath = data_workspace + inputDataPath;
   
}

template<typename I, typename O>
std::vector<typename::BlackScholes<I,O>::indexDateContainer>
BlackScholes<I,O>::readTransfIndex(){
  
 std::string path =  data_workspace +"/Output_Data/Scalability/BlackScholesLastDate.txt"; 
 char filename[ path.size() + 1];
 strcpy(filename, path.c_str());
 std::ifstream ifs(filename);
 std::string line;  std::vector<indexDateContainer> Index2sensorId; 
 // read one line from ifs
 while(std::getline(ifs, line)){
    std::istringstream iss(line); 
    int signal_Id;
    std::string date;
    
    iss >> signal_Id >> date ; 
    //std::cout << signal_Id << "  " << date << std::endl; 
    Index2sensorId.push_back(indexDateContainer(signal_Id,date));
    }
 return Index2sensorId;

}


template<typename I, typename O>
void
BlackScholes<I,O>::blackscholes_apply(std::vector<I> const & prices, int const signal_Index ) {

    int shift=2;
    I  call_strike_price = 2400;
    I  put_strike_price = 2300;
    I  risk_free_return = 0.01575;
    std::string expiry_date = "2024-11-23";       

  //compute the signa with standard deviation
  I sigma = sqrt(252) * calculate_std_dev(prices);
   
  //calculate the last closign price
  I last_closing_price = prices[prices.size()-1];
  //  auto last_date = prices[prices.size() - 1 - shift];
     
  std::vector<indexDateContainer> Index2sensorId  (readTransfIndex());


  std::string last_date   ( findSensorNameId(Index2sensorId, signal_Index) );
  //std::cout << signal_Index << "  " << last_date << std::endl;

  I years_until_expiry = compute_years_untill_expiry( last_date, expiry_date );
  //std::cout<< signal_Index << "  " << years_until_expiry << std::endl;
  years_until_expiry = 1.6;
  //sigma = 8.99179;
  //std::cout << last_closing_price << "  " << last_date <<"  " << expiry_date <<"  " << years_until_expiry <<  std::endl;    
  
  
   blackScholesFunction( last_closing_price
                       , call_strike_price
                       , put_strike_price
                       , years_until_expiry
                       , risk_free_return
                       , sigma);

   writeOutputTxtFile();
}


template<typename I, typename O>
std::string
BlackScholes<I,O>::findSensorNameId(std::vector<indexDateContainer> const & Index2Date
                                   ,int const signal_Index){
  
  auto it = std::find_if( Index2Date.begin()
                        , Index2Date.end()
                        , [&](indexDateContainer const & obj)
                            {
                                 return obj.signalIndex == signal_Index;
                            }
                 );     
   
   return it->date;
}

template<typename I,typename O>
std::vector<O> const &
BlackScholes<I,O>::getOutPut(){
 
   return OutputData;
}

template<typename I, typename O>
void
BlackScholes<I,O>::blackScholesFunction( I const last_closing_price
                                       , I const call_strike_price
                                       , I const put_strike_price
                                       , I const years_until_expiry
                                       , I const risk_free_return
                                       , I const sigma){

 

  O call_price = calculate_call_price(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma);
  OutputData.push_back(call_price);
 
  O put_price = calculate_put_price(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma);
  OutputData.push_back(put_price);
  
  OutputData.push_back(calculate_call_delta(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma));
  
  OutputData.push_back(calculate_put_delta(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma));
 
  //check untill here 
  OutputData.push_back(calculate_call_gamma(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma));

  OutputData.push_back(calculate_put_gamma(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma));
                
  OutputData.push_back(calculate_call_vega(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma));
 
  OutputData.push_back(calculate_put_vega(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma));

  OutputData.push_back(calculate_call_theta(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma));
   
  OutputData.push_back(calculate_put_theta(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma));

  OutputData.push_back(calculate_call_rho(last_closing_price, call_strike_price, years_until_expiry, risk_free_return, sigma));

  OutputData.push_back(calculate_put_rho(last_closing_price, put_strike_price, years_until_expiry, risk_free_return, sigma));

 // OutputData.push_back(calculate_call_implied_volatility(call_price, last_closing_price, call_strike_price, years_until_expiry, risk_free_return));
  
 // OutputData.push_back(calculate_put_implied_volatility(put_price, last_closing_price, put_strike_price, years_until_expiry, risk_free_return));
  
 
}
  

template<typename I, typename O>
void
BlackScholes<I,O>::writeOutputTxtFile(){
 
  std::cout << "call price " << OutputData[0] << std::endl; 
  std::cout << "put  price " << OutputData[1] << std::endl;
  std::cout << "call delta " << OutputData[2] << std::endl; 
  std::cout << "put  delta " << OutputData[3] << std::endl;
  std::cout << "call gamma " << OutputData[4] << std::endl; 
  std::cout << "put  gamma " << OutputData[5] << std::endl; 
  std::cout << "call vega  " << OutputData[6] << std::endl; 
  std::cout << "put  vega  " << OutputData[7] << std::endl; 
  std::cout << "call theta " << OutputData[8] << std::endl; 
  std::cout << "put  theta " << OutputData[9] << std::endl; 
  std::cout << "call rho   " << OutputData[10] << std::endl; 
  std::cout << "put  rho   " << OutputData[11] << std::endl; 
 // std::cout << "call implied volatility " << OutputData[12] << std::endl; 
 // std::cout << "put  implied volatility " << OutputData[13] << std::endl; 
 
}

template<typename I, typename O>
I const
BlackScholes<I,O>::compute_years_untill_expiry(std::string last_date, std::string expiry_date ) {

    // Convert the expiry date and last date strings to timepoints
    std::tm expiry_time = {};
    std::istringstream expiry_ss(expiry_date);
    expiry_ss >> std::get_time(&expiry_time, "%Y-%m-%d");
    std::time_t expiry_t = std::mktime(&expiry_time);

    std::tm last_time = {};
    std::istringstream last_ss(last_date);
    last_ss >> std::get_time(&last_time, "%Y-%m-%d");
    std::time_t last_t = std::mktime(&last_time);

    // Calculate the difference in seconds between expiry date and last date
    std::time_t diff_seconds = std::difftime(expiry_t, last_t);

    // Convert the difference in seconds to days
    int days_until_expiry = static_cast<int>(diff_seconds / 86400);

    // Calculate the number of years until expiry
    I years_until_expiry = static_cast<double>(days_until_expiry) / 365.0;

    // Print the result
   // std::cout << "Days until expiry: " << days_until_expiry << std::endl;
   // std::cout << "Years until expiry: " << years_until_expiry << std::endl;

   return years_until_expiry;
}



template<typename I, typename O>
O const
BlackScholes<I,O>::norm_cdf(I const x)
{
    return 0.5 * (1 + std::erf(x / std::sqrt(2)));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::norm_pdf(I const x) {

    return exp(-pow(x, 2)/2)/sqrt(2*M_PI);    
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_std_dev( std::vector<I> const & prices) {
    I mean = 0.0, sum = 0.0, sd = 0.0;
    int n = prices.size();

    for(int i = 0; i < n; ++i) {
        sum += prices[i];
    }

    mean = sum / n;

    for(int i = 0; i < n; ++i) {
        sd += pow(prices[i] - mean, 2);
    }

    return sqrt(sd / n);
}




template<typename I, typename O>
O const
BlackScholes<I,O>::d1(I const S, I const K, I const T, I const r, I const sigma) {
    return (log(S/K)+(r+pow(sigma, 2)/2.)*T)/(sigma*sqrt(T));
}


template<typename I, typename O>
O const
BlackScholes<I,O>::d2(I const S, I const K, I const T, I const r, I const sigma) {
    return d1(S, K, T, r, sigma) - sigma * sqrt(T);
}



template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_price(I const S, I const K, I const T, I const r, I const sigma) {
    I const call_price = S * norm_cdf(d1(S, K, T, r, sigma)) - K * exp(-r * T) * norm_cdf(d2(S, K, T, r, sigma));
    return call_price;
}


template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_price(I const S, I const K, I const T, I const r, I const sigma) {
    I const d_1 = d1(S, K, T, r, sigma);
    I const d_2 = d2(S, K, T, r, sigma);

    I const put_price = K*exp(-r*T)-S+calculate_call_price(S, K, T, r, sigma);
    return put_price;
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_delta(I const S, I const K, I const T, I const r, I const sigma) {
   
    I const d_1 = d1(S, K, T, r, sigma);
   
    I const call_delta = norm_cdf(d_1); 
    return call_delta;
}


template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_gamma(I const S, I const K, I const T, I const r, I const sigma) {

    return norm_pdf(d1(S, K, T, r, sigma))/(S*sigma*sqrt(T));
}


template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_vega(I const S, I const K, I const T, I const r, I const sigma) {
 
    return  0.01*(S*norm_pdf(d1(S, K, T, r, sigma))*sqrt(T));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_theta(I const S, I const K, I const T, I const r, I const sigma){
 
    return   0.01*(-(S*norm_pdf(d1(S, K, T, r, sigma))*sigma)/(2*sqrt(T)) - r*K*exp(-r*T)*norm_cdf(d2(S, K, T, r, sigma)));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_rho(I const S, I const K, I const T, I const r, I const sigma){
 
    return   0.01*(K*T*exp(-r*T)*norm_cdf(d2(S, K, T, r, sigma)));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_delta(I const S, I const K, I const T, I const r, I const sigma){
    return   -norm_cdf(-d1(S, K, T, r, sigma));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_gamma(I const S, I const K, I const T, I const r, I const sigma){
 
    return   norm_pdf(d1(S, K, T, r, sigma))/(S*sigma*sqrt(T));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_vega(I const S, I const K, I const T, I const r, I const sigma){
 
    return   0.01*(S*norm_pdf(d1(S, K, T, r, sigma))*sqrt(T));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_theta(I const S, I const K, I const T, I const r, I const sigma){
 
    return    0.01*(-(S*norm_pdf(d1(S, K, T, r, sigma))*sigma)/(2*sqrt(T)) + r*K*exp(-r*T)*norm_cdf(-d2(S, K, T, r, sigma)));
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_rho(I const S, I const K, I const T, I const r, I const sigma){
 
    return   0.01*(-K*T*exp(-r*T)*norm_cdf(-d2(S, K, T, r, sigma)));
}


template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_call_implied_volatility(I const Price, I const S, I const K, I const T, I const r){
    I sigma = 0.001;
    while (sigma < 1)
    {
        I Price_implied = S * norm_cdf(d1(S, K, T, r, sigma))
                            - K * exp(-r*T) * norm_cdf(d2(S, K, T, r, sigma));
        if (Price - Price_implied < 0.001)
        {
            return sigma;
        }
        sigma += 0.001;
    }
    return -1; // Not Found
}

template<typename I, typename O>
O const
BlackScholes<I,O>::calculate_put_implied_volatility(I const Price, I const S, I const K, I const T, I const r) {
    I sigma = 0.001;
    while (sigma < 1) {
        double Price_implied = K * std::exp(-r*T) - S + calculate_call_price(S, K, T, r, sigma);
        if (Price - Price_implied < 0.001) {
            return sigma;
        }
        sigma += 0.001;
    }
    return -1; // return "Not Found" or any other value to indicate no solution was found
}




/*
template<typename I,typename O>
void
BlackScholes<I,O>::signalDistribution()
{
   distribute = std::unique_ptr<signalDistributor<I> >(
                         new signalDistributor<I>(rank, size)
                                  );

   distribute->Data_IO_Path( inputDataPath);
   distribute->apply();
}



template<typename I,typename O>
void
BlackScholes<I,O>::loading_Signal()
{
   setFilterSignalPath();
   signalDistribution();
   for(int i(0); i< distribute->procSignalFiles().size(); i++){
         int classID = -1;
         std::string fullPath = distribute->procSignalFiles()[i];
         int signalIndex = distribute->procSignalIndex()[i];
         std::vector<priceContainer> const & inputSignalVal( readTxtFile(fullPath) );
       	 inputSignalContainerVec.push_back(signalContainerDate(signalIndex,inputSignalVal,classID));   
   }
  
}

template<typename I,typename O>
std::vector<O> const & 
BlackScholes<I,O>::apply_BlackScholes(std::vector<I> const & prices
                                     , int const & signal_Index){


  return blackscholes(prices, signal_Index );
   
   for(int i(0); i< distribute->procSignalFiles().size(); i++)
   {
      int classID = -1;
      std::vector<priceContainer> const & inputPrices(inputSignalContainerVec[i].signalVal);  
      int signalIndex = inputSignalContainerVec[i].signalId;  
      std::vector<O> OutputData (blackscholes(inputPrices));
      typename::signalContainer<O> outputSignal(signalIndex,OutputData,classID);
      outputSignalContainerVec.push_back(outputSignal);   
   }
   

}

template<typename I, typename O>
void
BlackScholes<I,O>::writeVecSignal(){
   bool write2TextFile(false); bool write2BinFile(true);
   for(int i(0); i<outputSignalContainerVec.size(); i++){
     int signalIndex = outputSignalContainerVec[i].signalId;
     std::vector<O> const & outputSignal (outputSignalContainerVec[i].signalVal);
     std::string freqsignal = blackScholes_path + "/signal_filter_"+ std::to_string(signalIndex); 
     if(write2BinFile){
       tool.writeBuffer2Binary( outputSignal , freqsignal);
     }
     if(write2TextFile){
       tool.writeBuffer2Text( outputSignal , freqsignal);
     }
   }  
}



template<typename I, typename O>
std::vector<typename::BlackScholes<I,O>::priceContainer>
BlackScholes<I,O>::readTxtFile(std::string const & path){

  char filename[ path.size() + 1];
 strcpy(filename, path.c_str());
 std::ifstream ifs(filename);
 std::string line;  std::vector<priceContainer> priceContainerBuffer; 
 // read one line from ifs
 while(std::getline(ifs, line)){
    std::istringstream iss(line); 
    int price_id;
    std::string price_date;
    std::string price_time;
    I   price_close;  
    
    iss >> price_id >> price_date >> price_time >> price_close ; 
    priceContainerBuffer.push_back(priceContainer(price_date,price_close));
    }

  return priceContainerBuffer;
 
}
*/

#endif