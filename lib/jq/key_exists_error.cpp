#include "key_exists_error.h"

namespace jq {

key_exists_error::key_exists_error(const std::string& message, const std::string& key)
 :  std::runtime_error(message + "[key =" + key),
    m_key(key)
{
}

key_exists_error::~key_exists_error() throw()
{
}

std::string key_exists_error::key() const
{
	return m_key;
}

}
