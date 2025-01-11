# Top K Shortest Path Problem with MPI and OpenMP

## Overview
This project tackles the **Top K Shortest Path Problem**, a challenging issue in graph theory and optimization. The solution is implemented using **MPI** for distributed computing and **OpenMP** for shared memory parallelization within MPI processes. It includes both serial and parallel implementations for comparison.

---

## Features
### **Key Highlights**
- Efficiently finds the top K shortest paths in a graph.
- Utilizes MPI for parallelizing computation across distributed systems.
- Employs OpenMP for loop-level parallelization within MPI processes.
- Processes large datasets like the Enron Email graph.

---

## Implementation Details
### **Parallelization Strategy**
1. **MPI**:
   - Divides computation of the K shortest paths among processes.
   - Each MPI process modifies one edge from the shortest path and computes the new shortest path.
   - Uses `MPI_Reduce` to select the minimum path among all processes.
2. **OpenMP**:
   - Parallelizes loops for selecting the minimum distance and updating paths within each MPI process.

### **Serial Implementation**
- Calculates the shortest paths sequentially for comparison with the parallel solution.

---

## Results
### **Performance Comparison**
- **Parallel Execution**:
  - Utilized MPI and OpenMP for distributed and shared memory parallelization.
  - Observed communication overhead when running on a single machine.
- **Serial Execution**:
  - Faster than parallel execution when run on a single machine due to reduced communication overhead.

### **Challenges**
- Handling disconnected graph components during testing.
- Optimizing communication overhead for small-scale parallel execution.

---

## Installation and Setup

### **Prerequisites**
- **MPI**: Install an MPI implementation such as OpenMPI.
- **OpenMP**: Ensure OpenMP support is enabled in your compiler.

### **Compilation**
1. Compile the **parallel version**:
   ```bash
   mpicc -fopenmp -o parallel ParallelKSS.c
2. Compile the **serial version**:
   ```bash
   gcc -o serial SerialKSS.c
### **Execution**
1. Define the machinefile with distribution:
   ```bash
   Master: 5
   Slave:  5
2. Run the **parallel** program:
   ```bash
   mpiexec -n 30 -f machinefile ./parallel
3. Run the **serial** program:
   ```bash
   ./serial

---

## How It Works

### **1. Input Handling**
- Reads the graph data from `email.txt`.
- Initializes the distance matrix and marks unreachable vertices.

### **2. Shortest Path Calculation**
- Implements Yen's K-shortest path algorithm:
  - Computes the shortest path.
  - Removes one edge from the shortest path and recomputes.
  - Repeats for all edges to find the next shortest path.

### **3. Result Consolidation**
- **Parallel Execution**:
  - Reduces computational load per process.
- **Serial Execution**:
  - Calculates all paths sequentially.

---

## File Structure

- `ParallelKSS.c`: Parallel implementation using MPI and OpenMP.
- `SerialKSS.c`: Serial implementation for benchmarking.
- `email.txt`: Graph data used for testing.
- `README.md`: Documentation for the project.

---

## Future Enhancements

- Optimize communication overhead in parallel execution.
- Extend the implementation to handle dynamic graphs.
- Scale the solution for distributed systems with large computational resources.

---

## Acknowledgments

This project was developed as part of the **Parallel and Distributed Computing** course by:
- **Muhammad Nouman Amjad**
- **Moeez Muslim**

Special thanks to the faculty for their guidance.

---

## License

This project is licensed under the [MIT License](LICENSE).
