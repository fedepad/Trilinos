#ifndef __Panzer_ResponseFunctional_Aggregator_hpp__
#define __Panzer_ResponseFunctional_Aggregator_hpp__

#include "Panzer_config.hpp"

#include <string>
#include <vector>

#include "Panzer_Traits.hpp"
#include "Panzer_ResponseAggregatorBase.hpp"

#include "Teuchos_RCP.hpp"
#include "Teuchos_ParameterList.hpp"

namespace panzer {

/** Abstract data object for response storage. Also manages
  * setting up and transfering to the responses. This works
  * in concert with the response aggregator.
  */
template <typename EvalT,typename TraitsT> class ResponseFunctional_Data;
template <typename EvalT,typename TraitsT> class ResponseFunctional_Aggregator;

template <typename TraitsT> 
class ResponseFunctional_Data<panzer::Traits::Residual,TraitsT> 
   : public ResponseDataDefault<TraitsT> {
public:
   ResponseFunctional_Data() {}

   /** \defgroup Required from ResponseData
     * @{
     */

   //! Allocate and initialize required storage based on the fields passed in
   virtual void allocateAndInitializeData(const std::vector<std::string> & fields)
   { 
      this->setFields(fields);

      data_.resize(fields.size());
      reinitializeData();
   }

   /** Reinitialize the data based on the fields original requested by
     * <code>allocateAndInitializeData</code>.
     */
   virtual void reinitializeData()
   { for(std::size_t i=0;i<data_.size();i++) data_[i] = 0.0; }
   

   /** Fill a response object with data from a particular field.
     */
   virtual void fillResponse(const std::string & field,Response<TraitsT> & response) const
   {
      std::size_t index = this->fieldIndex(field);
      TEUCHOS_TEST_FOR_EXCEPTION(index>=this->getFields().size(),std::logic_error,
                         "Cannnot find field \""+field+"\" in ReponseFunctional_Data object.");
      response.setValue(data_[index]);
   }

   //! @}
 
   //! get the vector with data (writable)
   std::vector<typename TraitsT::RealType> & getData() 
   { return data_; }

   //! get the vector with data (non-writable)
   const std::vector<typename TraitsT::RealType> & getData() const
   { return data_; }

private:
   //! values stored
   std::vector<typename TraitsT::RealType> data_;
};

#ifdef HAVE_STOKHOS
template <typename TraitsT> 
class ResponseFunctional_Data<panzer::Traits::SGResidual,TraitsT> 
   : public ResponseDataDefault<TraitsT> {
public:
   ResponseFunctional_Data() {}

   /** \defgroup Required from ResponseData
     * @{
     */

   //! Allocate and initialize required storage based on the fields passed in
   virtual void allocateAndInitializeData(const std::vector<std::string> & fields)
   { 
      this->setFields(fields);

      data_.resize(fields.size());
      reinitializeData();
   }

   /** Reinitialize the data based on the fields original requested by
     * <code>allocateAndInitializeData</code>.
     */
   virtual void reinitializeData()
   { for(std::size_t i=0;i<data_.size();i++) data_[i] = 0.0; }

   /** Fill a response object with data from a particular field.
     */
   virtual void fillResponse(const std::string & field,Response<TraitsT> & response) const
   {
      std::size_t index = this->fieldIndex(field);
      TEUCHOS_TEST_FOR_EXCEPTION(index>=this->getFields().size(),std::logic_error,
                         "Cannnot find field \""+field+"\" in ReponseFunctional_Data object.");
      response.setSGValue(data_[index]);
   }

   //! @}
 
   //! get the vector with data (writable)
   std::vector<typename TraitsT::SGType> & getData() 
   { return data_; }

   //! get the vector with data (non-writable)
   const std::vector<typename TraitsT::SGType> & getData() const
   { return data_; }

private:
   //! values stored
   std::vector<typename TraitsT::SGType> data_;
};
#endif

template <typename TraitsT>
class ResponseFunctional_Aggregator<panzer::Traits::Residual,TraitsT>
   : public ResponseAggregator<panzer::Traits::Residual,TraitsT> {
public:
   // useful for cloning and the factory mechanism
   ResponseFunctional_Aggregator();

   ResponseFunctional_Aggregator(const Teuchos::ParameterList & p);

   /** \defgroup Methods required by ResponseAggregatorBase
     * @{
     */

   //! Clone this aggregator
   Teuchos::RCP<ResponseAggregatorBase<TraitsT> > clone(const Teuchos::ParameterList & p) const;

   //! Build response data for a specified set of fields (ResponseAggregator decides data layout)
   Teuchos::RCP<ResponseData<TraitsT> > buildResponseData(const std::vector<std::string> & fields) const;

   //! Register and build evaluator required by this aggregator and this set of data.
   virtual void registerAndRequireEvaluators(PHX::FieldManager<TraitsT> & fm,const Teuchos::RCP<ResponseData<TraitsT> > & data, 
                                             const Teuchos::ParameterList & p) const;

   //! perform global reduction on this set of response data
   void globalReduction(const Teuchos::Comm<int> & comm,ResponseData<TraitsT>  & rd) const;

   //! @}

   //! Aggregate fields into a data object
   template <typename FieldT>
   void evaluateFields(panzer::Workset & wkst,ResponseData<TraitsT> & data,
                       const std::vector<FieldT> & fields) const;

   //! Aggregate a set of responses locally
   virtual void aggregateResponses(Response<TraitsT> & dest,const std::list<Teuchos::RCP<const Response<TraitsT> > > & sources) const;

};

#ifdef HAVE_STOKHOS
template <typename TraitsT>
class ResponseFunctional_Aggregator<panzer::Traits::SGResidual,TraitsT>
   : public ResponseAggregator<panzer::Traits::SGResidual,TraitsT> {
public:
   // useful for cloning and the factory mechanism
   ResponseFunctional_Aggregator();

   ResponseFunctional_Aggregator(const Teuchos::ParameterList & p);

   /** \defgroup Methods required by ResponseAggregatorBase
     * @{
     */

   //! Clone this aggregator
   Teuchos::RCP<ResponseAggregatorBase<TraitsT> > clone(const Teuchos::ParameterList & p) const;

   //! Build response data for a specified set of fields (ResponseAggregator decides data layout)
   Teuchos::RCP<ResponseData<TraitsT> > buildResponseData(const std::vector<std::string> & fields) const;

   //! Register and build evaluator required by this aggregator and this set of data.
   virtual void registerAndRequireEvaluators(PHX::FieldManager<TraitsT> & fm,const Teuchos::RCP<ResponseData<TraitsT> > & data, 
                                             const Teuchos::ParameterList & p) const;

   //! perform global reduction on this set of response data
   void globalReduction(const Teuchos::Comm<int> & comm,ResponseData<TraitsT>  & rd) const;

   //! @}

   //! Aggregate fields into a data object
   template <typename FieldT>
   void evaluateFields(panzer::Workset & wkst,ResponseData<TraitsT> & data,
                       const std::vector<FieldT> & fields) const;

   //! Aggregate a set of responses locally
   virtual void aggregateResponses(Response<TraitsT> & dest,const std::list<Teuchos::RCP<const Response<TraitsT> > > & sources) const;

};
#endif

// Specialized for panzer::Traits
class ResponseFunctional_Aggregator_Builder {
public:
   void setGlobalIndexer(const Teuchos::RCP<UniqueGlobalIndexer<int,int> > & ugi)
   { globalIndexer_ = ugi; }

