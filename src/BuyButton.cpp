//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <BuyButton.h>
#include <DBInterface.h>
#include <FlavoursButton.h>
#include <QuantityButton.h>

int BuyButton::m_MaxNos = 6;
int BuyButton::m_NosRow = 2;

/*TgBot::ReplyKeyboardMarkup::Ptr BuyButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	TgBot::ReplyKeyboardMarkup::Ptr pQntyMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	TgBot::KeyboardButton::Ptr kbBuyFlvrs = std::make_shared<TgBot::KeyboardButton>();
	kbBuyFlvrs->text = "flvrSelected Soap : 1";
	row.push_back(kbBuyFlvrs);
	pQntyMenu->keyboard.push_back(row);
	return pQntyMenu;
}*/

TgBot::ReplyKeyboardMarkup::Ptr BuyButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"BuyButton::prepareMenu\" onClick\n"); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr> qntyBtns;
	TgBot::KeyboardButton::Ptr 				qntyBtn;

	std::shared_ptr<QuantityButton> auraBtnQnty	= std::make_shared<QuantityButton>(getDBHandle());
	int iLoop1 = 0, iLoop2, iRun = 0;
	for(iLoop1 = 0; iLoop1 < m_MaxNos; iLoop1++) {
		qntyBtn			= std::make_shared<TgBot::KeyboardButton>();
		qntyBtn->text	= "Buy " + std::to_string(++iRun) + " " + flvrSelected + " Soap";
		qntyBtns.push_back(qntyBtn);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pQntyMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	iRun = 0;
	for(iLoop1 = 0; iLoop1 < (m_MaxNos/m_NosRow); iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_NosRow; iLoop2++) {
			if(iRun < m_MaxNos) row.push_back(qntyBtns[iRun++]);
		}
		pQntyMenu->keyboard.push_back(row);
	}

	pQntyMenu->keyboard.push_back(getMainMenu());
	return pQntyMenu;
}

void BuyButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	std::vector<std::string> flvrNames = getDBHandle()->getFlavours();
	
	std::string pref = "Buy ";
	//	Find the user chosen flavour
	for(auto &flvr : flvrNames) {
		if(flvr == pMsg->text.substr(pref.length(), std::string::npos)) {
			flvrSelected = flvr;
			break;
		}
	}
	fprintf(fp, "AURA: flavor selected %s\n", flvrSelected.c_str()); fflush(fp);

}