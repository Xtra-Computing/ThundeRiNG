#ifndef __ASAP_TYPE_H__
#define __ASAP_TYPE_H__



#ifndef FLAG_SET
#define FLAG_SET                (1u)
#endif

#ifndef FLAG_RESET
#define FLAG_RESET              (0u)
#endif


typedef ap_uint < 32 > fe_select_t;
typedef unsigned int  uint32_t;

typedef struct
{
    uint32_t s;
    uint32_t d;
} edge_t;


#endif  /* __ASAP_TYPE_H__ */
