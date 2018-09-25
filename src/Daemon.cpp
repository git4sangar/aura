//sgn

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
#include <tgbot/tgbot.h>
#include "vigenere/encrypt.h"
#include "AuraButton.h"
#include "StartButton.h"
#include <map>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ShippingAddress.h>
#include <OTPButton.h>
#include <ReminderButton.h>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

#define SKIP_INTERVAL   (5)
#define MYPORT (4950)

#define AURA_BOT_TOKEN     "uPwIhbFxqA6avQhimCCNuHM9UohLrjB2voJXupoIngq y5ixTRdBGZL3oIMC"
#define THRAYA_BOT_TOKEN   "uPAHhbp2qklEvQhiniGSu3DGA PdqbZRtHiDDspfuEagpnurxx1MGWqbBIm8"

std::string decode_string(std::string enc_msg, std::string key) {
   return aura_decrypt(enc_msg, key);
}

void petWatchDog(FILE *fp) {
   int sockfd = 0;
   struct hostent *he;
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   struct sockaddr_in their_addr;
   std::stringstream ss;
   ss << time(0);

   he=gethostbyname("localhost");
   their_addr.sin_family = AF_INET;      /* host byte order */
   their_addr.sin_port = htons(MYPORT);  /* short, network byte order */
   their_addr.sin_addr = *((struct in_addr *)he->h_addr);
   bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */
   sendto(sockfd, ss.str().c_str(), ss.str().size(), 0, 
             (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
   close(sockfd);
   fprintf(fp, "AURA %ld: Petting WatchDog\n", time(0)); fflush(fp);
}

void AuraMainLoop(FILE *fp) {
   fprintf(fp, "AURA %ld: Starting AuraMainLoop\n", time(0)); fflush(fp);
   std::shared_ptr<TgBot::Bot> pBot          = std::make_shared<TgBot::Bot>(decode_string(AURA_BOT_TOKEN, MAKE_STR(DECRYPT_KEY)));
   DBInterface::Ptr hDB       = std::make_shared<DBInterface>(std::string(MAKE_STR(AURA_DB_FILE)), fp);
   std::map<std::string, std::shared_ptr<AuraButton>> auraButtons;
   time_t startSec = time(NULL);

   std::shared_ptr<StartButton> btnStart  = std::make_shared<StartButton>(hDB);
   auraButtons["/start"]                  = btnStart;
   auraButtons["start"]                   = btnStart;
   auraButtons["Main Menu"]               = btnStart;

   auraButtons["/otp"]     = std::make_shared<OTPButton>(hDB);
   auraButtons["/remind"]  = std::make_shared<ReminderButton>(hDB);

   pBot->getEvents().onAnyMessage( [pBot, &auraButtons, fp, &startSec](TgBot::Message::Ptr pMsg) {
      fprintf(fp, "AURA %ld: Received \"%s\" onAnyMessage as it arrived\n", time(0),  pMsg->text.c_str()); fflush(fp);
      static bool isSkipOver = false;
      std::shared_ptr<AuraButton> pAuraBtn = nullptr;
      std::string strCmd = pMsg->text;

      // Skip everything for a few secs
      if(!isSkipOver && (startSec + SKIP_INTERVAL) > time(NULL)) {
         fprintf(fp, "AURA %ld: Skipping %s\n", time(0), pMsg->text.c_str()); fflush(fp);
         return;
      }
      isSkipOver = true;

      // When /otp command arrives, it cotains order_no as param
      // As the order_no varies everytime it cannot be checked against auraButtons.
      // Split the command string delimited by spaces so that we can check.
      if(std::string::npos != pMsg->text.find("/otp")) {
         std::istringstream iss(pMsg->text);
         std::vector<std::string> words(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
         strCmd = words[0];
      }

      if(pMsg->contact) {
         // pMsg->text will be empty in this case. So assign to it too.
         pMsg->text  = strCmd = ShippingAddress::STR_BTN_CONTACT;
      }

      fprintf(fp, "AURA %ld: Received \"%s\" onAnyMessage after processing\n", time(0),  strCmd.c_str()); fflush(fp);
      fprintf(fp, "AURA %ld: AuraBtnList Size %ld \n", time(0),  auraButtons.size()); fflush(fp);

      std::map<std::string, std::shared_ptr<AuraButton>>::const_iterator itr;
      if(auraButtons.end() != (itr = auraButtons.find(strCmd)) ||
         auraButtons.end() != (itr = auraButtons.find(std::to_string(pMsg->chat->id)))) {
         TgBot::GenericReply::Ptr pMenu;
         pAuraBtn = itr->second->getSharedPtr();
         fprintf(fp, "AURA %ld: Found \"%s\" button\n", time(0), pMsg->text.c_str()); fflush(fp);

         // Initialize local data
         pAuraBtn->init(pMsg, fp);

         // On Click
         pAuraBtn->onClick(pMsg, fp);

         // Notification to moderators
         std::string strNotify = pAuraBtn->getNotifyStr(pMsg->chat->id);
         if(!strNotify.empty()) {
            std::vector<unsigned int> chatIds   = pAuraBtn->getNotifyUsers();
            for(auto &chatId : chatIds) {
               pBot->getApi().sendMessage(chatId, strNotify, false, 0, nullptr, pAuraBtn->getParseMode());
            }
         }

         // For end-user notifications
         std::vector<unsigned int> chatIds = pAuraBtn->getChatIdsForNotification(pMsg, fp);
         for(auto& chatId : chatIds) {
            std::string strUserNotif = pAuraBtn->getNotifyStrForCustomer(chatId);
            if(!strUserNotif.empty()) pBot->getApi().sendMessage(chatId, strUserNotif, false, 0, pMenu, pAuraBtn->getParseMode());
         }

         // Send Snaps if any
         TgBot::InputFile::Ptr pFile = pAuraBtn->getMedia(pMsg, fp);
         if(pFile)   pBot->getApi().sendPhoto(pMsg->chat->id, pFile);

         // Send the Keyboard
         pMenu = pAuraBtn->prepareMenu(auraButtons, fp);
         pBot->getApi().sendMessage(pMsg->chat->id, pAuraBtn->getMsg(), false, 0, pMenu, pAuraBtn->getParseMode());
      } else {
         fprintf(fp, "AURA %ld: \"%s\" button missing\n", time(0), pMsg->text.c_str()); fflush(fp);
         std::stringstream ss;
         ss << "Hi " << pMsg->from->firstName
              << ", could not recognize what you say, pls use the buttons below.\nRegret the inconvenience caused.";
         pBot->getApi().sendMessage(pMsg->chat->id, ss.str(),
                        false, 0, auraButtons["Main Menu"]->prepareMenu(auraButtons, fp));
      }
   });

   fprintf(fp, "AURA %ld: Bot username %s\n", time(0), pBot->getApi().getMe()->username.c_str()); fflush(fp);

   TgBot::TgLongPoll longPoll(*pBot);
   while (true) {
      try {
         petWatchDog(fp);
         pBot->getApi().deleteWebhook();
         fprintf(fp, "AURA %ld: Long poll started\n", time(0)); fflush(fp);
         longPoll.start();
      } catch (std::exception& e) {
         fprintf(fp, "AURA %ld: An exception occured at longPoll %s\n", time(0), e.what()); fflush(fp);
      }
   }
}


int main(void) {
   std::string aura_log_path = MAKE_STR(AURA_LOG_PATH);
   std::string aura_log_file = MAKE_STR(AURA_LOG_FILE);
   std::string aura_log_with_path  = aura_log_path + std::string("/") + aura_log_file;
   FILE *fp = fopen(aura_log_with_path.c_str(), "w");
   // Define variables
   pid_t pid, sid;

   fprintf(fp, "AURA: Very start\n"); fflush(fp);
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

   fprintf(fp, "AURA: Successfully started Bot daemon\n"); fflush(fp);
   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   /*umask(0);

   // Open system logs for the child process
   //openlog("daemon-named", LOG_NOWAIT | LOG_PID, LOG_USER);
   //syslog(LOG_NOTICE, "AURA: Successfully started daemon-name");

   // Generate a session ID for the child process
   sid = setsid();
   // Ensure a valid SID for the child process
   if(sid < 0) {
      // Log failure and exit
      //syslog(LOG_ERR, "AURA: Could not generate session ID for child process");
      fprintf(fp, "AURA: Could not generate session ID for child process\n"); fflush(fp);

      // If a new session ID could not be generated, we must terminate the child process
      // or it will be orphaned
      exit(EXIT_FAILURE);
   }

   // Change the current working directory to a directory guaranteed to exist
   if((chdir("/")) < 0) {
      // Log failure and exit
      //syslog(LOG_ERR, "AURA: Could not change working directory to /");
      fprintf(fp, "AURA: Could not change working directory to\n"); fflush(fp);

      // If our guaranteed directory does not exist, terminate the child process to ensure
      // the daemon has not been hijacked
      exit(EXIT_FAILURE);
   }

   // A daemon cannot use the terminal, so close standard file descriptors for security reasons
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);*/

   AuraMainLoop(fp);

   // Close system logs for the child process
   //syslog(LOG_NOTICE, "AURA: Stopping daemon-name");
   fprintf(fp, "AURA: Stopping Bot daemon\n"); fflush(fp);
   fclose(fp);
   closelog();

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);
}
