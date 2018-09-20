//sgn

#ifndef __OTP_BUTTON__
#define __OTP_BUTTON__

#include <iostream>
#include <AuraButton.h>

#include <DBInterface.h>

class OTPButton : public AuraButton,
					public std::enable_shared_from_this<OTPButton> {
	std::string m_StrOTP;
	unsigned int m_ChatId, m_Amount, m_OTP, m_OrderNo;
public:
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
	OTPButton(DBInterface::Ptr hDB) : AuraButton(hDB), m_ChatId{0}, m_Amount{0}, m_OTP{0}, m_OrderNo{0} {}
	virtual ~OTPButton() {}

	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

	std::string getMsg() { return m_StrOTP;}
	void init(TgBot::Message::Ptr pMessage, FILE *fp);
	std::string getParseMode() {return "HTML";}
	unsigned int getChatIdForNotification(TgBot::Message::Ptr pMessage, FILE *fp);
	std::string getNotifyStrForCustomer();
};


#endif