   void setLinearObjFactory(const Teuchos::RCP<LinearObjFactory<panzer::Traits> > & lof)
   { linObjFactory_ = lof; }

   Teuchos::RCP<UniqueGlobalIndexer<int,int> > getGlobalIndexer() const
   { return globalIndexer_; }

   Teuchos::RCP<LinearObjFactory<panzer::Traits> > getLinearObjFactory() const
   { return linObjFactory_; }

   template <typename EvalT>
   Teuchos::RCP<ResponseAggregatorBase<panzer::Traits> > build() const
   { return Teuchos::null; }

private:
   Teuchos::RCP<UniqueGlobalIndexer<int,int> > globalIndexer_;
   Teuchos::RCP<LinearObjFactory<panzer::Traits> > linObjFactory_;
};

// declaration so methods are not inlined
template < >
Teuchos::RCP<ResponseAggregatorBase<panzer::Traits> > ResponseFunctional_Aggregator_Builder::build<panzer::Traits::Residual>() const;

#ifdef HAVE_STOKHOS
template < >
Teuchos::RCP<ResponseAggregatorBase<panzer::Traits> > ResponseFunctional_Aggregator_Builder::build<panzer::Traits::SGResidual>() const;
#endif

}

#ifndef PANZER_EXPLICIT_TEMPLATE_INSTANTIATION
#include "Panzer_ResponseFunctional_AggregatorT.hpp"
#ifdef HAVE_STOKHOS
   #include "Panzer_ResponseFunctional_AggregatorSGT.hpp"
#endif
#endif

#endif
