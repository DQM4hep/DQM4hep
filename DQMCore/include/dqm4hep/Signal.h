//==========================================================================
//  DQM4hep a data quality monitoring software for high energy physics 
//--------------------------------------------------------------------------
//
// For the licensing terms see $DQM4hep_DIR/LICENSE.
// For the list of contributors see $DQM4hep_DIR/AUTHORS.
//
// Author     : R.Ete
//====================================================================

#ifndef DQM4HEP_SIGNAL_H
#define DQM4HEP_SIGNAL_H

namespace dqm4hep {

  namespace core {

    /**
     *  @brief  SignalBase class (internal)
     *          Base class for emitting signals
     */
    template <typename... Args>
    class SignalBase {
    public:
      /**
       *  @brief  Destructor
       */
      virtual ~SignalBase() {
        /* nop */
      }

      /**
      *  @brief  Emit the signal
      */
      virtual void emit(Args... args) = 0;
    };

    /**
     *  @brief  SignalBase specialization (internal)
     *          No argument in callback function
     */
    template <>
    class SignalBase<void> {
    public:
      /**
       *  @brief  Destructor
       */
      virtual ~SignalBase() {
        /* nop */
      }

      /**
      *  @brief  Emit the signal
      */
      virtual void emit() = 0;
    };

    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    /**
    *  @brief  SignalT class (internal)
    */
    template <typename T, typename... Args>
    class SignalT : public SignalBase<Args...> {
    public:
      typedef void (T::*Function)(Args...);
      SignalT() = delete;
      SignalT(const SignalT<T, Args...>&) = delete;
      SignalT<T, Args...>& operator=(const SignalT<T, Args...>&) = delete;

      /**
      *  @brief  Constructor
      */
      SignalT(T *obj, Function func);

      /**
      *  @brief  Emit the signal
      *
      * @param  args the arguments to forward
      */
      void emit(Args... args) override;

      /**
       *  @brief  Get the class object
       */
      const T *object() const;

      /**
       *  @brief  Get the object member-function 
       */
      Function function() const;

    private:
      /// The class object
      T              *m_object = {nullptr};
      /// The object member-function
      Function        m_function = {};
    };

    // SPECIALIZATION - CLASS + VOID
    template <typename T>
    class SignalT<T, void> : public SignalBase<void> {
    public:
      typedef void (T::*Function)();
      SignalT() = delete;
      SignalT(const SignalT<T, void>&) = delete;
      SignalT<T, void>& operator=(const SignalT<T, void>&) = delete;

      /**
      *  @brief  Constructor with
      */
      SignalT(T *obj, Function func);

      /**
      *  @brief  Process the callback
      */
      void emit() override;

      /**
       *  @brief  Get the class object
       */
      const T *object() const;

      /**
       *  @brief  Get the object member-function 
       */
      Function function() const;

    private:
      /// The class object
      T              *m_object = {nullptr};
      /// The object member-function
      Function        m_function = {};
    };
    
    /// SPECIALIZATION - PURE FUNCTION + ARGS
    template <typename... Args>
    class SignalT<void, Args...> : public SignalBase<Args...> {
    public:
      typedef void (*Function)(Args...);

      /**
      *  @brief  Constructor
      */
      SignalT(Function func);

      /**
      *  @brief  Emit the signal
      */
      void emit(Args... args) override;

      /**
       *  @brief  Get the associated function 
       */
      Function function() const;

    private:
      /// The associated callback function
      Function        m_function = {};
    };
    
    // SPECIALIZATION - PURE FUNCTION + NO ARGS
    template <>
    class SignalT<void, void> : public SignalBase<void> {
    public:
      typedef void (*Function)();

      /**
      *  @brief  Constructor
      */
      SignalT(Function func);

      /**
      *  @brief  Emit the signal
      */
      void emit() override;

      /**
       *  @brief  Get the associated function 
       */
      Function function() const;

