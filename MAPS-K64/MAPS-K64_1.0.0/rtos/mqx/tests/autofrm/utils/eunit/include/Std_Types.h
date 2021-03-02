#ifndef STD_TYPES_H
#define STD_TYPES_H

/* 'bool' type is defined already in psptypes.h
typedef unsigned long bool;
*/

typedef signed char sint8;              
typedef unsigned char uint8;            
typedef signed short sint16;            
typedef unsigned short uint16;          
typedef signed long sint32;             
typedef unsigned long uint32;           
typedef unsigned long uint8_least;      
typedef unsigned long uint16_least;     
typedef unsigned long uint32_least;     
typedef signed long sint8_least;        
typedef signed long sint16_least;       
typedef signed long sint32_least;       
typedef float float32;                  
typedef double float64;                 

#define NULL_PTR ((void *)0)

#ifndef TRUE
    #define TRUE ((bool) 1)
#endif

#ifndef FALSE
    #define FALSE ((bool) 0)
#endif

#ifndef STD_HIGH
#define STD_HIGH    0x01
#endif

#ifndef STD_LOW
#define STD_LOW     0x00
#endif

#endif /* STD_TYPES_H */
