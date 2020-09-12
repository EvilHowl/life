#include "../include/grid.h"

Grid::Grid(const Grid& g)
{
    alloc(g.m_rows, g.m_cols);
    memcpy(m_grid, g.m_grid, m_size);
}

Grid::~Grid()
{
    if (m_grid != nullptr)
        free(m_grid);
}

bool Grid::alloc(int rows, int cols)
{
    if (rows > 0 && cols > 0)
    {
        m_rows = rows;
        m_cols = cols;
    }
    else
        return false;

    m_size = (m_rows * m_cols) * sizeof(bool);

    m_grid = static_cast<bool*>(realloc(m_grid, m_size));

    memset(m_grid, 0, m_size);

    //Check if allocation worked
    if (m_grid != nullptr)
        return true;
    else
        return false;
}

bool& Grid::operator()(int row, int col) noexcept
{
    return m_grid[row * m_cols + col];
}

const bool& Grid::operator()(int row, int col) const noexcept
{
    return m_grid[row * m_cols + col];
}
Grid& Grid::operator=(Grid&& g) noexcept
{
    if (this != &g)
    {
        free(m_grid);

        m_cols = g.m_cols;
        m_rows = g.m_rows;

        m_size = g.m_size;

        m_grid = g.m_grid;

        g.m_grid = nullptr;
        g.m_cols = 0;
        g.m_rows = 0;
        g.m_size = 0;
    }
    return *this;
}

Grid& Grid::operator=(const Grid& g)
{
    if(&g != this)
    {
        alloc(g.m_rows, g.m_cols);
        memcpy(m_grid, g.m_grid, m_size);
    }

    return *this;
}
