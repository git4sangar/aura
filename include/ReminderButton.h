//sgn

#ifndef __REMINDER_BUTTON__
#define __REMINDER_BUTTON__

#include <iostream>
#include <AuraButton.h>

#include <DBInterface.h>

class ReminderButton : public AuraButton,
					public std::enable_shared_from_this<ReminderButton> {
	std::string m_StrReminder;
	std::vector<POrder::Ptr> m_Orders;
	bool m_IsAuthorized;

public:
	void init(TgBot::Message::Ptr pMsg, FILE *fp) { m_IsAuthorized = false; }
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
	ReminderButton(DBInterface::Ptr hDB) : AuraButton(hDB), m_IsAuthorized{false} {}
	virtual ~ReminderButton() {}

	TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

	std::string getMsg() { return m_StrReminder;}
	std::string getParseMode() {return "HTML";}
	std::vector<unsigned int> getChatIdsForNotification(TgBot::Message::Ptr pMessage, FILE *fp);
	std::string getStrForUser(TgBot::Message::Ptr pMsg);
};


#endif