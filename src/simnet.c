#include "simnet.h"
#include "dotgen.h"

//shortest routes of all nodes to id. Using Dijkstra's algorithm
void Router_SPF(struct Router *routers, int start_id){
    struct table_entry{
        int dist_to_id;
        int pv_node;
        bool visited;
    };
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
    printf("\nDIJKSTRA'S ALGORITHM TABLE\n");
    for(int i = 0; i < ROUTER_TOTAL; i++){
        printf("%d | %d | %d\n",
                i,
                dist_table[i].dist_to_id,
                dist_table[i].pv_node
              );
    }
}


int main(void){
    struct Router routers[ROUTER_TOTAL];
    Simnet_init(routers);
    Simnet_createGraph(routers);
    Router_SPF(routers, 3);
    return 0;
}

//create a sample graph
void Simnet_createGraph(struct Router *routers){
    Router_insertDEdge(routers, 0, 1, 2);
    Router_insertDEdge(routers, 0, 4, 4);
    Router_insertDEdge(routers, 1, 2, 5);
    Router_insertDEdge(routers, 2, 3, 2);
    Router_insertDEdge(routers, 3, 4, 1);
    Router_insertDEdge(routers, 4, 1, 3);
    for(int i = 0; i < ROUTER_TOTAL; i++){
        Router_printNeighbors(routers[i].neighbors);
    }
    Simnet_dotgen(routers);
}

void Simnet_init(struct Router *routers){
    for(int i = 0; i < ROUTER_TOTAL; i++){
        memset(&routers[i], 0, sizeof(struct Router));
    }
}

//Insert double edge
void Router_insertDEdge(struct Router *routers, int id1, int id2, int weight){
    Router_insertNeighbor(&routers[id1].neighbors, id2, weight);
    Router_insertNeighbor(&routers[id2].neighbors, id1, weight);
}

void Router_insertNeighbor(RouteLink **neighborList, int id, int weight){
    //create new node
	RouteLink* newNode = malloc(sizeof(RouteLink));
    newNode->node_id = id;
    newNode->weight = weight;
    newNode->next = NULL;

    //traverse list and insert node
    //inserts in increasing order
	RouteLink* previousNode = NULL;
	RouteLink* tmp = *neighborList;	
    for(;tmp != NULL && tmp->node_id < id; tmp = tmp->next){
        previousNode = tmp;
    }
    
    if(previousNode == NULL){// tmp is list root
        newNode->next = *neighborList;
        *neighborList = newNode;
    }else{
        newNode->next = tmp;
        previousNode->next = newNode;
    }
    //Router_printNeighbors(*neighborList);
}

void Router_printNeighbors(RouteLink *sPtr){
	RouteLink *tmp = sPtr;
	if((sPtr) == NULL){
		printf("List is empty\n");
        return;
	}else{
		while(tmp != NULL){
			printf("%d(w%lu) -> ", tmp->node_id, tmp->weight);
			tmp = tmp->next;
		}	
		printf("NULL\n");
	}	
}
