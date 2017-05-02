#include <cstdio>
#include <unistd.h>
int main() {
    int a;
    while(scanf("%d", &a) != EOF) {
        printf("%d %d\n", 2, a - 1);
    }
}
