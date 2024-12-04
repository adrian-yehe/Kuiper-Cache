#include "include/probe.h"
// #include "params/ProbeListenerObject.hh"

namespace Kuiper {
    namespace Cache {
        ProbePoint::ProbePoint(ProbeManager *manager, const std::string &_name)
            : name(_name)
        {
            if (manager)
            {
                manager->addPoint(*this);
            }
        }

        // ProbeListenerObject::ProbeListenerObject(
        //     const ProbeListenerObjectParams &params)
        //     : manager(params.manager->getProbeManager())
        // {
        // }

        ProbeListenerObject::ProbeListenerObject(
            const ProbeListenerObjectParams &params)
        {
        }

        ProbeListenerObject::~ProbeListenerObject()
        {
            for (auto l = listeners.begin(); l != listeners.end(); ++l)
            {
                delete (*l);
            }
            listeners.clear();
        }

        ProbeListener::ProbeListener(ProbeManager *_manager, const std::string &_name)
            : manager(_manager), name(_name)
        {
            manager->addListener(name, *this);
        }

        ProbeListener::~ProbeListener()
        {
            manager->removeListener(name, *this);
        }

        bool
        ProbeManager::addListener(std::string point_name, ProbeListener &listener)
        {
            // DPRINTFR(ProbeVerbose, "Probes: Call to addListener to \"%s\" on %s.\n",
            //          point_name, object->name());
            bool added = false;
            for (auto p = points.begin(); p != points.end(); ++p)
            {
                if ((*p)->getName() == point_name)
                {
                    (*p)->addListener(&listener);
                    added = true;
                }
            }
            if (!added)
            {
                // DPRINTFR(ProbeVerbose, "Probes: Call to addListener to \"%s\" on "
                //                        "%s failed, no such point.\n",
                //          point_name, object->name());
            }
            return added;
        }

        bool
        ProbeManager::removeListener(std::string point_name, ProbeListener &listener)
        {
            // DPRINTFR(ProbeVerbose, "Probes: Call to removeListener from \"%s\" on "
            //                        "%s.\n",
            //          point_name, object->name());
            bool removed = false;
            for (auto p = points.begin(); p != points.end(); ++p)
            {
                if ((*p)->getName() == point_name)
                {
                    (*p)->removeListener(&listener);
                    removed = true;
                }
            }
            if (!removed)
            {
                // DPRINTFR(ProbeVerbose, "Probes: Call to removeListener from \"%s\" "
                //                        "on %s failed, no such point.\n",
                //          point_name, object->name());
            }
            return removed;
        }

        void
        ProbeManager::addPoint(ProbePoint &point)
        {
            // DPRINTFR(ProbeVerbose, "Probes: Call to addPoint \"%s\" to %s.\n",
            //          point.getName(), object->name());

            for (auto p = points.begin(); p != points.end(); ++p)
            {
                if ((*p)->getName() == point.getName())
                {
                    // DPRINTFR(ProbeVerbose, "Probes: Call to addPoint \"%s\" to %s "
                    //                        "failed, already added.\n",
                    //          point.getName(), object->name());
                    return;
                }
            }
            points.push_back(&point);
        }
    } // namespace Cache
} // namespace Kuiper
