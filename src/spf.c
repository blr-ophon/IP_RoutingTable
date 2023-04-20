#include "spf.h"

void SPF_fillL3Table(struct Router *routers, struct table_entry *dist_table, int id){
    routers[id].Routing_table = RNode_create();
    RouteNode *rtable = routers[id].Routing_table;
    for(int i = 0; i < ROUTER_TOTAL; i++){

        //find which neighbor of id is the gateway, aka the first node in the shortest path
        int tmp_i = i;
        while(dist_table[tmp_i].pv_node != id){
            tmp_i = dist_table[tmp_i].pv_node;
        }

        uint32_t gateway = routers[tmp_i].ipv4;
        uint32_t gw_mask_len = routers[dist_table[i].pv_node].mask_len;
        RNode_insert(&rtable, routers[i].ipv4, routers[i].mask_len, gateway, gw_mask_len);
    }
    RNode_print(rtable);
}

//shortest routes of all nodes to id. Using Dijkstra's algorithm
void Router_SPF(struct Router *routers, int start_id){
    //distance table
    struct table_entry dist_table[ROUTER_TOTAL];
    //set all entries
    for(int i = 0; i < ROUTER_TOTAL; i++){
        dist_table[i].dist_to_id = 9999;        //arbitrary high number
        dist_table[i].pv_node = -1;        //arbitrary high number
        dist_table[i].visited = false;        //arbitrary high number
    }


    //distance to itself is 0
    dist_table[start_id].dist_to_id = 0;
    dist_table[start_id].pv_node = start_id;

    for(;;){

        //use dist_table to find closest unvisited node to start
        int closest_node = start_id;
        int closest_dist = 9999;
        for(int i = 0; i < ROUTER_TOTAL; i++){
            if(!dist_table[i].visited && dist_table[i].dist_to_id < closest_dist){
                closest_dist = dist_table[i].dist_to_id;
                closest_node = i;
            }
        }

        //end loop if there are no unvisited nodes anymore
        if(closest_dist == 9999){
            break;
        }
        
        //visit closest node
        dist_table[closest_node].visited = true;
        RouteLink *p = routers[closest_node].neighbors;
        for(;p != NULL; p = p->next){

            //ignore node from which it came
            if(p->node_id == dist_table[closest_node].pv_node){
                continue;
            }

            //distance from start to this neighbor via closest_node
            int neighbor_dist = closest_dist + p->weight;       
            //if this distance is less then what was previously registered
            if(neighbor_dist < dist_table[p->node_id].dist_to_id){
                dist_table[p->node_id].dist_to_id = neighbor_dist;
                dist_table[p->node_id].pv_node = closest_node;
            }
        }
    }
    //printf("\nDIJKSTRA'S ALGORITHM TABLE\n");
    //for(int i = 0; i < ROUTER_TOTAL; i++){
    //    printf("%d | %d | %d\n",
    //            i,
    //            dist_table[i].dist_to_id,
    //            dist_table[i].pv_node
    //          );
    //}

    //Fill routing table
    SPF_fillL3Table(routers, dist_table, start_id);
}

