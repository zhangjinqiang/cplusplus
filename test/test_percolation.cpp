#include "percolation.h"
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

using namespace boost::unit_test;
using namespace boost::assign;

void populate_grid(jq::percolation& perco, const std::vector<std::vector<bool> >& grid)
{
	for(std::size_t i = 0; i < grid.size(); ++i)
	{
		const std::vector<bool>& row = grid[i];
		for(std::size_t j = 0; j < row.size(); ++j)
		{
			if(row[j]){
				perco.open(i, j);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE( percolation_open_test )
{
	//in the following testing data, 0 means closed, 1 means open
	const std::size_t rows_num = 9;
	const std::size_t cols_num = 8;
	std::vector<std::vector<bool> > grid(rows_num);
	grid[0] += 1, 0, 0, 0, 1, 0, 0, 1;
	grid[1] += 1, 1, 1, 0, 1, 0, 1, 0;
	grid[2] += 0, 0, 1, 0, 0, 0, 1, 1;
	grid[3] += 0, 1, 1, 0, 0, 0, 0, 1;
	grid[4] += 0, 1, 0, 0, 1, 1, 1, 1;
	grid[5] += 0, 1, 1, 1, 1, 0, 0, 0;
	grid[6] += 0, 0, 0, 0, 0, 1, 1, 0;
	grid[7] += 0, 1, 0, 0, 1, 0, 1, 0;
	grid[8] += 1, 1, 1, 1, 1, 1, 1, 0;

	jq::percolation perco(rows_num, cols_num);
 	
	populate_grid(perco, grid);

	//opened cells in line0 is open
	BOOST_CHECK( perco.is_open(0, 0));
	BOOST_CHECK(!perco.is_open(0, 1));
	BOOST_CHECK(!perco.is_open(0, 2));
	BOOST_CHECK(!perco.is_open(0, 3));
	BOOST_CHECK( perco.is_open(0, 4));
	BOOST_CHECK(!perco.is_open(0, 5));
	BOOST_CHECK(!perco.is_open(0, 6));
	BOOST_CHECK( perco.is_open(0, 7));

	//line1 test
	BOOST_CHECK( perco.is_open(1, 0));
	BOOST_CHECK( perco.is_open(1, 1));
	BOOST_CHECK( perco.is_open(1, 2));
	BOOST_CHECK(!perco.is_open(1, 3));
	BOOST_CHECK( perco.is_open(1, 4));
	BOOST_CHECK(!perco.is_open(1, 5));
	BOOST_CHECK( perco.is_open(1, 6));
	BOOST_CHECK(!perco.is_open(1, 7));

	//line2 test
	BOOST_CHECK(!perco.is_open(2, 0));
	BOOST_CHECK(!perco.is_open(2, 1));
	BOOST_CHECK( perco.is_open(2, 2));
	BOOST_CHECK(!perco.is_open(2, 3));
	BOOST_CHECK(!perco.is_open(2, 4));
	BOOST_CHECK(!perco.is_open(2, 5));
	BOOST_CHECK( perco.is_open(2, 6));
	BOOST_CHECK( perco.is_open(2, 7));

	//line3 test
	BOOST_CHECK(!perco.is_open(3, 0));
	BOOST_CHECK( perco.is_open(3, 1));
	BOOST_CHECK( perco.is_open(3, 2));
	BOOST_CHECK(!perco.is_open(3, 3));
	BOOST_CHECK(!perco.is_open(3, 4));
	BOOST_CHECK(!perco.is_open(3, 5));
	BOOST_CHECK(!perco.is_open(3, 6));
	BOOST_CHECK( perco.is_open(3, 7));

	//line4 test
	BOOST_CHECK(!perco.is_open(4, 0));
	BOOST_CHECK( perco.is_open(4, 1));
	BOOST_CHECK(!perco.is_open(4, 2));
	BOOST_CHECK(!perco.is_open(4, 3));
	BOOST_CHECK( perco.is_open(4, 4));
	BOOST_CHECK( perco.is_open(4, 5));
	BOOST_CHECK( perco.is_open(4, 6));
	BOOST_CHECK( perco.is_open(4, 7));

	//line5 test
	BOOST_CHECK(!perco.is_open(5, 0));
	BOOST_CHECK( perco.is_open(5, 1));
	BOOST_CHECK( perco.is_open(5, 2));
	BOOST_CHECK( perco.is_open(5, 3));
	BOOST_CHECK( perco.is_open(5, 4));
	BOOST_CHECK(!perco.is_open(5, 5));
	BOOST_CHECK(!perco.is_open(5, 6));
	BOOST_CHECK(!perco.is_open(5, 7));

	//line6 test
	BOOST_CHECK(!perco.is_open(6, 0));
	BOOST_CHECK(!perco.is_open(6, 1));
	BOOST_CHECK(!perco.is_open(6, 2));
	BOOST_CHECK(!perco.is_open(6, 3));
	BOOST_CHECK(!perco.is_open(6, 4));
	BOOST_CHECK(!perco.is_open(6, 5));
	BOOST_CHECK(!perco.is_open(6, 6));
	BOOST_CHECK(!perco.is_open(6, 7));

	//line7 test
	BOOST_CHECK(!perco.is_open(7, 0));
	BOOST_CHECK(!perco.is_open(7, 1));
	BOOST_CHECK(!perco.is_open(7, 2));
	BOOST_CHECK(!perco.is_open(7, 3));
	BOOST_CHECK(!perco.is_open(7, 4));
	BOOST_CHECK(!perco.is_open(7, 5));
	BOOST_CHECK(!perco.is_open(7, 6));
	BOOST_CHECK(!perco.is_open(7, 7));

	//line8 test
	BOOST_CHECK(!perco.is_open(8, 0));
	BOOST_CHECK(!perco.is_open(8, 1));
	BOOST_CHECK(!perco.is_open(8, 2));
	BOOST_CHECK(!perco.is_open(8, 3));
	BOOST_CHECK(!perco.is_open(8, 4));
	BOOST_CHECK(!perco.is_open(8, 5));
	BOOST_CHECK(!perco.is_open(8, 6));
	BOOST_CHECK(!perco.is_open(8, 7));
}

BOOST_AUTO_TEST_CASE( percolation_test_0 )
{
	//in the following testing data, 0 means closed, 1 means open

	const std::size_t rows_num = 9;
	const std::size_t cols_num = 8;
	std::vector<std::vector<bool> > grid(rows_num);
	grid[0] += 0, 1, 0, 0, 1, 0, 0, 1;
	grid[1] += 0, 1, 1, 0, 1, 0, 1, 1;
	grid[2] += 0, 0, 1, 0, 1, 0, 1, 0;
	grid[3] += 0, 1, 1, 0, 0, 0, 1, 0;
	grid[4] += 0, 1, 0, 0, 1, 1, 1, 0;
	grid[5] += 0, 1, 1, 1, 1, 0, 0, 0;
	grid[6] += 0, 0, 0, 0, 0, 1, 1, 0;
	grid[7] += 0, 1, 0, 0, 1, 0, 1, 0;
	grid[8] += 0, 1, 0, 0, 1, 0, 1, 0;

	jq::percolation perco(rows_num, cols_num);
 	
	populate_grid(perco, grid);

	BOOST_CHECK(!perco.percolates());
}

BOOST_AUTO_TEST_CASE( percolation_test_1 )
{
	//in the following testing data, 0 means closed, 1 means open

	const std::size_t rows_num = 9;
	const std::size_t cols_num = 8;
	std::vector<std::vector<bool> > grid(rows_num);
	grid[0] += 0, 1, 0, 0, 0, 0, 0, 0;
	grid[1] += 1, 1, 0, 1, 1, 1, 1, 0;
	grid[2] += 1, 0, 0, 1, 0, 0, 1, 1;
	grid[3] += 1, 1, 0, 1, 1, 0, 0, 1;
	grid[4] += 0, 1, 0, 0, 1, 0, 0, 1;
	grid[5] += 0, 1, 1, 1, 1, 0, 1, 1;
	grid[6] += 0, 0, 0, 0, 0, 1, 1, 0;
	grid[7] += 0, 1, 1, 1, 1, 1, 0, 0;
	grid[8] += 0, 1, 0, 0, 0, 0, 0, 0;

	jq::percolation perco(rows_num, cols_num);
 	
	populate_grid(perco, grid);

	BOOST_CHECK(perco.percolates());
}
