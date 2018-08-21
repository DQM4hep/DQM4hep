//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_SINGLETON_H
#define DQM4HEP_SINGLETON_H

namespace dqm4hep {

  namespace core {

    /** Singleton class
     */
    template <typename T>
    class Singleton {
    public:
      /** Constructor
       */
      Singleton();

      /** Destructor
       */
      ~Singleton();

      /** Return a unique instance of the class
       */
      static T *instance();

      /** Kill the unique instance of the class
       */
      static void kill();

    protected:
      static T *m_pInstance; ///< The unique instance of the class
    };

    //--------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------

    template <typename T>
    T *Singleton<T>::m_pInstance = nullptr;

    //--------------------------------------------------------------------------------------------

    template <typename T>
    Singleton<T>::Singleton() {
      /* nop */
    }

    //--------------------------------------------------------------------------------------------

    template <typename T>
    Singleton<T>::~Singleton() {
      /* nop */
    }

    //--------------------------------------------------------------------------------------------

    template <typename T>
    T *Singleton<T>::instance() {
      if (nullptr == m_pInstance)
        m_pInstance = new T();

      return m_pInstance;
    }

    //--------------------------------------------------------------------------------------------

    template <typename T>
    void Singleton<T>::kill() {
      if (nullptr == m_pInstance)
        return;

      delete m_pInstance;
      m_pInstance = nullptr;
    }
  }
}

#endif //  DQM4HEP_SINGLETON_H
