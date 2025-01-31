//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_DIRECTORY_H
#define DQM4HEP_DIRECTORY_H

// -- dqm4hep headers
#include <dqm4hep/Internal.h>
#include <dqm4hep/Path.h>
#include <dqm4hep/StatusCodes.h>

namespace dqm4hep {

  namespace core {

    /** Directory class.
     *
     *  A directory is the owner of its sub-directories.
     *  Unit owning depends on how the method
     *  are called (default : not owner)
     *
     *  This interface doesn't allow moving sub-directories
     */
    template <typename T>
    class Directory : public std::enable_shared_from_this<Directory<T>> {
    public:
      typedef std::shared_ptr<T> ObjectPtr;
      typedef std::vector<ObjectPtr> ObjectList;
      typedef std::shared_ptr<Directory<T>> DirectoryPtr;
      typedef std::vector<DirectoryPtr> DirectoryList;

      /** Factory method for simple constructor
       */
      static DirectoryPtr make_shared();

      /** Factory method for constructor with name and parent directory
       */
      static DirectoryPtr make_shared(const std::string &name, DirectoryPtr parent = nullptr);

      /** Destructor.
       *  Delete the subdirectories but not the contents
       */
      ~Directory();

      /** Create a new directory
       */
      DirectoryPtr mkdir(const std::string &dirName);

      /** Returns the directory name
       */
      const std::string &name() const;

      /** Returns the parent directory
       */
      DirectoryPtr parent() const;

      /** Get the sub directory list
       */
      const DirectoryList &subdirs() const;

      /** Whether the directory contains the sub dir
       */
      bool hasChild(const std::string &dirName) const;

      /** Find the sub directory by name
       */
      StatusCode find(const std::string &dirName, DirectoryPtr &directory) const;

      /** Add an object to the directory
       */
      StatusCode add(ObjectPtr object);

      /** Find a monitor element with a given name in the directory
       */
      template <typename F>
      ObjectPtr find(F function) const;

      /** Whether the directory contains the monitor element (search by ptr compare)
       */
      bool containsObject(const ObjectPtr &object) const;

      /** Whether the directory contains the monitor element (search by name compare)
       */
      template <typename F>
      bool contains(F function) const;

      /** Remove the monitor element from the directory
       */
      StatusCode remove(ObjectPtr object);

      /** Remove the monitor element from the directory
       */
      template <typename F>
      StatusCode remove(F function);

      /** Get the monitor element list
       */
      const ObjectList &contents() const;

      /** Remove the directory and its contents
       */
      StatusCode rmdir(const std::string &dirName);

      /** Clear the directory.
       *  The operation is done recursively
       */
      void clear();

      /** Get the full path name of the directory
       */
      Path fullPath() const;

      /** Whether the directory is a root directory
       */
      bool isRoot() const;

      /** Whether the directory is empty
       */
      bool isEmpty() const;
      
      /** Get the number of parent directories
       */
      int nParents() const;

    private:
      /** Default constructor
       */
      Directory();

      /** Constructor with name and parent dir
       */
      Directory(const std::string &name, DirectoryPtr parent = nullptr);

