#include "client.hpp"

client::client()
{
}

client::~client()
{
}

void*	client::getBuffer() const
{
	return this->_buffer;
}
