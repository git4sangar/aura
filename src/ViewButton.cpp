//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <ViewButton.h>
#include <DBInterface.h>
#include <QuantityButton.h>

// Stringify param x.
// Step (01): Replaces the pattern MAKE_STR(x) with MAKE_STR(value-of-x)
#define MAKE_STR(x)  _MAKE_STR(x)
// Step (02): Replaces the pattern _MAKE_STR(value-of-x) with quotes-added-param, ie "value-of-x"
#define _MAKE_STR(x) #x          // Adds quotes to the param

std::string ViewButton::STR_CHOOSE_A_SOAP = "Choose a soap from below list";

TgBot::GenericReply::Ptr ViewButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:ViewButton prepareMenu\n", time(0)); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr>	kbBtnFlvrs;
	TgBot::KeyboardButton::Ptr 				kbBtnFlvr;

	std::shared_ptr<QuantityButton> auraQntyBuy		= std::make_shared<QuantityButton>(getDBHandle());

	std::vector<Soap::Ptr> soapFlvrs = getDBHandle()->getFlavours();
	for(auto &flavour : soapFlvrs) {
		kbBtnFlvr 				= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFlvr->text			= "View " + flavour->m_Name + " soap";
		kbBtnFlvrs.push_back(kbBtnFlvr);
		m_Soaps[kbBtnFlvr->text]		= flavour;
		listAuraBtns[kbBtnFlvr->text]	= shared_from_this();
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlavoursMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	int iLoop = 0;
	for(auto &kbBtn : kbBtnFlvrs) {
		row.push_back(kbBtn);
		iLoop++;
		if(0 == (iLoop%2)) {
			pFlavoursMenu->keyboard.push_back(row);
			row.clear();
		}
	}
	if(row.size()) { row.push_back(getMainMenu()[0]); pFlavoursMenu->keyboard.push_back(row); }
	else { pFlavoursMenu->keyboard.push_back(getMainMenu()); }
	fprintf(fp, "AURA %ld: Finishing ViewButton::prepareMenu::onClick\n", time(0)); fflush(fp);
	return pFlavoursMenu;
}

TgBot::InputFile::Ptr ViewButton::getMedia(TgBot::Message::Ptr pMsg, FILE *fp) {
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

void ViewButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld:\"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
}
