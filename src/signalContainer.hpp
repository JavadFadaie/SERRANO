#ifndef SIGNALCONTAINER_HPP
#define SIGNALCONTAINER_HPP

template<typename I>
class signalContainer{
         
         public:
          signalContainer
            ( int _signalId
            , std::vector<I> const & _signalVal
            , int _classID)
            :signalId(_signalId)
            ,signalVal(_signalVal)
            ,classID(_classID)
            {}             
        
          signalContainer()
          :signalId(0)
          ,signalVal(0,0)
          ,classID(0)
          {}

          
          
          public : 
          int  signalId;
          std::vector<I> signalVal;
          int classID; 

          std::string outputpath;
          bool outputBinaryFile;
};

template<typename I>
class signalContainerAcceleration{
         
         public:
        signalContainerAcceleration( int _signal_GId
                       , int _signal_LId
                       , int _group_Id
                       , int _class_Id
                       , std::vector<I> const & _signal_Val)
         :signal_GId(_signal_GId)
         ,signal_LId(_signal_LId)
         ,group_Id(_group_Id)
         ,class_Id(_class_Id)
         ,signal_Val(_signal_Val)
         {}              
        
        signalContainerAcceleration()
         :signal_GId(0)
         ,signal_LId(0)
         ,group_Id(0)
         ,class_Id(-1)
         ,signal_Val(0,0)
         {}

        public : 
         int signal_GId;
         int signal_LId;
         int group_Id;
         int class_Id;
         std::vector<I> signal_Val;
      };


 


struct Index2Sensor{
     Index2Sensor
        ( std::string _sensor_Id
        , int _id)
        :sensor_Id(_sensor_Id)
        ,id(_id)
        {}

    std::string sensor_Id;
    int id;
   };



#endif