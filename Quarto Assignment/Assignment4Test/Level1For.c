void foo(int a[], int b[], int c[], int d[], int N){
    for (int i = 0; i < N; i++){
        a[i] = 1 / b[i] * c[i];
    }

    for (int i = 0; i < N; i++){
        d[i] = a[i + 5] + c[i];     
    }
    
}

#if 0
int main(){
    const int N = 10;
    int a[N], b[N], c[N], d[N];
    foo(a, b, c, d, N);
    return 0;
}
#endif