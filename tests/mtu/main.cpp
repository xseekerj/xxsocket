#include "yasio/xxsocket.hpp"
#include "yasio/byte_buffer.hpp"

int main()
{
  using namespace yasio;
  printf("Testing udp mtu ...\n");

  xxsocket s;
  s.popen(AF_INET, SOCK_DGRAM, 0);

  std::vector<ip::endpoint> endpoints;
  xxsocket::resolve(endpoints, "github.com", 51122);
  if (endpoints.empty())
  {
    printf("%s", "resolve host fail\n");
    return ENOENT;
  }

  yasio::byte_buffer data;
  data.resize(65535, '1');

  // set sndbuf match with data size, while, linux kenerl will double it
  int os_sndbuf_size = s.get_optval<int>(SOL_SOCKET, SO_SNDBUF);
  printf("old sndbuf=%d\n", os_sndbuf_size);
  int iret = s.set_optval(SOL_SOCKET, SO_SNDBUF, data.size());
  if (iret == 0)
  {
    os_sndbuf_size = s.get_optval<int>(SOL_SOCKET, SO_SNDBUF);
    printf("set sockopt SNDBUF succed, sndbuf=%d\n", os_sndbuf_size);
  }
  else
  {
    auto error = xxsocket::get_last_errno();
    printf("set sockopt SNDBUF fail: ec: %d, detail: %s\n", error, xxsocket::strerror(error));
  }

  int error = 0;
  int nret  = 0;
  int tries = 0; // try increase sndbuf size
  do
  {
    nret = s.sendto(data.data(), data.size(), endpoints[0]);
    if (nret == data.size())
    {
      printf("[%d] send data succeed, %zu bytes transferred\n", tries + 1, data.size());
    }
    else
    {
      error = xxsocket::get_last_errno();
      printf("[%d] sendto data %zu fail, sndbuf=%d: ec: %d, detail: %s\n", tries + 1, data.size(), s.get_optval<int>(SOL_SOCKET, SO_SNDBUF), error, xxsocket::strerror(error));
      ++tries;
      if (tries == 1) {
        if(os_sndbuf_size <= data.size()) {
            s.set_optval(SOL_SOCKET, SO_SNDBUF, os_sndbuf_size + 28);
        }
        else {
            printf("linux kernel sndbuf %d, greater than data size: %zu\n", os_sndbuf_size, data.size());
        }
      } else if(tries == 2) {
        data.resize(data.size() - 28); // decrease ip and udp header size
      }
    }
  } while (nret < 0 && error == EMSGSIZE && tries <= 2);

  return 0;
}
