//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_ALLOCATORHELPER_H
#define DQM4HEP_ALLOCATORHELPER_H

// -- std headers
#include <string>

// -- dqm4hep headers
#include <dqm4hep/RootHeaders.h>

namespace dqm4hep {

  namespace core {
    
    template <typename T>
    class TScalarObject;
    class TDynamicGraph;
    class TiXmlElement;

    /**
     *  @brief  TObjectXMLAllocator class
     */
    class TObjectXMLAllocator {
    public:
      /**
       *  @brief  Destructor
       */
      virtual ~TObjectXMLAllocator() {}
      /**
       *  @brief  Create an instance of a sub class of TObject from the xml element 
       *
       * @param  element the input xml element describing the object to allocate
       */
      virtual TObject* create(TiXmlElement *element) const = 0;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    /// TXMLDefaultAllocator class for all ROOT class with default constructor 
    class DefaultXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TH1XMLAllocator class for all TH1 types 
    class TH1XMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TH2XMLAllocator class for all TH2 types 
    class TH2XMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TH3XMLAllocator class for all TH3 types 
    class TH3XMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// THStackXMLAllocator class for all THStack type
    class THStackXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// THStackXMLAllocator class for all TH2Poly type
    class TH2PolyXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TProfileXMLAllocator class for all TProfile type
    class TProfileXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TProfile2DXMLAllocator class for all TProfile2D type
    class TProfile2DXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /// TScalarXMLAllocator class for all scalar types
    class TScalarXMLAllocator final : public TObjectXMLAllocator {
      TObject* create(TiXmlElement *element) const override;
    };
    
  }
  
}

#endif //  DQM4HEP_ALLOCATORHELPER_H
