#ifndef __SIM_PROBE_PROBE_HH__
#define __SIM_PROBE_PROBE_HH__

#include <functional>
#include <string>
#include <vector>

namespace Kuiper {
  namespace Cache {
    /** Forward declare the ProbeManager. */
    class ProbeManager;
    class ProbeListener;
    struct ProbeListenerObjectParams;

    /**
     * Name space containing shared probe point declarations.
     *
     * Probe types that are shared between multiple types of SimObjects
     * should live in this name space. This makes it possible to use a
     * common instrumentation interface for devices such as PMUs that have
     * different implementations in different ISAs.
     */
    namespace probing
    {
      /* Note: This is only here for documentation purposes, new probe
       * points should normally be declared in their own header files. See
       * for example pmu.hh.
       */
    }

    /**
     * This class is a minimal wrapper around SimObject. It is used to declare
     * a python derived object that can be added as a ProbeListener to any other
     * SimObject.
     *
     * It instantiates manager from a call to Parent.any.
     * The vector of listeners is used simply to hold onto listeners until the
     * ProbeListenerObject is destroyed.
     */
    class ProbeListenerObject {
    protected:
      ProbeManager *manager;
      std::vector<ProbeListener *> listeners;

    public:
      ProbeListenerObject(const ProbeListenerObjectParams &params);
      virtual ~ProbeListenerObject();
      ProbeManager *getProbeManager() { return manager; }
    };

    /**
     * ProbeListener base class; here to simplify things like containers
     * containing multiple types of ProbeListener.
     *
     * Note a ProbeListener is added to the ProbePoint in constructor by
     * using the ProbeManager passed in.
     */
    class ProbeListener
    {
    public:
      ProbeListener(ProbeManager *manager, const std::string &name);
      virtual ~ProbeListener();
      ProbeListener(const ProbeListener &other) = delete;
      ProbeListener &operator=(const ProbeListener &other) = delete;
      ProbeListener(ProbeListener &&other) noexcept = delete;
      ProbeListener &operator=(ProbeListener &&other) noexcept = delete;

    protected:
      ProbeManager *const manager;
      const std::string name;
    };

    /**
     * ProbeListener base class; again used to simplify use of ProbePoints
     * in containers and used as to define interface for adding removing
     * listeners to the ProbePoint.
     */
    class ProbePoint
    {
    protected:
      const std::string name;

    public:
      ProbePoint(ProbeManager *manager, const std::string &name);
      virtual ~ProbePoint() {}

      virtual void addListener(ProbeListener *listener) = 0;
      virtual void removeListener(ProbeListener *listener) = 0;
      std::string getName() const { return name; }
    };

    /**
     * ProbeManager is a conduit class that lives on each SimObject,
     *  and is used to match up probe listeners with probe points.
     */
    class ProbeManager
    {
    private:
      /** Required for sensible debug messages.*/
      // GEM5_CLASS_VAR_USED const SimObject *object;
      /** Vector for name look-up. */
      std::vector<ProbePoint *> points;

    public:
      // ProbeManager(SimObject *obj)
      //     : object(obj)
      // {
      // }
      ProbeManager() {};
      virtual ~ProbeManager() {}

      /**
       * @brief Add a ProbeListener to the ProbePoint named by pointName.
       *        If the name doesn't resolve a ProbePoint return false.
       * @param point_name name of the ProbePoint to add the ProbeListener to.
       * @param listener the ProbeListener to add.
       * @return true if added, false otherwise.
       */
      bool addListener(std::string point_name, ProbeListener &listener);

      /**
       * @brief Remove a ProbeListener from the ProbePoint named by pointName.
       *        If the name doesn't resolve a ProbePoint return false.
       * @param point_name the name of the ProbePoint to remove the ProbeListener
       *        from.
       * @param listener the ProbeListener to remove.
       * @return true if removed, false otherwise.
       */
      bool removeListener(std::string point_name, ProbeListener &listener);

      /**
       * @brief Add a ProbePoint to this SimObject ProbeManager.
       * @param point the ProbePoint to add.
       */
      void addPoint(ProbePoint &point);
    };

    /**
     * ProbeListenerArgBase is used to define the base interface to a
     * ProbeListenerArg (i.e the notify method on specific type).
     *
     * It is necessary to split this out from ProbeListenerArg, as that
     * templates off the class containing the function that notify calls.
     */
    template <class Arg>
    class ProbeListenerArgBase : public ProbeListener
    {
    public:
      ProbeListenerArgBase(ProbeManager *pm, const std::string &name)
          : ProbeListener(pm, name)
      {
      }
      virtual void notify(const Arg &val) = 0;
    };

