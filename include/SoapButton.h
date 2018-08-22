//sgn

#ifndef __SOAP_BUTTON__
#define __SOAP_BUTTON__

#include <iostream>
#include <AuraButton.h>
#include <vector>

class SoapButton : public AuraButton {
	std::vector<std::string> soapNames;
	static std::string STR_CHOOSE_A_SOAP;
public:
	SoapButton(std::shared_ptr<SQLite::Database> hDB) : AuraButton(hDB) {}
	virtual ~SoapButton() {}

	std::string getMsg() { return STR_CHOOSE_A_SOAP;}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
};


#endif