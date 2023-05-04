#include "simnet.h"
#include "dotgen.h"
#include "spf.h"

#define BUF_SIZE 32

int main(void){
    //generate topology and fill routing tables
    struct Router routers[ROUTER_TOTAL];
    Simnet_init(routers);
    Simnet_createGraph(routers);
    Simnet_fillTables(routers);

    //prompt for retrieval
    int router_id;
    struct in_addr iaddr;

    char buf[BUF_SIZE];
    printf("\nEnter Router ID:\n");
    fgets(buf, BUF_SIZE, stdin);
    router_id = atoi(buf);

    printf("Enter IPv4 Address: (ddd.ddd.ddd.ddd)\n");
    fgets(buf, BUF_SIZE, stdin);
    buf[strcspn(buf, "\n")] = 0;    //remove \n
    inet_pton(AF_INET, buf, &iaddr); //convert buf string to in_addr

    //retrieve address and store retrieved info in addr_in
    struct RN_addr_in addr_in;
    RNode_retrieve(routers[router_id].Routing_table, ntohl(iaddr.s_addr), 32, &addr_in);

    //print retrieved address
    iaddr.s_addr = htonl(addr_in.addr);
    inet_ntop(AF_INET, &iaddr, buf, INET_ADDRSTRLEN);
    printf("\nRetrieved: %s/%d\n", buf, addr_in.mask_len);

    //print retrieved gateway 
    iaddr.s_addr = htonl(addr_in.gateway);
    inet_ntop(AF_INET, &iaddr, buf, INET_ADDRSTRLEN);
    printf("Gateway: %s/%d\n", buf, addr_in.gw_mask_len);

    return 0;
}

//create a sample graph
void Simnet_createGraph(struct Router *routers){
    //create nodes
    //10.1.1.0 = 0x0a010100
    routers[0].ipv4 = 0x0a000100;
    routers[0].mask_len = 24;
    routers[1].ipv4 = 0x0a010100;
    routers[1].mask_len = 32;
    routers[2].ipv4 = 0x0a020100;
    routers[2].mask_len = 24;
    routers[3].ipv4 = 0x0a030100;
    routers[3].mask_len = 32;
    routers[4].ipv4 = 0x0a040100;
    routers[4].mask_len = 24;
    
    Router_insertDEdge(routers, 0, 1, 2);
    Router_insertDEdge(routers, 0, 4, 4);
    Router_insertDEdge(routers, 1, 2, 5);
    Router_insertDEdge(routers, 2, 3, 2);
    Router_insertDEdge(routers, 3, 4, 1);
    Router_insertDEdge(routers, 4, 1, 3);
    //printf("Adjacency lists:\n");
    //for(int i = 0; i < ROUTER_TOTAL; i++){
    //    Router_printNeighbors(routers[i].neighbors);
    //}
    Simnet_dotgen(routers);
}

void Simnet_init(struct Router *routers){
    for(int i = 0; i < ROUTER_TOTAL; i++){
        memset(&routers[i], 0, sizeof(struct Router));
    }
}

void Simnet_fillTables(struct Router *routers){
    //fill l3 routing table of all routers
    for(int i = 0; i < ROUTER_TOTAL; i++){
        printf("\nRouter %d IPv4 Routing Table:\n", i);
        Router_SPF(routers, i);
    }
}

//Insert edge in both ways
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
