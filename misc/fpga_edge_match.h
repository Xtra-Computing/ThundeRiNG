#ifndef __FPGA_EDGE_MATCH_H__
#define __FPGA_EDGE_MATCH_H__


#define IS_ADJACENT(e1, e2)             (                  \
                                            (e1.s == e2.s) \
                                         || (e1.s == e2.d) \
                                         || (e1.d == e2.s) \
                                         || (e1.d == e2.d) \
                                        )


#define IS_CLOSE(e1, e2)               (                                 \
                                        ((e1.s == e2.s)&&(e1.d == e2.d)) \
                                         ||                              \
                                        ((e1.d == e2.s)&&(e1.s == e2.d)) \
                                        )


/*
if (edge.s == first_edge.s)
{
    match_edge.s = edge.d;
    match_edge.d = first_edge.d;
} else if (edge.s == first_edge.d)
{
    match_edge.s = edge.d;
    match_edge.d = first_edge.s;
} else if (edge.d == first_edge.s)
{
    match_edge.s = edge.s;
    match_edge.d = first_edge.d;
} else
{
    match_edge.s = edge.s;
    match_edge.d = first_edge.s;
}
*/
inline uint32_t get_ms(edge_t &e1, edge_t &e2)
{
    if ((e1.s == e2.s) || (e1.s == e2.d ))
    {
        return e1.d;
    }
    else
    {
        return e1.s;
    }
}

inline uint32_t get_md(edge_t &e1, edge_t &e2)
{
    if ((e2.s == e1.s) || (e2.s == e1.d ))
    {
        return e2.d;
    }
    else
    {
        return e2.s;
    }
}



#endif /* __FPGA_EDGE_MATCH_H__ */
