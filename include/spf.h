#ifndef SPF_H
#define SPF_H

#include "simnet.h"
#include "l3table.h"

struct table_entry{
    int dist_to_id;
    int pv_node;
    bool visited;
};

void Router_SPF(struct Router *routers, int start_id);

//fills l3 table of node id
void SPF_fillL3Table(struct Router *routers, struct table_entry *dist_table, int id);

#endif
