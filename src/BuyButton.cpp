//SGN

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <BuyButton.h>
#include <DBInterface.h>
#include <QuantityButton.h>

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

std::string BuyButton::STR_CHOOSE_A_SOAP = "Choose a soap from below list";

TgBot::GenericReply::Ptr BuyButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:BuyButton prepareMenu\n", time(0)); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr>	kbBuyBtns;
	TgBot::KeyboardButton::Ptr 				kbBtnBuy;

	std::shared_ptr<QuantityButton> auraQntyBuy		= std::make_shared<QuantityButton>(getDBHandle());

	std::vector<Soap::Ptr> soapFlvrs = getDBHandle()->getFlavours();
	for(auto &flavour : soapFlvrs) {
		kbBtnBuy		= std::make_shared<TgBot::KeyboardButton>();
		kbBtnBuy->text	= "Buy " + flavour->m_Name + " soap";
		kbBuyBtns.push_back(kbBtnBuy);
		auraQntyBuy->setEvent(kbBtnBuy->text, flavour->m_SoapId);
		listAuraBtns[kbBtnBuy->text]	= auraQntyBuy;
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlavoursMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	int iLoop = 0;
	for(auto &kbBtn : kbBuyBtns) {
		row.push_back(kbBtn);
		iLoop++;
		if(0 == (iLoop%2)) {
			pFlavoursMenu->keyboard.push_back(row);
			row.clear();
		}
	}
	if(row.size()) { row.push_back(getMainMenu()[0]); pFlavoursMenu->keyboard.push_back(row); }
	else { pFlavoursMenu->keyboard.push_back(getMainMenu()); }
	fprintf(fp, "AURA %ld: Finishing BuyButton::prepareMenu::onClick\n", time(0)); fflush(fp);
	return pFlavoursMenu;
}

void BuyButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:\"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
}
