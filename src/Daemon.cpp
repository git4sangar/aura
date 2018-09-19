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

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

#define SKIP_INTERVAL   (5)

#define AURA_BOT_TOKEN     "uPwIhbFxqA6avQhimCCNuHM9UohLrjB2voJXupoIngq y5ixTRdBGZL3oIMC"
#define THRAYA_BOT_TOKEN   "uPAHhbp2qklEvQhiniGSu3DGA PdqbZRtHiDDspfuEagpnurxx1MGWqbBIm8"

std::string decode_string(std::string enc_msg, std::string key) {
   return aura_decrypt(enc_msg, key);
}

void AuraMainLoop(FILE *fp) {
   fprintf(fp, "AURA: Starting AuraMainLoop\n"); fflush(fp);
   std::shared_ptr<TgBot::Bot> pBot          = std::make_shared<TgBot::Bot>(decode_string(AURA_BOT_TOKEN, MAKE_STR(DECRYPT_KEY)));
   DBInterface::Ptr hDB       = std::make_shared<DBInterface>(std::string(MAKE_STR(AURA_DB_FILE)), fp);
   std::map<std::string, std::shared_ptr<AuraButton>> auraButtons;
   time_t startSec = time(NULL);

   std::shared_ptr<StartButton> btnStart  = std::make_shared<StartButton>(hDB);
   auraButtons["start"]                   = btnStart;
   auraButtons["Main Menu"]               = btnStart;

   std::shared_ptr<OTPButton> btnOtp    = std::make_shared<OTPButton>(hDB);
   auraButtons["/otp"]                    = btnOtp;

   pBot->getEvents().onAnyMessage( [pBot, &auraButtons, fp, &startSec](TgBot::Message::Ptr pMsg) {
      static bool isSkipOver = false;
      std::shared_ptr<AuraButton> pAuraBtn = nullptr;
      std::string strCmd = pMsg->text;

      // Skip everything for a few secs
      if(!isSkipOver && (startSec + SKIP_INTERVAL) > time(NULL)) {
         fprintf(fp, "AURA: Skipping %s\n", pMsg->text.c_str()); fflush(fp);
         return;
      }
      isSkipOver = true;

      fprintf(fp, "AURA: Received \"%s\" onAnyMessage\n",  pMsg->text.c_str()); fflush(fp);
      fprintf(fp, "AURA: AuraBtnList Size %ld \n",  auraButtons.size()); fflush(fp);

      // Split the command string delimited by spaces
      if(std::string::npos != pMsg->text.find("/otp")) {
         std::istringstream iss(pMsg->text);
         std::vector<std::string> words(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
         strCmd = words[0];
      }

      if(pMsg->contact) {
         strCmd = ShippingAddress::STR_BTN_CONTACT;
      }

      std::map<std::string, std::shared_ptr<AuraButton>>::const_iterator itr;

      itr = auraButtons.find(strCmd);
      if(auraButtons.end() != itr) {
         TgBot::ReplyKeyboardMarkup::Ptr pMenu;
         pAuraBtn = itr->second->getSharedPtr();
         fprintf(fp, "AURA: Found \"%s\" button\n", pMsg->text.c_str()); fflush(fp);

         // Initialize local data
         pAuraBtn->init(pMsg, fp);

         // On Click
         pAuraBtn->onClick(pMsg, fp);

         // On notifications
         std::string strNotify = pAuraBtn->getNotifyStr(pMsg->chat->id);
         if(!strNotify.empty()) {
            std::vector<unsigned int> chatIds   = pAuraBtn->getNotifyUsers();
            for(auto &chatId : chatIds) {
               pBot->getApi().sendMessage(chatId, strNotify, false, 0, nullptr, pAuraBtn->getParseMode());
            }
         }

         // For user notifications
         std::string strUserNotif = pAuraBtn->getNotifyStrForCustomer();
         if(!strUserNotif.empty()) {
            unsigned int chatId = pAuraBtn->getChatIdForNotification(pMsg, fp);
            pBot->getApi().sendMessage(chatId, strUserNotif);
         }

         // Send Snaps if any
         TgBot::InputFile::Ptr pFile = pAuraBtn->getMedia(pMsg, fp);
         if(pFile)   pBot->getApi().sendPhoto(pMsg->chat->id, pFile);

         // Send the Keyboard
         pMenu = pAuraBtn->prepareMenu(auraButtons, fp);
         pBot->getApi().sendMessage(pMsg->chat->id, pAuraBtn->getMsg(), false, 0, pMenu, pAuraBtn->getParseMode());
      } else {
         fprintf(fp, "AURA: \"%s\" button missing\n", pMsg->text.c_str()); fflush(fp);
         itr = auraButtons.find("start");
         pAuraBtn = itr->second->getSharedPtr();
         pAuraBtn->onClick(pMsg, fp);
         pBot->getApi().sendMessage(pMsg->chat->id,
                        "Hi " + pMsg->from->firstName + ",\n" + pAuraBtn->getMsg(),
                        false, 0, pAuraBtn->prepareMenu(auraButtons, fp));
      }
   });

   pBot->getEvents().onCommand("start", [pBot, &auraButtons, fp, &startSec](TgBot::Message::Ptr pMsg) {
      static bool isSkipOver = false;
      // Skip everything for a few secs
      if(!isSkipOver && (startSec + SKIP_INTERVAL) > time(NULL)) {
         fprintf(fp, "AURA: Skipping %s\n", pMsg->text.c_str()); fflush(fp);
         return;
      }
      isSkipOver = true;

      fprintf(fp, "AURA: Received start command\n"); fflush(fp);
      std::map<std::string, std::shared_ptr<AuraButton>>::const_iterator itr;

      itr = auraButtons.find("start");

      if(auraButtons.end() != itr) {
         fprintf(fp, "AURA: Found start button\n"); fflush(fp);
         itr->second->onClick(pMsg, fp);
         pBot->getApi().sendMessage(pMsg->chat->id,
                        "Hi " + pMsg->from->firstName + ",\n" + itr->second->getMsg(),
                        false, 0, itr->second->prepareMenu(auraButtons, fp));
      } else {
         fprintf(fp, "AURA: Start button missing\n"); fflush(fp);
      }
   });

   std::string strMsg = "AURA: Bot username " + pBot->getApi().getMe()->username;
   fprintf(fp, "AURA: Bot username %s\n", pBot->getApi().getMe()->username.c_str()); fflush(fp);

   TgBot::TgLongPoll longPoll(*pBot);
   while (true) {
      try {
         pBot->getApi().deleteWebhook();
         fprintf(fp, "AURA: Long poll started\n"); fflush(fp);
         longPoll.start();
      } catch (std::exception& e) {
         fprintf(fp, "AURA: An exception occured at longPoll %s\n", e.what()); fflush(fp);
      }
   }
}


int main(void) {
   FILE *fp = fopen(MAKE_STR(AURA_LOG_FILE), "w");
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

   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   umask(0);

   // Open system logs for the child process
   //openlog("daemon-named", LOG_NOWAIT | LOG_PID, LOG_USER);
   //syslog(LOG_NOTICE, "AURA: Successfully started daemon-name");
   fprintf(fp, "AURA: Successfully started Bot daemon\n"); fflush(fp);

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
   close(STDERR_FILENO);

   AuraMainLoop(fp);

   // Close system logs for the child process
   //syslog(LOG_NOTICE, "AURA: Stopping daemon-name");
   fprintf(fp, "AURA: Stopping Bot daemon\n"); fflush(fp);
   fclose(fp);
   closelog();

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);
}
