void foo(int **a, int **b, int **c, int **d, int N, ...){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            a[i][j] = 1 / b[i][j] * c[i][j];
        }
    }

    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            d[i][j] = a[i + 1][j] + c[i][j];
        }
    }
}