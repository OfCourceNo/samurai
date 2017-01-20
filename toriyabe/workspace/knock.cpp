#include <iostream>
#include <vector>


void print(std::vector<std::vector<int> > v) {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            std::cout << v[i][j] << " "; 
        }
        std::cout << std::endl;
    }
    return;
}

int main(void) {
    std::vector<std::vector<int> > kill = std::vector<std::vector<int> >(15,std::vector<int>(15, 0));
    print(kill);
    return 0;
}
