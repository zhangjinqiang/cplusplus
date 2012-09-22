#ifndef JQ_WEIGHTED_UNION_FIND_H
#define JQ_WEIGHTED_UNION_FIND_H

#include <vector>

namespace jq
{
	class weighted_union_find
	{
	public:
		weighted_union_find(std::size_t size);

		//since union is a key word of c++
		void connect(std::size_t id0, std::size_t id1);

		bool connected(std::size_t id0, std::size_t id1) const;

	private:
		std::size_t root(std::size_t id) const;

		std::vector<std::size_t> m_ids;
		std::vector<std::size_t> m_sizes;
	};
}

#endif

