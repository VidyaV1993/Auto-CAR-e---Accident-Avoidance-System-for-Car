
#ifdef __cplusplus
extern "C" {
#endif // def __cplusplus
#ifdef MYDEBUG
	#define DEBUGPRINTF(format, args...)  printf(format, ##args)
#else 
	#define DEBUGPRINTF(format, args...)  
#endif 

#ifdef __cplusplus
} // extern "C"
#endif // def __cplusplus