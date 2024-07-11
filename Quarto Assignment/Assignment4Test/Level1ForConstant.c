void foo(int a[], int b[], int c[], int d[]){

    for (int i = 0; i < 10; i++){
        a[i] = 1 / b[i] * c[i];
    }

    for (int i = 0; i < 10; i++){
        d[i] = a[i] + c[i];     
    }
}

#if 0
int main(){
    int a[N], b[N], c[N], d[N];
    foo(a, b, c, d);
    return 0;
}
#endif  