    /**
     * ProbeListenerArg generates a listener for the class of Arg and the
     * class type T which is the class containing the function that notify will
     * call.
     *
     * Note that the function is passed as a pointer on construction.
     */
    template <class T, class Arg>
    class ProbeListenerArg : public ProbeListenerArgBase<Arg>
    {
    private:
      T *object;
      void (T::*function)(const Arg &);

    public:
      /**
       * @param obj the class of type Tcontaining the method to call on notify.
       * @param name the name of the ProbePoint to add this listener to.
       * @param func a pointer to the function on obj (called on notify).
       */
      ProbeListenerArg(T *obj, const std::string &name,
                       void (T::*func)(const Arg &))
          : ProbeListenerArgBase<Arg>(obj->getProbeManager(), name),
            object(obj),
            function(func)
      {
      }

      /**
       * @brief called when the ProbePoint calls notify. This is a shim through
       *        to the function passed during construction.
       * @param val the argument value to pass.
       */
      void notify(const Arg &val) override { (object->*function)(val); }
    };

    /**
     * ProbePointArg generates a point for the class of Arg. As ProbePointArgs talk
     * directly to ProbeListenerArgs of the same type, we can store the vector of
     * ProbeListeners as their Arg type (and not as base type).
     *
     * Methods are provided to addListener, removeListener and notify.
     */
    template <typename Arg>
    class ProbePointArg : public ProbePoint
    {
      /** The attached listeners. */
      std::vector<ProbeListenerArgBase<Arg> *> listeners;

    public:
      ProbePointArg(ProbeManager *manager, std::string name)
          : ProbePoint(manager, name)
      {
      }

      /**
       * Informs whether any listeners are attached to this probe. This can
       * be used to avoid performing costly tasks needed by the probe when
       * nobody is listening.
       *
       * @return Whether this probe has any listener.
       */
      bool hasListeners() const { return listeners.size() > 0; }

      /**
       * @brief adds a ProbeListener to this ProbePoints notify list.
       * @param l the ProbeListener to add to the notify list.
       */
      void
      addListener(ProbeListener *l) override
      {
        // check listener not already added
        if (std::find(listeners.begin(), listeners.end(), l) ==
            listeners.end())
        {
          listeners.push_back(static_cast<ProbeListenerArgBase<Arg> *>(l));
        }
      }

      /**
       * @brief remove a ProbeListener from this ProbePoints notify list.
       * @param l the ProbeListener to remove from the notify list.
       */
      void
      removeListener(ProbeListener *l) override
      {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), l),
                        listeners.end());
      }

      /**
       * @brief called at the ProbePoint call site, passes arg to each listener.
       * @param arg the argument to pass to each listener.
       */
      void
      notify(const Arg &arg)
      {
        for (auto l = listeners.begin(); l != listeners.end(); ++l)
        {
          (*l)->notify(arg);
        }
      }
    };

    /**
     * ProbeListenerArgFunc generates a listener for the class of Arg and
     * a lambda callback function that is called by the notify.
     *
     * Note that the function is passed as lambda function on construction
     * Example:
     * ProbeListenerArgFunc<MyArg> (myobj->getProbeManager(),
     *                "MyProbePointName", [this](const MyArg &arg)
     *                { my_own_func(arg, xyz...); // do something with arg
     *  }));
     */
    template <class Arg>
    class ProbeListenerArgFunc : public ProbeListenerArgBase<Arg>
    {
      typedef std::function<void(const Arg &)> NotifyFunction;

    private:
      NotifyFunction function;

    public:
      /**
       * @param obj the class of type Tcontaining the method to call on notify.
       * @param pm A probe manager that is not part of the obj
       * @param name the name of the ProbePoint to add this listener to.
       * @param func a pointer to the function on obj (called on notify).
       */
      ProbeListenerArgFunc(ProbeManager *pm, const std::string &name,
                           const NotifyFunction &func)
          : ProbeListenerArgBase<Arg>(pm, name),
            function(func)
      {
      }

      /**
       * @brief called when the ProbePoint calls notify. This is a shim through
       *        to the function passed during construction.
       * @param val the argument value to pass.
       */
      void notify(const Arg &val) override { function(val); }
    };
  } // namespace Cache
} // namespace Kuiper

#endif//__SIM_PROBE_PROBE_HH__
