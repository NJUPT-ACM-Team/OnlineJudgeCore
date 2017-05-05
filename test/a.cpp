#include <cstdio>
#include <unistd.h>
int main() {
    int a;
    while(scanf("%d", &a) != EOF) {
        printf("%d %d\n", 1, a - 1);
    }
}
