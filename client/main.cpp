#include "client.h"

int main()
{
    client client("127.0.0.1", 10000);
    client.start();
}
