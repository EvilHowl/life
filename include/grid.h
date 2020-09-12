#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

class Grid
{
public:
    Grid() = default;
    Grid(const Grid& g);

    ~Grid();

    bool alloc(int rows, int cols);

    [[nodiscard]] int cols() const { return m_cols; };
    [[nodiscard]] int rows() const { return m_rows; };

    bool& operator()(int row, int col) noexcept;
    const bool& operator()(int row, int col) const noexcept;

    Grid& operator=(Grid&& g) noexcept;
    Grid& operator=(const Grid& g);

private:
    bool* m_grid = nullptr;
    size_t m_size = 0;

    int m_cols = 0;
    int m_rows = 0;
};
