#ifndef OBELIXLIBDEF_H
#define OBELIXLIBDEF_H

//! DLL export/import macro. Meaningless on Linux.
#ifndef _MSC_VER
    #define OBELIXLIBSHARED_EXPORT
#else
    #ifdef OBELIXLIB_LIBRARY
        #define OBELIXLIBSHARED_EXPORT Q_DECL_EXPORT
    #else // OBELIXLIB_LIBRARY
        #define OBELIXLIBSHARED_EXPORT Q_DECL_IMPORT
        #ifdef _MSC_VER
            // Auto-link (MSVC only).
            #ifdef QT_DEBUG
                #pragma comment(lib, "ObelixLib_d")
            #else
                #pragma comment(lib, "ObelixLib")
            #endif
        #endif
    #endif // OBELIXLIB_LIBRARY
#endif

#endif // OBELIXLIBDEF_H
