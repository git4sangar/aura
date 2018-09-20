//sgn
#include <sys/types.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

#define MAX_TIMEOUT_INTERVAL_SECS (120)
#define MAX_WAKEUP_INTERVAL_SECS (60)
#define LISTEN_PORT (4950)

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

unsigned int last_time_stamp = 0;

void thread_function(FILE *fp) {
  std::string aura_bin_path = MAKE_STR(AURA_BIN_PATH);
  std::string aura_bin_file = MAKE_STR(AURA_BIN_FILE);
  std::string aura_bin_with_path  = aura_bin_path + std::string("/") + aura_bin_file;

  //  The command is "pidof -s SoapSeller"
  std::string pid_of_cmd  = std::string("pidof -s ") + aura_bin_file;

  while(true) {
    fprintf(fp,  "WatchDogServer %ld: Last time stamp: %d, Cur Time: %ld, Delta: %ld\n",
                time(0), last_time_stamp, time(0), (time(0) - last_time_stamp)); fflush(fp);
    if(MAX_TIMEOUT_INTERVAL_SECS <= (time(0) - last_time_stamp)) {
      fprintf(fp, "WatchDogServer %ld: time elapsed, so killing %s\n", time(0), aura_bin_file.c_str());
  		char buf[512];
  		FILE *cmd_pipe = popen(pid_of_cmd.c_str(), "r");
  		fgets(buf, 512, cmd_pipe);
  		pid_t pid = strtoul(buf, NULL, 10);
  		pclose( cmd_pipe );
  		if(0 < pid) {
        fprintf(fp, "WatchDogServer %ld: killing process pid %d\n", time(0), pid); fflush(fp);
  			kill(pid, 9);
  		} else {
        fprintf(fp, "WatchDogServer %ld: Invalid pid %d. Seems %s is not running\n",
                    time(0), pid, aura_bin_file.c_str()); fflush(fp);
  		}
      fprintf(fp, "WatchDogServer %ld: Starting SoapSeller %s after 3 secs\n", time(0), aura_bin_with_path.c_str()); fflush(fp);
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
  server(boost::asio::io_service& io_service, short port, FILE *fp_)
    : socket_(io_service, udp::endpoint(udp::v4(), port)), fp(fp_) {
    do_receive();
  }

  void do_receive() {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
          if (!ec && bytes_recvd > 0) {
            data_[bytes_recvd] = '\0';
            last_time_stamp = std::stoul(std::string(data_));
            fprintf(fp, "WatchDogServer %ld: WatchDog petted at: %d\n", time(0), last_time_stamp); fflush(fp);
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
  FILE *fp;
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
};

int wdog_server(FILE *fp) {
	last_time_stamp = time(0);
  try {
    boost::asio::io_service io_service;
    std::thread t(&thread_function, fp);
    server s(io_service, LISTEN_PORT, fp);
    io_service.run();
  } catch (std::exception& e) {
    fprintf(fp, "WatchDogServer %ld: An exception occured: %s\n", time(0), e.what()); fflush(fp);
  }

  return 0;
}

int main(void) {
  std::string wdog_log_path = MAKE_STR(WATCHDOG_LOG_PATH);
  std::string wdog_log_file = MAKE_STR(WATCHDOG_LOG_FILE);
  std::string wdog_log_with_path  = wdog_log_path + std::string("/") + wdog_log_file;

  FILE *fp = fopen(wdog_log_with_path.c_str(), "w");

   // Define variables
   pid_t pid, sid;

   fprintf(fp, "WatchDogServer: Very start\n"); fflush(fp);
   // Fork the current process
   pid = fork();

   // The parent process continues with a process ID greater than 0
   if(pid > 0) {
      std::cout << "Exiting pid " << getpid() << std::endl;
      exit(EXIT_SUCCESS);
   }
   // A process ID lower than 0 indicates a failure in either process
   else if(pid < 0) {
      exit(EXIT_FAILURE);
   }
   // The parent process has now terminated, and the forked child process will continue
   // (the pid of the child process was 0)

   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   umask(0);

   // Open system logs for the child process
   fprintf(fp, "WatchDogServer: Successfully started\n"); fflush(fp);

   // Generate a session ID for the child process
   sid = setsid();
   // Ensure a valid SID for the child process
   if(sid < 0) {
      // Log failure and exit
      fprintf(fp, "WatchDogServer: Could not generate session ID for child process\n"); fflush(fp);

      // If a new session ID could not be generated, we must terminate the child process
      // or it will be orphaned
      exit(EXIT_FAILURE);
   }

   // Change the current working directory to a directory guaranteed to exist
   if((chdir("/")) < 0) {
      // Log failure and exit
      fprintf(fp, "WatchDogServer: Could not change working directory to\n"); fflush(fp);

      // If our guaranteed directory does not exist, terminate the child process to ensure
      // the daemon has not been hijacked
      exit(EXIT_FAILURE);
   }

   // A daemon cannot use the terminal, so close standard file descriptors for security reasons
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);

   wdog_server(fp);

   // Close system logs for the child process
   fprintf(fp, "WatchDogServer: Stopping...\n"); fflush(fp);
   fclose(fp);
   closelog();

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);
}