    private:
      std::string m_name;
      DirectoryPtr m_parent;
      DirectoryList m_subdirs;
      ObjectList m_contents;
    };

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Directory<T>::DirectoryPtr Directory<T>::make_shared() {
      return std::shared_ptr<Directory<T>>(new Directory<T>());
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Directory<T>::DirectoryPtr Directory<T>::make_shared(const std::string &name, DirectoryPtr parent) {
      return std::shared_ptr<Directory<T>>(new Directory<T>(name, parent));
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Directory<T>::Directory() : m_name(""), m_parent(nullptr), m_subdirs(), m_contents() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Directory<T>::Directory(const std::string &dname, DirectoryPtr dparent)
        : m_name(dname), m_parent(dparent), m_subdirs(), m_contents() {
      /* nop */
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Directory<T>::~Directory() {
      this->clear();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const std::string &Directory<T>::name() const {
      return m_name;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Directory<T>::DirectoryPtr Directory<T>::parent() const {
      return m_parent;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const typename Directory<T>::DirectoryList &Directory<T>::subdirs() const {
      return m_subdirs;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline typename Directory<T>::DirectoryPtr Directory<T>::mkdir(const std::string &dirName) {
      DirectoryPtr directory;

      this->find(dirName, directory);

      if (nullptr != directory)
        return directory;

      if (dirName.find("/") != std::string::npos || dqm4hep::core::containsSpecialCharacters(dirName) ||
          dirName.empty())
        return nullptr;

      auto newDirectory = Directory<T>::make_shared(dirName, this->shared_from_this());
      m_subdirs.push_back(newDirectory);

      return newDirectory;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Directory<T>::hasChild(const std::string &dirName) const {
      if (dirName.find("/") != std::string::npos || dqm4hep::core::containsSpecialCharacters(dirName) ||
          dirName.empty())
        return false;

      for (auto iter = m_subdirs.begin(), endIter = m_subdirs.end(); endIter != iter; ++iter) {
        auto directory = *iter;

        if (directory->name() == dirName)
          return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Directory<T>::find(const std::string &dirName, DirectoryPtr &directory) const {
      directory = nullptr;

      for (auto iter = m_subdirs.begin(), endIter = m_subdirs.end(); endIter != iter; ++iter) {
        auto dir = *iter;

        if (dir->name() == dirName) {
          directory = dir;
          return STATUS_CODE_SUCCESS;
        }
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Directory<T>::add(ObjectPtr object) {
      if (nullptr == object)
        return STATUS_CODE_INVALID_PTR;

      if (this->containsObject(object))
        return STATUS_CODE_ALREADY_PRESENT;

      m_contents.push_back(object);

      return STATUS_CODE_SUCCESS;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline typename Directory<T>::ObjectPtr Directory<T>::find(F function) const {
      for (auto iter = m_contents.begin(), endIter = m_contents.end(); endIter != iter; ++iter)
        if (function(*iter))
          return *iter;

      return ObjectPtr();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Directory<T>::containsObject(const ObjectPtr &object) const {
      if (nullptr == object)
        return false;

      for (auto iter = m_contents.begin(), endIter = m_contents.end(); endIter != iter; ++iter)
        if (object == *iter)
          return true;

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline bool Directory<T>::contains(F function) const {
      for (auto iter = m_contents.begin(), endIter = m_contents.end(); endIter != iter; ++iter)
        if (function(*iter))
          return true;

      return false;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Directory<T>::remove(ObjectPtr object) {
      if (nullptr == object)
        return STATUS_CODE_INVALID_PTR;

      for (auto iter = m_contents.begin(), endIter = m_contents.end(); endIter != iter; ++iter) {
        if (object == *iter) {
          m_contents.erase(iter);
          return STATUS_CODE_SUCCESS;
        }
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    template <typename F>
    inline StatusCode Directory<T>::remove(F function) {
      for (auto iter = m_contents.begin(), endIter = m_contents.end(); endIter != iter; ++iter) {
        if (function(*iter)) {
          m_contents.erase(iter);
          return STATUS_CODE_SUCCESS;
        }
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline const typename Directory<T>::ObjectList &Directory<T>::contents() const {
      return m_contents;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline StatusCode Directory<T>::rmdir(const std::string &dirName) {
      for (auto iter = m_subdirs.begin(), endIter = m_subdirs.end(); endIter != iter; ++iter) {
        DirectoryPtr dir = *iter;

        if (dir->name() == dirName) {
          m_subdirs.erase(iter);
          return STATUS_CODE_SUCCESS;
        }
      }

      return STATUS_CODE_NOT_FOUND;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline void Directory<T>::clear() {
      for (auto iter = m_subdirs.begin(), endIter = m_subdirs.end(); endIter != iter; ++iter)
        (*iter)->clear();

      m_subdirs.clear();
      m_contents.clear();
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline Path Directory<T>::fullPath() const {
      Path fPath = this->name();
      DirectoryPtr dir = const_cast<Directory<T> *>(this)->shared_from_this();

      while (true) {
        DirectoryPtr parentDir = dir->parent();

        if (nullptr != parentDir) {
          fPath = parentDir->name() + fPath;
          dir = parentDir;
        } else {
          fPath = Path("/") + fPath;
          break;
        }
      }

      return fPath;
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Directory<T>::isRoot() const {
      return (nullptr == m_parent);
    }

    //-------------------------------------------------------------------------------------------------

    template <typename T>
    inline bool Directory<T>::isEmpty() const {
      return (m_subdirs.empty() && m_contents.empty());
    }
    
    //-------------------------------------------------------------------------------------------------
    
    template <typename T>
    inline int Directory<T>::nParents() const {
      if(isRoot()) {
        return 0;
      }
      return parent()->nParents() + 1;
    }
  }
}

#endif //  DQM4HEP_DIRECTORY_H
