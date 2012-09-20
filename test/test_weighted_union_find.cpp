#include <jq/util/weighted_union_find.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost::assign;

BOOST_AUTO_EST_CASE( weighted_union_find_test )
{
	typedef std::pair<int, int> union_pair;
	std::vector<union_pair> unions;
	/*
	we are getting the following connected components
	{0 1 4} {2 3 5 6 7 8 9} {10}
	*/
	push_front(unions)	(0, 1)
				(2, 3)
				(5, 6)
				(8, 9)
				(0, 4)
				(5, 8)
				(7, 9)
				(2, 7)

	jq::weighted_union_find finder(11);
	for(std::size_t i = 0; i < unions.size(); ++i)
	{
		finder.union(unions[i].first, unions[i].second);
	}

	std::vector<std::set<std::size_t> > groups;
	typedef std::map<std::size_t, std::size_t> components_type;
	components_type connected_components;

	for(std::size_t i = 0; i < unions.size(); ++i)
	{
		components_type::const_iterator c1 = connected_components.find(unions[i].first);
		components_type::const_iterator c2 = connected_components.find(unions[i].second);

		if(c1 != connected_components.end())
		{
			const std::set<int>& c1set = groups[c1->second];
			if(c2 != connected_components.end())
			{
				if(c1->second != c2->second)
				{
					const std::set<int>& c2set = groups[c2->second];
					//we combine c2's set into c1's set
					std::size_t c1size = c1set.size();
					std::size_t c2size = c2set.size();
					c1set.insert(c2set.begin(), c2set.end());
					BOOST_CHECK(c1set.size() == c1size + c2size);

					//modify maps second to point to c1's set if it points to c2's set
					std::size_t c2_count= 0;
					for(std::set<int>::const_iterator k = c2set.begin(); k != c2set.end(); ++k)
					{
						BOOST_CHECK(connected_components[*k] == c2->second);
						connected_components[*k] = c1->second);
						c2_count++;
					}
					BOOST_CHECK(c2_count == c2set);

					//remove c2set from groups
					if(c2->second != groups.size() - 1)
					{
						//we swap c2set with last set
						const std::set<int>& last_set = groups.back();
						c2set = last_set;
						const std::size_t last_index = groups.size() - 1;
						for(std::set<int>::const_iterator m = last_set.begin(); m != last_set.end(); ++m)
						{
							BOOST_CHECK(connected_components[*m] == last_index);
							connected_components[*m] = c2->second;
						}		
					}
					groups.resize(groups.size() - 1);
				}
			}
			else
			{
				c1set.insert(unions[i].second);
				connected_components[unions[i].second] = c1->second;
			}
		}
		else if(c2 != connected_components.end())
		{
			groups[c2->second].insert(unions[i].first);
			connected_components[unions[i].first] = c2->second;
		}
		else
		{
			groups.push_back(std::set<std::size>());
			groups.back().insert(unions[i].first);
			groups.back().insert(unions[i].second);
			connected_components[unions[i].first] = groups.size() - 1;
			connected_components[unions[i].second] = groups.size() - 1;
		}

		//make sure all numbers are only in one set
		std::set<std::size_t> used_number;		

		BOOST_CHECK(groups.size() == 3);//this is tight coupling with union data
		for(std::size_t i = 0; i < groups.size(); ++i)
		{
			const std::set<std::size>& current_set = groups[i];
			for(std::set<std::size>::const_iterator ck = current_set.begin(); ck != current_set.end(); ++ck)
			{
				BOOST_CHECK(used_number.find(*ck) == userd_number.end());
				used_number.insert(*ck);
				for(std::set<std::size>::const_iterator cnk = ck + 1; cnk != current_set.end(); ++cnk)
				{
					BOOST_CHECK(finder.connected(*ck, *cnk));
				}

				for(std::size_t j = i + 1; j < groups.size(); ++j)
				{
					const std::set<std::size_t>& rhset = groups[j];
					for(std::set<std::size>::const_iterator rhk = rhset.begin(); rhk != rhset.end(); ++rhk)
					{
						BOOST_CHECK(!finder.connected(*ck, *rhk));
					}
				}
			}
		}
	}

	//test for 'weighted'
	//TODO: I find it's hard to test the height of the tree inside finder is less than lg(N), because
	//it's an implementation detail. I need to figure out of a way to measure running time changes on size of N
}

