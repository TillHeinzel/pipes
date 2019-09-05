#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
#define DISABLE_WARNING_POP            __pragma(warning( pop )) 
#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

#define DISABLE_WARNING_UNUSED_VALUE 
#define DISABLE_WARNING_MISUSED_COMMA 

#elif defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop) 
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNUSED_VALUE   DISABLE_WARNING(-Wunused-value)
#define DISABLE_WARNING_MISUSED_COMMA  DISABLE_WARNING(-Wcomma)

#elif defined(__GNUC__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop) 
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNUSED_VALUE   DISABLE_WARNING(-Wunused-value)
#define DISABLE_WARNING_MISUSED_COMMA  

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP

#define DISABLE_WARNING_UNUSED_VALUE  
#define DISABLE_WARNING_MISUSED_COMMA 

#endif
