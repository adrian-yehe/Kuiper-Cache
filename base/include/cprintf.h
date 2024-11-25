#ifndef __BASE_CPRINTF_HH__
#define __BASE_CPRINTF_HH__

#include <ios>
#include <iostream>
#include <list>
#include <string>

#include "cprintf_formats.h"

namespace Kuiper {
    namespace Cache {
        namespace cp {
            struct Print {
            protected:
                std::ostream& stream;
                const char* format;
                const char* ptr;
                bool cont;

                std::ios::fmtflags savedFlags;
                char savedFill;
                int savedPrecision;
                int savedWidth;

                Format fmt;
                void process();
                void processFlag();

            public:
                Print(std::ostream& stream, const std::string& format);
                Print(std::ostream& stream, const char* format);
                ~Print();

                int
                    getNumber(int data)
                {
                    return data;
                }

                template <typename T>
                int
                    getNumber(const T& data)
                {
                    return 0;
                }

                template <typename T>
                void
                    addArg(const T& data)
                {
                    if (!cont)
                        process();

                    if (fmt.getWidth) {
                        fmt.getWidth = false;
                        cont = true;
                        fmt.width = getNumber(data);
                        return;
                    }

                    if (fmt.getPrecision) {
                        fmt.getPrecision = false;
                        cont = true;
                        fmt.precision = getNumber(data);
                        return;
                    }

                    switch (fmt.format) {
                    case Format::Character:
                        formatChar(stream, data, fmt);
                        break;

                    case Format::Integer:
                        formatInteger(stream, data, fmt);
                        break;

                    case Format::Floating:
                        formatFloat(stream, data, fmt);
                        break;

                    case Format::String:
                        formatString(stream, data, fmt);
                        break;

                    default:
                        stream << "<bad format>";
                        break;
                    }
                }

                void endArgs();
            };

        } // namespace cp

        inline void
            ccprintf(cp::Print& print)
        {
            print.endArgs();
        }


        template<typename T, typename ...Args> void
            ccprintf(cp::Print& print, const T& value, const Args &...args)
        {
            print.addArg(value);

            ccprintf(print, args...);
        }


        template<typename ...Args> void
            ccprintf(std::ostream& stream, const char* format, const Args &...args)
        {
            cp::Print print(stream, format);

            ccprintf(print, args...);
        }

        template<typename ...Args> void
            cprintf(const char* format, const Args &...args)
        {
            ccprintf(std::cout, format, args...);
        }

        template<typename ...Args> std::string
            csprintf(const char* format, const Args &...args)
        {
            std::stringstream stream;
            ccprintf(stream, format, args...);
            return stream.str();
        }

        /*
         * functions again with std::string.  We have both so we don't waste
         * time converting const char * to std::string since we don't take
         * advantage of it.
         */
        template<typename ...Args> void
            ccprintf(std::ostream& stream, const std::string& format, const Args &...args)
        {
            ccprintf(stream, format.c_str(), args...);
        }

        template<typename ...Args> void
            cprintf(const std::string& format, const Args &...args)
        {
            ccprintf(std::cout, format.c_str(), args...);
        }

        template<typename ...Args> std::string
            csprintf(const std::string& format, const Args &...args)
        {
            return csprintf(format.c_str(), args...);
        }
    } // namespace Cache
} // namespace Kuiper

#endif // __CPRINTF_HH__
