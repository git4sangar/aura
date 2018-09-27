//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <SoapButton.h>
#include <DBInterface.h>
#include <BuyButton.h>

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

std::string SoapButton::STR_CHOOSE_A_SOAP = "Choose a soap from below list";

TgBot::GenericReply::Ptr SoapButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:SoapButton prepareMenu\n", time(0)); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr>	kbBtnFlvrs, kbBuyBtns;
	TgBot::KeyboardButton::Ptr 				kbBtnFlvr, kbBtnBuy;

	std::shared_ptr<BuyButton> auraBtnBuy		= std::make_shared<BuyButton>(getDBHandle());

	std::vector<Soap::Ptr> soapFlvrs = getDBHandle()->getFlavours();
	for(auto &flavour : soapFlvrs) {
		kbBtnFlvr 				= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFlvr->text			= "View " + flavour->m_Name + " soap";
		kbBtnFlvrs.push_back(kbBtnFlvr);
		m_Soaps[kbBtnFlvr->text]		= flavour;
		listAuraBtns[kbBtnFlvr->text]	= shared_from_this();

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
	fprintf(fp, "AURA %ld: Finishing SoapButton::prepareMenu::onClick\n", time(0)); fflush(fp);
	return pFlavoursMenu;
}

TgBot::InputFile::Ptr SoapButton::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:\"%s\" getMedia\n", time(0), pMsg->text.c_str()); fflush(fp);
	std::map<std::string, Soap::Ptr>::iterator itr;
	TgBot::InputFile::Ptr pFile = nullptr;
	if(m_Soaps.end() != (itr = m_Soaps.find(pMsg->text))) {
		Soap::Ptr pSoap	= itr->second;
		std::string filePath = std::string(MAKE_STR(ASSET_PATH)) +
								std::string("/") + pSoap->m_PicFile;
		pFile = TgBot::InputFile::fromFile(filePath, "image/jpeg");
	}
	return pFile;
}

void SoapButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:\"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
}
