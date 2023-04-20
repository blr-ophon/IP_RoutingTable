#include "simnet.h"

void simnet_init(void){
   RouteLink routers[8]; 
}

int main(void){
	RouteLink *rlink = NULL;
    for(int i = 0; i < 10; i++){
        Router_insertNeighbor(&rlink, i);
    }
    return 0;
}

void Router_insertNeighbor(RouteLink **neighborList, int id){
    //create new node
	RouteLink* newNode = malloc(sizeof(RouteLink));
    newNode->node_id = id;
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
    Router_printNeighbors(*neighborList);
}

void Router_printNeighbors(RouteLink *sPtr){
	RouteLink *tmp = sPtr;
	if((sPtr) == NULL){
		printf("List is empty\n");
        return;
	}else{
		printf("\n");
		while(tmp != NULL){
			printf("%d -> ", tmp->node_id);
			tmp = tmp->next;
		}	
		printf("NULL\n\n");
	}	
}
