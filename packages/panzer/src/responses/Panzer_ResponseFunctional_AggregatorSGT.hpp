#ifndef __Panzer_ResponseFunctional_AggregatorSGT_hpp__
#define __Panzer_ResponseFunctional_AggregatorSGT_hpp__

#include "Panzer_config.hpp"

#include <string>
#include <vector>

#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterList.hpp"

#include "Panzer_ResponseScatterEvaluator.hpp"

namespace panzer {

// useful for cloning and the factory mechanism
template <typename TraitsT>
ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
ResponseFunctional_Aggregator() 
{}

template <typename TraitsT>
ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
ResponseFunctional_Aggregator(const Teuchos::ParameterList & p) 
{}

template <typename TraitsT>
Teuchos::RCP<ResponseAggregatorBase<TraitsT> > 
ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
clone(const Teuchos::ParameterList & p) const
{ 
   return Teuchos::rcp(new ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>(p)); 
}

//! Build response data for a specified set of fields (ResponseAggregator decides data layout)
template <typename TraitsT>
Teuchos::RCP<ResponseData<TraitsT> > 
ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
buildResponseData(const std::vector<std::string> & fields) const
{
   // simply build data object fully allocated and initialized
   Teuchos::RCP<ResponseData<TraitsT> > data  
         = Teuchos::rcp(new ResponseFunctional_Data<panzer::Traits::SGResidual,TraitsT>());
   data->allocateAndInitializeData(fields);
   
   return data;
}

//! Build an evaluator ofr the set of fields to be aggregated (calculated) together
template <typename TraitsT>
void ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
registerAndRequireEvaluators(PHX::FieldManager<TraitsT> & fm,const Teuchos::RCP<ResponseData<TraitsT> > & data,
                             const Teuchos::ParameterList & p) const
{
   typedef ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT> ThisType;

   // build useful evaluator
   Teuchos::RCP<PHX::Evaluator<TraitsT> > eval = Teuchos::rcp(
         new ResponseScatterEvaluator<panzer::Traits::SGResidual,TraitsT,ThisType>("Functional Response",
                                                                        data,
                                                                        Teuchos::rcpFromRef(*this),
                                                                        data->getFields(),
                                                                        p.get<int>("Workset Size")));

   // add and require fields from aggregator constructed evaluator
   fm.template registerEvaluator<panzer::Traits::SGResidual>(eval);
   for(std::size_t i=0;i<eval->evaluatedFields().size();i++)
      fm.template requireField<panzer::Traits::SGResidual>(*(eval->evaluatedFields()[i]));
}

//! Aggregate fields into a specific data object
template <typename TraitsT>
template <typename FieldT>
void ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
evaluateFields(panzer::Workset & wkst,ResponseData<TraitsT> & in_data,
               const std::vector<FieldT> & fields) const
{
   ResponseFunctional_Data<panzer::Traits::SGResidual,TraitsT> & data 
         = Teuchos::dyn_cast<ResponseFunctional_Data<panzer::Traits::SGResidual,TraitsT> >(in_data); // dynamic cast to correct data type

   std::vector<typename TraitsT::SGType> & dataVec = data.getData();

   TEUCHOS_ASSERT(fields.size()==dataVec.size()); // sanity check

   // loop over reponse fields
   for(std::size_t i=0;i<fields.size();i++) {
      const PHX::MDField<panzer::Traits::SGResidual::ScalarT,Cell> & field = fields[i]; // this also forces type

      // loop over cells
      for(std::size_t c=0;c<wkst.num_cells;c++)
         dataVec[i] += field(c);
   }
}

//! perform global reduction on this set of response data
template <typename TraitsT>
void
ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
globalReduction(const Teuchos::Comm<int> & comm,ResponseData<TraitsT>  & rd) const
{
/*
   std::vector<typename TraitsT::RealType> & dataVec = 
      Teuchos::dyn_cast<ResponseFunctional_Data<panzer::Traits::SGResidual,TraitsT> >(rd).getData();
   std::vector<typename TraitsT::RealType> dataVec2 = dataVec;

  // do communication
  Teuchos::reduceAll(comm, Teuchos::REDUCE_SUM, static_cast<int>(dataVec.size()),
                     &dataVec2[0], &dataVec[0]);
*/
}

template <typename TraitsT>
void ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>::
aggregateResponses(Response<TraitsT> & dest,const std::list<Teuchos::RCP<const Response<TraitsT> > > & sources) const
{
   typename TraitsT::SGType val = dest.getSGValue();

   // sum over all values
   typename std::list<Teuchos::RCP<const Response<TraitsT> > >::const_iterator itr;
   for(itr=sources.begin();itr!=sources.end();++itr)
      val += (*itr)->getSGValue();      

   dest.setSGValue(val);
}

}

#endif
