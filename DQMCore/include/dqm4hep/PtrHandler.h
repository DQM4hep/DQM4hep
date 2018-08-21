//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_PTRHANDLER_H
#define DQM4HEP_PTRHANDLER_H

// -- std headers
#include <cstddef>

namespace dqm4hep {

  namespace core {

    /** PtrHandler template class
     */
    template <typename T>
    class PtrHandler {
    public:
      /** Constructor
       */
      PtrHandler(T *ptr = nullptr, bool owner = true);

      /** Take the pointer, setting the handle pointer to nullptr.
       *  The ownership is transfered to the caller
       */
      T *take();

      /** Set the pointer and ownership
       */
      void set(T *ptr, bool owner = true);

      /** Clear the ptr.
       *  Delete if owned and set to nullptr
       */
      void clear();

      /** Get the ptr
       */
      T *ptr() const;

      /** Whether the handler owns the ptr
       */
      bool isOwner() const;

      /** Pointer operator
       */
      const T *operator->() const;

      /** Pointer operator
       */
      T *operator->();

      /** Reference operator
       */
      const T &operator*() const;

      /** Reference operator
       */
      T &operator*();

      /** Boolean operator
       */
      operator bool() const;

    private:
      T *m_ptr;     ///< The handled pointer
      bool m_owner; ///< Whether the pointer is owned by the handler

      template <typename S>
      friend bool operator==(const PtrHandler<S> &lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator==(const S *lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator==(const PtrHandler<S> &lhs, const S *rhs);
      template <typename S>
      friend bool operator!=(const PtrHandler<S> &lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator!=(const S *lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator!=(const PtrHandler<S> &lhs, const S *rhs);
      template <typename S>
      friend bool operator==(std::nullptr_t lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator==(const PtrHandler<S> &lhs, std::nullptr_t rhs);
      template <typename S>
      friend bool operator!=(std::nullptr_t lhs, const PtrHandler<S> &rhs);
      template <typename S>
      friend bool operator!=(const PtrHandler<S> &lhs, std::nullptr_t rhs);
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline PtrHandler<T>::PtrHandler(T *pointer, bool owner) : m_ptr(pointer), m_owner(owner) {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *PtrHandler<T>::take() {
      T *pointer = m_ptr;
      m_ptr = nullptr;
      return pointer;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void PtrHandler<T>::set(T *pointer, bool owner) {
      this->clear();
      m_ptr = pointer;
      m_owner = owner;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void PtrHandler<T>::clear() {
      if (m_owner && nullptr != m_ptr)
        delete m_ptr;
      m_ptr = nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *PtrHandler<T>::ptr() const {
      return m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool PtrHandler<T>::isOwner() const {
      return m_owner;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const T *PtrHandler<T>::operator->() const {
      return m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T *PtrHandler<T>::operator->() {
      return m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const T &PtrHandler<T>::operator*() const {
      return *m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline T &PtrHandler<T>::operator*() {
      return *m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline PtrHandler<T>::operator bool() const {
      return m_ptr != nullptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator==(const PtrHandler<T> &lhs, const PtrHandler<T> &rhs) {
      return lhs.m_ptr == rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator==(const T *lhs, const PtrHandler<T> &rhs) {
      return lhs == rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator==(const PtrHandler<T> &lhs, const T *rhs) {
      return lhs.m_ptr == rhs;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator!=(const PtrHandler<T> &lhs, const PtrHandler<T> &rhs) {
      return lhs.m_ptr != rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator!=(const T *lhs, const PtrHandler<T> &rhs) {
      return lhs != rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool operator!=(const PtrHandler<T> &lhs, const T *rhs) {
      return lhs.m_ptr != rhs;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename S>
    inline bool operator==(std::nullptr_t lhs, const PtrHandler<S> &rhs) {
      return lhs == rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename S>
    inline bool operator==(const PtrHandler<S> &lhs, std::nullptr_t rhs) {
      return lhs.m_ptr == rhs;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename S>
    inline bool operator!=(std::nullptr_t lhs, const PtrHandler<S> &rhs) {
      return lhs != rhs.m_ptr;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename S>
    inline bool operator!=(const PtrHandler<S> &lhs, std::nullptr_t rhs) {
      return lhs.m_ptr != rhs;
    }
  }
}

#endif //  DQM4HEP_LOGGING_H
