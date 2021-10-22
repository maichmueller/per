
#ifndef PER_MACRO_HPP
#define PER_MACRO_HPP

//#ifndef PER_EXPORT
//   #if defined(MSVC_VER)
//      #define PER_EXPORT __declspec(dllexport)
//   #elif defined(__GNUG__)
//      #define PER_EXPORT __attribute__((visibility("default")))
//   #elif defined(__clang__)
//      #define PER_EXPORT __attribute__((visibility("default")))
//   #else
//      #define PER_EXPORT
//   #endif
//#endif  // PER_EXPORT

#ifndef BUILD_DLL
   #define BUILD_DLL 0  // by default, we assume user code
#endif

#if BUILD_DLL
   #ifndef PER_EXPORT
      #define PER_EXPORT __declspec(dllexport)
   #endif
#else
   #ifndef PER_EXPORT
      #define PER_EXPORT __declspec(dllimport)
   #endif
#endif

#endif  // PER_MACRO_HPP
