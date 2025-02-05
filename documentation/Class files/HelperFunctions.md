## Core Purpose
Header-only utility functions for random generation needs.

## Available Functions

### Random String Generation
```cpp
// Generate random alphanumeric string
std::string id = random_string(10);  // "aB3kX9pL2q"
```

### Random Integer Generation
```cpp
// Generate random int in range
int value = getRandomNumber(1, 100);  // 1-100 inclusive
```

## Key Features
1. Header-only implementation
2. Modern C++ random generators
3. Alphanumeric character set
4. Inclusive range for integers
5. Thread-safe random device

Used by:
- [[MinigameManager]]

Note: Uses `<random>` for better randomization than rand()