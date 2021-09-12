#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct Node_{
    int dest;
    int weight;
    struct Node_* next;
}Node;

typedef struct NodeList_{
    Node* head;
}NodeList;

typedef struct Graph_{
    NodeList* srcArr;
}Graph;

typedef struct MinHeapNode_{
    int node;
    int dist;
}MinHeapNode;

typedef struct MinHeap_{
    int numNode;
    int *pos;   //save the position of node in arr of heap
    MinHeapNode **arr;
}MinHeap;

void createEdge(Graph* graph, int src, int dest, int weight){
    //add newNode to the front of NodeList
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = graph->srcArr[src].head;
    graph->srcArr[src].head = newNode;
}

MinHeapNode* newMinHeapNode(int v,int dist){
	MinHeapNode* minHeapNode =(MinHeapNode*)malloc(sizeof(MinHeapNode));
	minHeapNode->node = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}

MinHeap* createMinHeap(int N)
{
	MinHeap* minHeap =(MinHeap*)malloc(sizeof(MinHeap));
	minHeap->pos = (int *)malloc(N*sizeof(int));
	minHeap->numNode = 0;
	minHeap->arr =(MinHeapNode**)malloc(N*sizeof(MinHeapNode*));
	return minHeap;
}

void swapNode(MinHeapNode** a,MinHeapNode** b){
	MinHeapNode* temp = *a;
	*a = *b;
	*b = temp;
}

void heapify(MinHeap* minHeap,int idx){
	int parent, left, right;
    parent = idx;
    left = 2*idx + 1;
    right = 2*idx + 2;

    if (left < minHeap->numNode
        && minHeap->arr[left]->dist < minHeap->arr[parent]->dist){
        parent = left;
    }
    if (right < minHeap->numNode
        && minHeap->arr[right]->dist < minHeap->arr[parent]->dist){
        parent = right;
    }

    if(parent != idx){
        minHeap->pos[minHeap->arr[parent]->node] = idx;
        minHeap->pos[minHeap->arr[idx]->node] = parent;
        swapNode(&minHeap->arr[parent], &minHeap->arr[idx]);
        heapify(minHeap, parent);
    }
}

MinHeapNode* extractMin(MinHeap* minHeap){
	if(minHeap->numNode == 0)
        return NULL;

    MinHeapNode* extractNode = minHeap->arr[0];
    MinHeapNode* lastNode = minHeap->arr[minHeap->numNode-1];

    minHeap->arr[0] = lastNode;

    minHeap->pos[extractNode->node] = minHeap->numNode-1;
    minHeap->pos[lastNode->node] = 0;

    minHeap->numNode--;
    heapify(minHeap, 0);

    return extractNode;
}

void decreaseKey(MinHeap* minHeap,int v, int dist){
	int idx = minHeap->pos[v];
    minHeap->arr[idx]->dist = dist;

    while(idx > 0 && minHeap->arr[idx]->dist < minHeap->arr[(idx-1)/2]->dist){
        minHeap->pos[minHeap->arr[(idx-1)/2]->node] = idx;
        minHeap->pos[minHeap->arr[idx]->node] = (idx-1)/2;
        swapNode(&minHeap->arr[(idx-1)/2], &minHeap->arr[idx]);
        idx = (idx-1)/2;
    }
}

void relax(MinHeap* minHeap, int* dist, int* path, Node* temp, int src){
    int v = temp->dest;
    if(minHeap->pos[v] < minHeap->numNode && dist[v] > temp->weight + dist[src]){
        path[v] = src;
        dist[v] = temp->weight + dist[src];
        decreaseKey(minHeap, v, dist[v]);
    }
}

void dijkstra(Graph* graph, int N){
	int dist[N], path[N];
	MinHeap* minHeap = createMinHeap(N);

	for (int i = 0; i < N; i++){
		dist[i] = INT_MAX;
		minHeap->arr[i] = newMinHeapNode(i,dist[i]);
		minHeap->pos[i] = i;
	}

	minHeap->arr[0] = newMinHeapNode(0, dist[0]);
	minHeap->pos[0] = 0;
	dist[0] = 0;
	decreaseKey(minHeap, 0, dist[0]);

	minHeap->numNode = N;

	while (minHeap->numNode != 0){
        int src;
        MinHeapNode* relaxNode;
        relaxNode = extractMin(minHeap);
        src = relaxNode->node;
        if (src == N-1)
            break;

        Node* temp = graph->srcArr[src].head;
        while(temp!= NULL){
            relax(minHeap, dist, path, temp, src);
            temp = temp->next;
        }
	}

	int max_weight = 0;
    int u, v;
    int ans;
    v = N-1;
    while(v != 0){
        u = path[v];
        Node* temp = graph->srcArr[u].head;
        while(temp->dest != v)
            temp = temp->next;

        if(temp->weight > max_weight)
            max_weight = temp->weight;
        v = u;
    }
    ans = dist[N-1] - max_weight;

    Node* temp = graph->srcArr[N-1].head;
    while(temp!= NULL){
        int max_weight = 0;
        int u, v;
        v = temp->dest;
        while(v != 0){
            u = path[v];
            Node* temp = graph->srcArr[u].head;
            while(temp->dest != v)
                temp = temp->next;

            if(temp->weight > max_weight)
                max_weight = temp->weight;
            v = u;
        }

        if(dist[temp->dest] + max_weight < ans)
            ans = dist[temp->dest] + max_weight;
        temp = temp->next;
    }

    printf("%d\n", ans);
}

int main()
{
	int N, M;
    int src, dest, weight;
    Graph* graph;
    scanf("%d %d", &N, &M);

    graph = (Graph*)malloc(sizeof(Graph));
    graph->srcArr = (NodeList*)malloc(N*sizeof(NodeList));
    for (int i = 0; i < N; i++)
        graph->srcArr[i].head = NULL;

    for(int i = 0; i < M; i++){
        scanf("%d %d %d", &src, &dest, &weight);
        createEdge(graph, src-1, dest-1, weight);
        createEdge(graph, dest-1, src-1, weight);
    }

    dijkstra(graph, N);

	return 0;
}
