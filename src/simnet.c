#include "simnet.h"
#include "dotgen.h"



int main(void){
    struct Router routers[ROUTER_TOTAL];
    Simnet_init(routers);
    Simnet_createGraph(routers);
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
