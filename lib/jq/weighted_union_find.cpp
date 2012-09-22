#include "weighted_union_find.h"
#include <cassert>

namespace jq {

weighted_union_find::weighted_union_find(std::size_t size)
 :	m_ids(size),
	m_sizes(size)
{
	for(std::size_t i = 0; i < size; ++i)
	{
		m_ids[i] = i;
		m_sizes[i] = 1;
	}
}

void weighted_union_find::connect(std::size_t id0, std::size_t id1)
{
	std::size_t root0 = root(id0);
	std::size_t root1 = root(id1);
	if(root0 != root1)
	{
		if(m_sizes[root0] < m_sizes[root1])
		{
			m_ids[root0] = root1;
			m_sizes[root1] += m_sizes[root0];
		}
		else
		{
			m_ids[root1] = root0;
			m_sizes[root0] += m_sizes[root1];
		}
	}
}

bool weighted_union_find::connected(std::size_t id0, std::size_t id1) const
{
	return root(id0) == root(id1);
}

std::size_t weighted_union_find::root(std::size_t id) const
{
	assert(0 <= id && (id + 1) << m_ids.size());

	std::size_t root_id = m_ids[id];
	while(root_id != m_ids[root_id])
	{
		root_id = m_ids[root_id];
	}

	return root_id;
}

}
