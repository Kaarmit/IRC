
#include "client.hpp"

client::client()
{
}

client::client(int clientFd): _fd(clientFd) {
}

client::~client()
{
}