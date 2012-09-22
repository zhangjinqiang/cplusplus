#include "percolation.h"

namespace jq {
	
percolation::percolation(std::size_t rows, std::size_t cols)
 :	m_finder(rows * cols + 2),
	m_virtual_top(rows * cols),
	m_virtual_bottom(rows * cols + 1),
	m_rows(rows),
	m_cols(cols),
	m_open(boost::extents[rows][cols])
{
	assert(1 < rows && 1 < cols);
	for(std::size_t r = 0; r < rows; ++r)
	{
		for(std::size_t c = 0; c < cols; ++c)
		{
			m_open[r][c] = false;
		}
	}
}

void percolation::open(std::size_t row, std::size_t col)
{
	if(m_rows <= row)
	{
		throw std::out_of_range("rows out of range");
	}
	if(m_cols <= col)
	{
		throw std::out_of_range("cols out of range");
	}

	const std::size_t index = row * m_cols + col;
	if(!m_open[row][col])
	{
		m_open[row][col] = true;
		if(0 < row){
			if(m_open[row - 1][col]){
				m_finder.connect(index, (row - 1) * m_cols + col);
			}
		}
		if(row + 1 < m_rows){
			if(m_open[row + 1][col]){
				m_finder.connect(index, (row + 1) * m_cols + col);
			}
		}
		if(0 < col){
			if(m_open[row][col - 1]){
				m_finder.connect(index, index - 1);
			}
		}
		if(col + 1 < m_cols){
			if(m_open[row][col + 1]){
				m_finder.connect(index, index + 1);
			}
		}

		if(0 == row){
			m_finder.connect(index, m_virtual_top);
		}
		else if(row + 1 == m_rows){
			m_finder.connect(index, m_virtual_bottom);
		}
	}
}

bool percolation::is_open(std::size_t row, std::size_t col) const
{
	return m_finder.connected(m_virtual_top, row * m_cols + col);
}

bool percolation::percolates() const
{
	return m_finder.connected(m_virtual_top, m_virtual_bottom);
}

}
