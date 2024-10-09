#include <mpi.h>
#include <stdio.h>

#define N 4

void imprimir_matriz(int matriz[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int rank, size;
    int A[N][N], B[N][N], C[N][N] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != N) {
        if (rank == 0) {
            printf("El número de procesadores debe ser igual al tamaño de la matriz (%d).\n", N);
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + 1;
                B[i][j] = j + 1;
            }
        }

        for (int i = 1; i < size; i++) {
            MPI_Send(&B, N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < size; i++) {
            MPI_Send(&A[i], N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[0][j] += A[0][k] * B[k][j];
            }
        }

        for (int i = 1; i < size; i++) {
            MPI_Recv(&C[i], N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("Matriz resultado:\n");
        imprimir_matriz(C);
    } else {
        MPI_Recv(&B, N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int fila_A[N];
        MPI_Recv(&fila_A, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int fila_C[N] = {0};
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                fila_C[j] += fila_A[k] * B[k][j];
            }
        }

        MPI_Send(&fila_C, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