    private:
      /// The associated callback function
      Function        m_function = {};
    };

    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    /**
     *  @brief Signal class
     *
     *  Implement the observer pattern a la Qt.
     *  Example:
     *  @code{.cpp}
     *
     *  void func() {
     *    std::cout << "Hello world !" << std::endl;
     *  }
     *
     *  class Toto {
     *  public:
     *    void hello() {
     *      std::cout << "Toto world !" << std::endl;
     *    }
     *  };
     *  
     *  Signal<> sig;
     *  Toto toto;
     *  sig.connect(func);
     *  sig.connect(&toto, &Toto::hello);
     *  sig.emit();
     *  @endcode
     *
     *  Allows nice and understandable piece of code like:
     *  @code{.cpp}
     *  // Holds a property and signal
     *  class Property {
     *  public:
     *    void setProperty(int value) {
     *      m_property = value;
     *      m_onUpdate.emit(m_property);
     *    }
     *    Signal<int> &onUpdate() {
     *      return m_onUpdate;
     *    }
     *  private:
     *    int           m_property = {0};
     *    Signal<int>   m_onUpdate = {};
     *  };
     *
     *  // To print the property !
     *  class Printer {
     *  public:
     *    void print(int value) {
     *      std::cout << "Value is " << value << std::endl;
     *    }
     *  };
     *  
     *  Printer printer;
     *  Property property;
     *  property.onUpdate().connect(&printer, &Printer::print); // I love this line !
     *  property.setProperty(42); // Calls 'print' method
     *  @endcode
     */
    template <typename... Args>
    class Signal {
    public:
      /**
       *  @brief  Default constructor
       */
      Signal() = default;
      
      /**
       *  @brief  Destructor
       */
      ~Signal();

      /**
       *  @brief  Emit the signal. This function is deprecated
       * 
       *  @param  args arguments to forward
       */      
      DEPRECATED(void process(Args... args));

      /**
       *  @brief  Emit the signal
       * 
       *  @param  args arguments to forward
       */
      void emit(Args... args);

      /**
       *  @brief  Connect a class member-function to signal
       *
       *  @param  object the class object
       *  @param  func the class member-function
       */
      template <typename T>
      bool connect(T *object, void (T::*func)(Args...));
      
      /**
       *  @brief  Connect a function to signal
       *  
       *  @param  func [description]
       */
      bool connect(void (*func)(Args...));

      /**
       *  @brief  Disconnect all functions of the target object 
       *
       *  @param  object the object target
       */
      template <typename T>
      bool disconnect(T *object);
      
      /**
       *  @brief  Disconnect a specific class member-function
       *
       *  @param  object the target object
       *  @param  func the class member-function
       */
      template <typename T>
      bool disconnect(T *object, void (T::*func)(Args...));
      
      /**
       *  @brief  Disconnect a function
       *
       *  @param  object the target object
       *  @param  func the class member-function
       */
      bool disconnect(void (*func)(Args...));

      /**
       *  @brief  Disconnect all functions
       */
      void disconnect();
      
      /**
       *  @brief  Disconnect all functions. Deprecated, use disconnect() instead
       */
      DEPRECATED(void disconnectAll());

      /**
       *  @brief  Whether the target object has any connected functions
       *
       *  @param  object the target object 
       */
      template <typename T>
      bool hasConnection(T *object) const;
      
      /**
       *  @brief  Whether the target object and member-function is connected
       *
       *  @param  object the target object 
       *  @param  func the class member-function
       */
      template <typename T>
      bool isConnected(T *object, void (T::*func)(Args...)) const;
      
      /**
       *  @brief  Whether the function is connected
       *
       *  @param  func the target function
       */
      bool isConnected(void (*func)(Args...)) const;

      /**
       *  @brief  Whether the signal has at least one connection 
       */
      bool hasConnection() const;
      
      /**
       *  @brief  Get the number of connected functions
       */
      size_t nConnections() const;

