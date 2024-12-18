#ifndef __BASE_EXTENSIBLE_HH__
#define __BASE_EXTENSIBLE_HH__

#include <cassert>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>

namespace Kuiper {
    namespace Cache {
        /**
         * This is base of every extension.
         */
        class ExtensionBase {
        public:
            explicit ExtensionBase(const unsigned int id)
                : extID(id) {}

            virtual ~ExtensionBase() = default;

            virtual std::unique_ptr<ExtensionBase> clone() const = 0;

            static unsigned int
                maxNumExtensions()
            {
                static unsigned int max_num = 0;
                return ++max_num;
            }

            unsigned int getExtensionID() const { return extID; }

        private:
            const unsigned int extID;
        };

        /**
         * This is the extension for carrying additional information.
         * Each type of extension will have a unique extensionID.
         * This extensionID will assign to base class for comparsion.
         *
         * Example usage:
         *
         *   class MyTarget : Extensible<MyTarget> {};
         *
         *   class MyExtension : public Extension<MyTarget, MyExtension>
         *   {
         *     public:
         *       MyExtension();
         *       std::unique_ptr<ExtensionBase> clone() const override;
         *       uint32_t getData();
         *
         *     private:
         *       uint32_t data_;;
         *   };
         *
         *   std::unique_ptr<MyTarget> mytarget(new MyTarget);
         *   std::shared_ptr<MyExtension> myext(new MyExtension);
         *   mytarget->setExtension(myext);
         *
         *   std::shared_ptr<MyExtension> ext = mytarget->getExtension<MyExtension>();
         *   uint32_t data = ext->getData();
         *   mytarget->removeExtension<MyExtension>();
         *
         *   In the example above, MyTarget can carry an extension named MyExtension,
         *   which contains an additional data field. This could be applicated to any
         *   debug information or any data field in any protocol.
         */
        template <typename Target, typename T>
        class Extension : public ExtensionBase
        {
        public:
            Extension() : ExtensionBase(extensionID) {}

            const static unsigned int extensionID;
        };

        template <typename Target, typename T>
        const unsigned int Extension<Target, T>::extensionID =
            ExtensionBase::maxNumExtensions() - 1;

        template <typename Target>
        class Extensible {
        public:
            Extensible() = default;
            Extensible(const Extensible& other) {
                // Clone every extension from other.
                for (auto& ext : other.extensions) {
                    extensions.emplace_back(ext->clone());
                }
            }
            virtual ~Extensible() = default;

            /**
             * Set a new extension to the packet and replace the old one, if there
             * already exists the same type of extension in this packet. This new
             * extension will be deleted automatically with the shared_ptr<>.
             *
             * @param ext Extension to set
             */
            template <typename T>
            void
                setExtension(std::shared_ptr<T> ext)
            {
                static_assert(std::is_base_of<ExtensionBase, T>::value,
                    "Extension should inherit from ExtensionBase.");
                assert(ext.get() != nullptr);

                auto it = findExtension<T>();

                if (it != extensions.end()) {
                    // There exists the same type of extension in the list.
                    // Replace it to the new one.
                    *it = std::move(ext);
                }
                else {
                    // Add ext into the linked list.
                    extensions.emplace_back(std::move(ext));
                }
            }

            /**
             * Remove the extension based on its type.
             *
             * @param ext Extension to remove
             */
            template <typename T>
            void
                removeExtension(void)
            {
                static_assert(std::is_base_of<ExtensionBase, T>::value,
                    "Extension should inherit from ExtensionBase.");

                auto it = findExtension<T>();
                if (it != extensions.end())
                    extensions.erase(it);
            }

            /**
             * Get the extension pointer by linear search with the extensionID.
             */
            template <typename T>
            std::shared_ptr<T>
                getExtension()
            {
                static_assert(std::is_base_of<ExtensionBase, T>::value,
                    "Extension should inherit from ExtensionBase.");
                auto it = findExtension<T>();
                if (it == extensions.end())
                    return nullptr;
                return std::static_pointer_cast<T>(*it);
            }

        protected:

            /**
             * Go through the extension list and return the iterator to the instance of
             * the type of extension. If there is no such an extension, return the end
             * iterator of the list.
             *
             *  @return The iterator to the extension type T if there exists.
             */
            template <typename T>
            std::list<std::shared_ptr<ExtensionBase>>::iterator
                findExtension()
            {
                auto it = extensions.begin();
                while (it != extensions.end()) {
                    if ((*it)->getExtensionID() == T::extensionID)
                        break;
                    it++;
                }
                return it;
            }

            // Linked list of extensions.
            std::list<std::shared_ptr<ExtensionBase>> extensions;
        };
    } // namespace Cache
} // namespace Kuiper

#endif //__BASE_EXTENSIBLE_HH__
