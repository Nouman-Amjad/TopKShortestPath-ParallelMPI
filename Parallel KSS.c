/*
Muhammad Nouman Amjad (21I-0853)
    Moeez Muslim (21I-0490)
           E
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <omp.h>
#include <mpi.h>
#include <time.h>

#define V 100

int minDistance(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;
  
    #pragma omp parallel for shared(dist, sptSet) reduction(min:min) 
    for (int v = 0; v < V; v++) {
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
  
    return min_index;
}

int* findPath(int parent[], int dest, int* pathLength) {
    int* path = (int*)malloc(V * sizeof(int));
    int index = 0;
    int current = dest;

    while (current != -1) {
        path[index++] = current;
        current = parent[current];
    }

    *pathLength = index;
    return path;
}

int dijkstra(int graph[V][V], int src, int dest, int** path, int* pathLength) {
    int dist[V];
    bool sptSet[V];
    int parent[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
        parent[i] = -1;
    }
  
    dist[src] = 0;
  
    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);
  
        sptSet[u] = true;
  
        #pragma omp parallel for shared(graph, dist, sptSet, parent, u)
        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
    *path = findPath(parent, dest, pathLength);
    return dist[dest];
}

int main(int argc, char** argv) {

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
/*
    int graph[V][V] = {
        {0, 4, 0, 0, 0, 0, 0, 8, 0, 0},
        {4, 0, 8, 0, 0, 0, 0, 11, 0, 0},
        {0, 8, 0, 7, 0, 4, 0, 0, 2, 0},
        {0, 0, 7, 0, 9, 14, 0, 0, 0, 0},
        {0, 0, 0, 9, 0, 10, 0, 0, 0, 0},
        {0, 0, 4, 14, 10, 0, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 2, 0, 1, 6, 0},
        {8, 11, 0, 0, 0, 0, 1, 0, 7, 0},
        {0, 0, 2, 0, 0, 0, 6, 7, 0, 4},
        {0, 0, 0, 0, 0, 0, 0, 0, 4, 0}
    };*/
    clock_t st, et;

    int graph[V][V] = {0};
    if(rank ==0){
    FILE *file = fopen("email.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to open file\n");
        exit(1);
    }
    
    int start = 0;
    int end = start + V;
    printf("%d,%d \n", start, end);
    int r, c; 
    while (fscanf(file, "%d\t%d\n", &r, &c)) {
        if ((r >= start && r < end) && (c >= start && c < end)) {
            graph[r - start][c - start] = 1;
            //printf("%d,%d \n", r, c);
        }
        if(r-1 == end)
        break;
    }

    fclose(file);
    
    }
    bool directed = false;
    int k = 3;
    int pathnumber = 1;
    int dest = V - 1;
    int* path;
    int pathLength;
    int cost;
    int new_path;
    if(rank == 0)
        st = clock();
        cost = dijkstra(graph, 0, dest, &path, &pathLength);
        if(rank == 0){
        printf("\n%d Shortest Path:\n",pathnumber++);
        for (int i = pathLength - 1; i >= 0; i--)
            printf("%d ", path[i]);
        printf("\nCost : %d\n\n\n",cost);
    }
    MPI_Bcast(&pathLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(path, pathLength -1 , MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(graph, V*V, MPI_INT, 0, MPI_COMM_WORLD);
    for (int p = 1; p<k; p++)
    {
	    int min_cost = 2147483647;
	    
	    if(rank<pathLength-1)
	    {
            if (directed)
                graph[path[rank]][path[rank + 1]] = 0;
            else
		        graph[path[rank]][path[rank + 1]] = graph[path[rank + 1]][path[rank]] = 0;
		min_cost = dijkstra(graph, 0, dest, &path, &pathLength);
		/*
        if(pathLength == 1)
		printf("\nNo Path exist\n");
		else
		{
            for (int i = pathLength - 1; i >= 0; i--)
		        printf("%d ", path[i]);
		    printf("\n Cost : %d\n",min_cost);}*/
	    }
	    int sendarr[2];
	    sendarr[0] = min_cost;
	    sendarr[1] = rank;
	    MPI_Reduce(&min_cost, &cost, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	    //MPI_Barrier(MPI_COMM_WORLD);
	    int min_cost_rank_array[size*2];

	    MPI_Gather(sendarr, 2, MPI_INT, min_cost_rank_array, 2, MPI_INT, 0, MPI_COMM_WORLD);
	    bool flag = false;
	    int next = -1;
	    if (rank == 0) {
	    
		printf("Minimum cost of the path from Source to Destination: %d,%d\n",rank, cost);
		printf("Processes with minimum cost:\n");
		for (int i = 0; i < 2*size; i+=2) {
		    if (cost == min_cost_rank_array[i]) {
		    	next = min_cost_rank_array[i+1];
		        printf("Process %d\n", min_cost_rank_array[i+1]);
		        break;
		    }
		}
	    }
	    //MPI_Barrier(MPI_COMM_WORLD);
	    MPI_Bcast(&next, 1, MPI_INT, 0, MPI_COMM_WORLD);
	    
	    if (next != -1)
	    {
	    		MPI_Bcast(&pathLength,1, MPI_INT, next, MPI_COMM_WORLD);
	    		MPI_Bcast(&cost,1, MPI_INT, 0, MPI_COMM_WORLD);
	    		MPI_Bcast(path, pathLength +1, MPI_INT, next, MPI_COMM_WORLD);
	    		MPI_Bcast(graph, V*V, MPI_INT, next, MPI_COMM_WORLD);
	    		
	    	if(rank == 0)
	    	{
	    		
	    		printf("\n%d Shortest Path:\n",pathnumber++);
                for (int i = pathLength - 1; i >= 0; i--)
                    printf("%d ", path[i]);
                printf("\nCost : %d\n\n\n",cost);
                
	    	}
	    }
	    else
	    	break;
	    MPI_Barrier(MPI_COMM_WORLD);
	    
	    if(pathLength == 1)
	    {
	    	if(rank == 0)
	    	printf("\nNo more Shortest Paths\n\n");
            
	    	break;
	    }
	    MPI_Barrier(MPI_COMM_WORLD);
	    
    }
    if(rank == 0)
    {
        et = clock();
        double ts = ((double)(et - st)) / CLOCKS_PER_SEC;
        printf("\nTime Taken by Parallel Execution: %f s\n\n",ts);
    }
    MPI_Finalize();
	
    return 0;
}
