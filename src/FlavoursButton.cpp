//sgn

#include <iostream>
#include <FlavoursButton.h>
#include <DBInterface.h>

TgBot::ReplyKeyboardMarkup::Ptr FlavoursButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	return nullptr;
}


void FlavoursButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	std::vector<std::string> flvrNames	= getDBHandle()->getFlavours();
}