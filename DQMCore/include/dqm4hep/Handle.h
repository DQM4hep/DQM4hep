//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_HANDLE_H
#define DQM4HEP_HANDLE_H

// -- root headers
#include <TClass.h>

// -- std headers
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <functional>

namespace dqm4hep {
  
  namespace core {
    
    /**
     *  @brief  Handle class
     *  Non-template class handling a template pointer
     *  in a type-safe manner
     */
    class Handle {
    public:
      Handle() = delete;
      ~Handle() = default;
      Handle(const Handle &) = delete;
      Handle(Handle &&) = default;
      Handle &operator=(const Handle &) = delete;
      
      /**
       *  @brief  Constructor
       *
       *  @param  ptr a raw pointer to handle
       */
      template <typename T>
      Handle(T *ptr);
      
      /**
       *  @brief  Constructor
       *
       *  @param  ptr a shared pointer to handle
       */
      template <typename T>
      Handle(std::shared_ptr<T> ptr);
      
      /**
       *  @brief  Retrieve the handled pointer as a specific type
       */
      template <typename T>
      std::shared_ptr<T> ptr() const;
      
      /**
       *  @brief  Create a new handle based on the same underlying type
       */
      Handle create() const;
      
      /**
       *  @brief  Retrieve the handled pointer as raw pointer
       */
      void *raw();
      
      /**
       *  @brief  Retrieve the handled pointer as raw pointer (const version)
       */
      const void *raw() const;
      
      /**
       *  @brief  Retrieve the handled pointer type information
       */
      const std::type_index &typeInfo() const;
      
      /**
       *  @brief  Whether the handled pointer is valid (non nullptr)
       */
      bool isValid() const;
      
    protected:
      /// The handled pointer
      std::shared_ptr<void>        m_ptr = {nullptr};
      /// The type information of the handled type
      std::type_index              m_typeInfo;
      /// A custom allocator function based on the underlying type
      std::function<Handle(void)>  m_newFunction = {nullptr};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    /**
     *  @brief  ROOTHandle class
     *  Special handle for ROOT objects. ROOT I/O API requires to 
     *  know the type or at least the TClass of the object to read or write
     */
    class ROOTHandle : public Handle {
    public:
      ROOTHandle() = delete;
      ~ROOTHandle() = default;
      ROOTHandle(const ROOTHandle &) = delete;
      ROOTHandle(ROOTHandle &&) = default;
      ROOTHandle &operator=(const ROOTHandle &) = delete;
      
      /**
       *  @brief  Constructor
       *
       *  @param  ptr a ROOT object ptr to handle
       */
      template <typename T>
      ROOTHandle(T *ptr);
      
      /**
       *  @brief  Constructor
       *
       *  @param  ptr a ROOT object shared ptr to handle
       */
      template <typename T>
      ROOTHandle(std::shared_ptr<T> ptr);
      
      /**
       *  @brief  Create a new handle based on the same underlying type
       */
      ROOTHandle create() const;
      
      /**
       *  @brief  Get the ROOT object class
       */
      TClass *getClass() const;
      
    private:
      /// The ROOT object class
      TClass           *m_class = {nullptr};
    };
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline Handle::Handle(T *ptr) : 
      m_ptr(ptr),
      m_typeInfo(typeid(T)) {
      m_newFunction = []() {
        return Handle(new T());
      };
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline Handle::Handle(std::shared_ptr<T> ptr) :
      m_ptr(ptr),
      m_typeInfo(typeid(T)) {
      m_newFunction = []() {
        return Handle(new T());
      };
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline std::shared_ptr<T> Handle::ptr() const {
      return std::static_pointer_cast<T>(m_ptr);
    }
    
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline ROOTHandle::ROOTHandle(T *ptr) :
      Handle(ptr),
      m_class(TClass::GetClass(typeid(T))) {
      m_newFunction = []() {
        return ROOTHandle(new T());
      };
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline ROOTHandle::ROOTHandle(std::shared_ptr<T> ptr) :
      Handle(ptr),
      m_class(TClass::GetClass(typeid(T))) {
      m_newFunction = []() {
        return ROOTHandle(new T());
      };
    }
    
  }
  
}

#endif //  DQM4HEP_HANDLE_H
