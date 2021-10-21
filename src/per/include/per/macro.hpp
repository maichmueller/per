
#ifndef PER_MACRO_HPP
#define PER_MACRO_HPP

#ifndef PER_PUBLIC
#if defined(MSVC_VER)
#define PER_PUBLIC __declspec(dllexport)
#elif defined(__GNUG__)
#define PER_PUBLIC __attribute__((visibility("default")))
#elif defined(__clang__)
#define PER_PUBLIC __attribute__((visibility("default")))
#else
#define PER_PUBLIC
#endif
#endif //PER_PUBLIC

#endif //PER_MACRO_HPP
