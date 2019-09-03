# Shunting-Yard-Algorithm

```cpp
#include "ShuntYard.hpp"

int main() {
    std::string input = {};

    while (std::cout << "\n> " && std::getline(std::cin, input)) {
        shuntYard::compute(std::move(input));
    }
    return 0;
}
```
