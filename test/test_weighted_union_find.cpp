#include "weighted_union_find.h"
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <set>
#include <map>

using namespace boost::unit_test;
using namespace boost::assign;

BOOST_AUTO_TEST_CASE( weighted_union_find_naive_test )
{
	const std::size_t size = 10;
	jq::weighted_union_find finder(size);
	for(std::size_t i = 0; i < size; ++i)
	{
		for(std::size_t j = i + 1; j < size; ++j)
		{
			//after initialization, everythin is not connecte
			BOOST_CHECK(!finder.connected(i, j));
		}
	}

	//all even numbers are connected
	finder.connect(0, 2);
	finder.connect(0, 4);
	finder.connect(2, 6);
	finder.connect(4, 8);

	//all odd numbers are connected
	finder.connect(1, 3);
	finder.connect(1, 5);
	finder.connect(5, 7);
	finder.connect(3, 9);

	for(std::size_t i = 0; i < size; ++i)
	{
		for(std::size_t j = i + 1; j < size; ++j)
		{
			BOOST_CHECK(((j - i) % 2 == 0) == finder.connected(i, j));
		}
	}
}

BOOST_AUTO_TEST_CASE( weighted_union_find_test )
{
	typedef std::pair<int, int> union_pair;
	std::vector<union_pair> unions;
	//we are getting the following connected components{0 1 4} {2 3 5 6 7 8 9} {10 11}
	push_back(unions)	(0, 1)
				(2, 3)
				(5, 6)
				(8, 9)
				(0, 4)
				(5, 8)
				(7, 9)
				(2, 7)
				(10, 11);

	BOOST_CHECK(unions.size() == 9);

	jq::weighted_union_find finder(12);
	for(std::size_t i = 0; i < unions.size(); ++i)
	{
		finder.connect(unions[i].first, unions[i].second);
	}

	std::vector<std::set<std::size_t> > groups;
	typedef std::map<std::size_t/*id*/, std::size_t/*set index*/> components_type;
	components_type connected_components;

	for(std::size_t i = 0; i < unions.size(); ++i)
	{
		const std::size_t a = unions[i].first;
		const std::size_t b = unions[i].second;

		components_type::const_iterator ai = connected_components.find(a);
		components_type::const_iterator bi = connected_components.find(b);

		if(ai != connected_components.end())
		{
			const std::size_t aset_index = ai->second;
			std::set<std::size_t>& aset = groups[aset_index];
			if(bi != connected_components.end())
			{
				const std::size_t bset_index = bi->second;
				std::set<std::size_t>& bset = groups[bset_index];
				if(aset_index != bset_index)
				{
					//we combine b's set into a's set
					const std::size_t aset_size = aset.size();
					const std::size_t bset_size = bset.size();
					aset.insert(bset.begin(), bset.end());
					BOOST_CHECK(aset.size() == aset_size + bset_size);

					//modify maps second to point to a's set if it points to b's set
					for(std::set<std::size_t>::const_iterator k = bset.begin(); k != bset.end(); ++k)
					{
						BOOST_CHECK(connected_components[*k] == bset_index);
						connected_components[*k] = aset_index;
					}

					//remove bset from groups
					if(bset_index != groups.size() - 1)
					{
						//we swap biset with last set
						bset.clear();
						const std::set<std::size_t>& last_set = groups.back();
						bset.insert(last_set.begin(), last_set.end());
						const std::size_t last_index = groups.size() - 1;
						for(std::set<std::size_t>::const_iterator m = last_set.begin(); m != last_set.end(); ++m)
						{
							BOOST_CHECK(connected_components[*m] == last_index);
							connected_components[*m] = bi->second;
						}		
					}
					groups.erase(groups.end() - 1);
				}
			}
			else
			{
				aset.insert(b);
				connected_components[b] = aset_index;
			}
		}
		else if(bi != connected_components.end())
		{
			const std::size_t bset_index = bi->second;
			groups[bset_index].insert(a);
			connected_components[a] = bset_index;
		}
		else
		{
			groups.push_back(std::set<std::size_t>());
			groups.back().insert(a);
			groups.back().insert(b);
			connected_components[a] = groups.size() - 1;
			connected_components[b] = groups.size() - 1;
		}
	}

	BOOST_CHECK(groups.size() == 3);//this is tight coupling with union data

	//make sure all numbers are only in one set
	std::set<std::size_t> used_number;	

	for(std::size_t i = 0; i < groups.size(); ++i)
	{
		const std::set<std::size_t>& current_set = groups[i];
		for(std::set<std::size_t>::const_iterator ck = current_set.begin(); ck != current_set.end(); ++ck)
		{
			BOOST_CHECK(used_number.find(*ck) == used_number.end());
			used_number.insert(*ck);
			std::set<std::size_t>::const_iterator cnk = ck;
			for(++cnk; cnk != current_set.end(); ++cnk)
			{
				BOOST_CHECK(finder.connected(*ck, *cnk));
			}

			for(std::size_t j = i + 1; j < groups.size(); ++j)
			{
				const std::set<std::size_t>& rhset = groups[j];
				for(std::set<std::size_t>::const_iterator rhk = rhset.begin(); rhk != rhset.end(); ++rhk)
				{
					BOOST_CHECK(!finder.connected(*ck, *rhk));
				}
			}
		}
	}

	//test for 'weighted'
	//TODO: I find it's hard to test the height of the tree inside finder is less than lg(N), because
	//it's an implementation detail. I need to figure out of a way to measure running time changes on size of N
}
