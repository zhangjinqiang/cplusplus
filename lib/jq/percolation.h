#ifndef JQ_PERCOLATION_H
#define JQ_PERCOLATION_H

#include "weighted_union_find.h"
#include <boost/multi_array.hpp>

namespace jq
{
	class percolation
	{
	public:
		percolation(std::size_t rows, std::size_t cols);

		void open(std::size_t row, std::size_t col);

		//does it connect to any cell on top row
		bool is_open(std::size_t row, std::size_t col) const;

		bool percolates() const;

	private:
		weighted_union_find m_finder;
		std::size_t m_virtual_top;
		std::size_t m_virtual_bottom;
		std::size_t m_rows;
		std::size_t m_cols;
		boost::multi_array<bool, 2> m_open;
	};
}

#endif

