#ifndef JQ_KEY_EXISTS_ERROR_H
#define JQ_KEY_EXISTS_ERROR_H

#include <stdexcept>

namespace jq
{
	class key_exists_error : public std::runtime_error
	{
	public:
		key_exists_error(const std::string& message, const std::string& key);
		~key_exists_error() throw();

		std::string key() const;

	private:
		std::string m_key;
	};
}

#endif

