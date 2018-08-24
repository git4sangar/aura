//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <SoapButton.h>
#include <DBInterface.h>
#include <FlavoursButton.h>
#include <BuyButton.h>


std::string SoapButton::STR_CHOOSE_A_SOAP = "Choose a flavour from below list";

TgBot::ReplyKeyboardMarkup::Ptr SoapButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	std::vector<TgBot::KeyboardButton::Ptr>	kbBtnFlvrs, kbBuyBtns;
	TgBot::KeyboardButton::Ptr 				kbBtnFlvr, kbBtnBuy;

	std::shared_ptr<FlavoursButton> auraBtnFlvr	= std::make_shared<FlavoursButton>(getDBHandle());
	std::shared_ptr<BuyButton> auraBtnBuy		= std::make_shared<BuyButton>(getDBHandle());

	for(auto &flavour : m_SoapFlvrs) {
		kbBtnFlvr 				= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFlvr->text			= "View " + flavour->m_Name + " soap";
		kbBtnFlvrs.push_back(kbBtnFlvr);
		listAuraBtns[kbBtnFlvr->text]	= auraBtnFlvr;

		kbBtnBuy		= std::make_shared<TgBot::KeyboardButton>();
		kbBtnBuy->text	= "Buy " + flavour->m_Name + " soap";
		kbBuyBtns.push_back(kbBtnBuy);
		auraBtnBuy->setEvent(kbBtnBuy->text, flavour->m_SoapId);
		listAuraBtns[kbBtnBuy->text]	= auraBtnBuy;
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlavoursMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	int iLoop = 0;
	for(auto &kbBtn : kbBtnFlvrs) {
		row.push_back(kbBtn);
		row.push_back(kbBuyBtns[iLoop++]);
		pFlavoursMenu->keyboard.push_back(row);
		row.clear();
	}
	pFlavoursMenu->keyboard.push_back(getMainMenu());

	return pFlavoursMenu;
}

void SoapButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	m_SoapFlvrs = getDBHandle()->getFlavours();
}
