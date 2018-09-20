//sgn
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <thread>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

#define MAX_TIMEOUT_INTERVAL_SECS (120)
#define MAX_WAKEUP_INTERVAL_SECS (60)

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

unsigned int last_time_stamp = 0;

void thread_function() {
  std::string aura_bin_path = MAKE_STR(AURA_BIN_PATH);
  std::string aura_bin_file = MAKE_STR(AURA_BIN_FILE);
  std::string aura_bin_with_path  = aura_bin_path + std::string("/") + aura_bin_file;

  //  The command is "pidof -s SoapSeller"
  std::string pid_of_cmd  = std::string("pidof -s ") + aura_bin_file;

  while(true) {
    std::cout << "Last time stamp " << last_time_stamp << ", Cur Time " << time(0) << ", Delta " << (time(0) - last_time_stamp) << std::endl;
    if(MAX_TIMEOUT_INTERVAL_SECS <= (time(0) - last_time_stamp)) {
      std::cout << "time elapsed, so killing SoapSeller" << std::endl;
  		char buf[512];
  		FILE *cmd_pipe = popen(pid_of_cmd.c_str(), "r");
  		fgets(buf, 512, cmd_pipe);
  		pid_t pid = strtoul(buf, NULL, 10);
  		pclose( cmd_pipe );
  		if(0 < pid) {
  			std::cout << "killing process pid " << pid << std::endl;
  			kill(pid, 9);
  		} else {
  			std::cout << "Invalid pid " << pid << ". Seems " << aura_bin_file << " is not running."<< std::endl;
  		}
      std::cout << "Starting SoapSeller " << aura_bin_with_path << " after 3 secs" << std::endl;
      sleep(3);
      std::system(aura_bin_with_path.c_str());
      last_time_stamp = time(0);
    }
    sleep(MAX_WAKEUP_INTERVAL_SECS);
  }
}

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : socket_(io_service, udp::endpoint(udp::v4(), port)) {
    do_receive();
  }

  void do_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
          if (!ec && bytes_recvd > 0)
          {
		data_[bytes_recvd] = '\0';
		last_time_stamp = std::stoul(std::string(data_));
		std::cout << "Updating last time stamp " << last_time_stamp << std::endl;
           }
	do_receive();

        });
  }

  void do_send(std::size_t length)
  {
    socket_.async_send_to(
        boost::asio::buffer(data_, length), sender_endpoint_,
        [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
        {
          do_receive();
        });
  }

private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
};

int main(int argc, char* argv[]) {
	last_time_stamp = time(0);
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_udp_echo_server <port>\n";
      return 1;
    }
    boost::asio::io_service io_service;
    std::thread t(&thread_function);
    server s(io_service, std::atoi(argv[1]));
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