    private:
      /// The list of callbacks
      typename std::vector<SignalBase<Args...>*>   m_callbacks = {};
    };

    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    template <typename... Args>
    inline Signal<Args...>::~Signal() {
      this->disconnect();
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline void Signal<Args...>::process(Args... args) {
      emit(args...);
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    inline void Signal<Args...>::emit(Args... args) {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        (*iter)->emit(args...);
      }
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    template <typename T>
    inline bool Signal<Args...>::connect(T *obj, void (T::*func)(Args...)) {
      if (this->isConnected(obj, func)) {
        return false;
      }
      m_callbacks.push_back(new SignalT<T, Args...>(obj, func));
      return true;
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline bool Signal<Args...>::connect(void (*func)(Args...)) {
      if (this->isConnected(func)) {
        return false;
      }
      m_callbacks.push_back(new SignalT<void, Args...>(func));
      return true;
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    template <typename T>
    inline bool Signal<Args...>::disconnect(T *obj) {
      bool disconnected = false;
      auto iter = m_callbacks.begin();
      while(iter != m_callbacks.end()) {        
      // for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<SignalT<T, Args...>*>(*iter);
        if(nullptr == callback) {
          iter++;
          continue;
        }
        if (callback->object() == obj) {
          delete callback;
          iter = m_callbacks.erase(iter);
          disconnected = true;
          continue;
        }
        iter++;
      }
      return disconnected;
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    template <typename T>
    inline bool Signal<Args...>::disconnect(T *obj, void (T::*func)(Args...)) {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<SignalT<T, Args...>*>(*iter);
        if(nullptr == callback) {
          continue;
        }
        if (callback->object() == obj && callback->function() == func) {
          delete callback;
          m_callbacks.erase(iter);
          return true;
        }
      }
      return false;
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline bool Signal<Args...>::disconnect(void (*func)(Args...)) {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<SignalT<void, Args...>*>(*iter);
        if(nullptr == callback) {
          continue;
        }
        if (callback->function() == func) {
          delete callback;
          m_callbacks.erase(iter);
          return true;
        }
      }
      return false;
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    inline void Signal<Args...>::disconnect() {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        delete *iter;        
      }
      m_callbacks.clear();
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline void Signal<Args...>::disconnectAll() {
      disconnect();
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    template <typename T>
    inline bool Signal<Args...>::hasConnection(T *obj) const {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<const SignalT<T, Args...>*>(*iter);
        if (nullptr == callback) {
          continue;          
        }
        if (callback->object() == obj) {
          return true;
        }
      }
      return false;
    }
    
    //----------------------------------------------------------------------------------

    template <typename... Args>
    template <typename T>
    inline bool Signal<Args...>::isConnected(T *obj, void (T::*func)(Args...)) const {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<const SignalT<T, Args...>*>(*iter);
        if (nullptr == callback) {
          continue;          
        }
        if (callback->object() == obj && callback->function() == func) {
          return true;
        }
      }
      return false;
    }
    
    //----------------------------------------------------------------------------------

    template <typename... Args>
    inline bool Signal<Args...>::isConnected(void (*func)(Args...)) const {
      for (auto iter = m_callbacks.begin(), endIter = m_callbacks.end(); endIter != iter; ++iter) {
        auto callback = dynamic_cast<const SignalT<void, Args...>*>(*iter);
        if (nullptr == callback) {
          continue;          
        }
        if (callback->function() == func) {
          return true;
        }
      }
      return false;
    }

    //----------------------------------------------------------------------------------

    template <typename... Args>
    inline bool Signal<Args...>::hasConnection() const {
      return (not m_callbacks.empty());
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline size_t Signal<Args...>::nConnections() const {
      return m_callbacks.size();
    }

    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    inline SignalT<T, Args...>::SignalT(T *obj, Function func) : 
      m_object(obj), 
      m_function(func) {
      /* nop */
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    inline void SignalT<T, Args...>::emit(Args... args) {
      (m_object->*m_function)(args...);
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    inline const T *SignalT<T, Args...>::object() const {
      return m_object;
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T, typename... Args>
    inline typename SignalT<T, Args...>::Function SignalT<T, Args...>::function() const {
      return m_function;
    }
    
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    
    template <typename T>
    inline SignalT<T, void>::SignalT(T *obj, Function func) : 
      m_object(obj), 
      m_function(func) {
      /* nop */
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T>
    inline void SignalT<T, void>::emit() {
      (m_object->*m_function)();
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T>
    inline const T *SignalT<T, void>::object() const {
      return m_object;
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename T>
    inline typename SignalT<T, void>::Function SignalT<T, void>::function() const {
      return m_function;
    }
    
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline SignalT<void, Args...>::SignalT(Function func) :
      m_function(func) {
      /* nop */
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline void SignalT<void, Args...>::emit(Args... args) {
      (*m_function)(args...);
    }
    
    //----------------------------------------------------------------------------------
    
    template <typename... Args>
    inline typename SignalT<void, Args...>::Function SignalT<void, Args...>::function() const {
      return m_function;
    }
    
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    
    inline SignalT<void, void>::SignalT(Function func) :
      m_function(func) {
      /* nop */
    }
    
    //----------------------------------------------------------------------------------
    
    inline void SignalT<void, void>::emit() {
      (*m_function)();
    }
    
    //----------------------------------------------------------------------------------
    
    inline typename SignalT<void, void>::Function SignalT<void, void>::function() const {
      return m_function;
    }
  }
}

#endif //  DQM4HEP_SIGNAL_H
