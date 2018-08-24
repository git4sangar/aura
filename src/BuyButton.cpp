//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <BuyButton.h>
#include <DBInterface.h>
#include <FlavoursButton.h>
#include <QuantityButton.h>

int BuyButton::m_Rows = 3;
int BuyButton::m_Cols = 2;

TgBot::ReplyKeyboardMarkup::Ptr BuyButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"BuyButton::prepareMenu\" onClick\n"); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr> qntyBtns;
	TgBot::KeyboardButton::Ptr 				qntyBtn;

	std::shared_ptr<QuantityButton> auraBtnQnty	= std::make_shared<QuantityButton>(getDBHandle());
	int iLoop1 = 0, iLoop2, iRun = 0;
	for(iLoop1 = 0, iRun = 0; iLoop1 < (m_Rows*m_Cols); iLoop1++) {
		qntyBtn			= std::make_shared<TgBot::KeyboardButton>();
		qntyBtn->text	= "Buy " + std::to_string(++iRun) + " " + m_Soap->m_Name + " Soap";
		m_QntyEvents[qntyBtn->text]	= iRun;
		listAuraBtns[qntyBtn->text]	= shared_from_this();
		qntyBtns.push_back(qntyBtn);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pQntyMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	iRun = 0;
	for(iLoop1 = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(qntyBtns[iRun++]);
		}
		pQntyMenu->keyboard.push_back(row);
	}

	pQntyMenu->keyboard.push_back(getMainMenu());
	return pQntyMenu;
}

void BuyButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	std::map<std::string, int>::iterator itBuy = m_BuyEvents.find(pMsg->text);
	if(m_BuyEvents.end() != itBuy) {
		std::vector<Soap::Ptr> flvrs = getDBHandle()->getFlavours();
		int iSoapId 	= m_BuyEvents[pMsg->text];
		for(auto &flvr : flvrs) {
			if(iSoapId == flvr->m_SoapId)
				m_Soap = flvr;
		}
		m_MsgToUser		= "Choose quantity";
		fprintf(fp, "AURA: flavor selected %s\n", m_Soap->m_Name.c_str()); fflush(fp);
	}
	else if(m_QntyEvents.end() != m_QntyEvents.find(pMsg->text)) {
		fprintf(fp, "AURA: Getting Quantity %d\n", m_QntyEvents[pMsg->text]); fflush(fp);
		User::Ptr user = getDBHandle()->getUserForChatId(pMsg->chat->id);
		getDBHandle()->addToCart(m_Soap->m_SoapId, user->m_UserId, m_QntyEvents[pMsg->text]);
		m_MsgToUser		= "Your cart is added with " +
							std::to_string(m_QntyEvents[pMsg->text]) +
							std::string(" ") +
							m_Soap->m_Name +
							" Soap(s). Choose again to change.";
	}
}

void BuyButton::setEvent(std::string clickMsg, int soapId) {
	m_BuyEvents[clickMsg] = soapId;
}