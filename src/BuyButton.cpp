//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <StartButton.h>
#include <ShippingAddress.h>

#include <ViewCart.h>
#include <BuyButton.h>
#include <DBInterface.h>

std::string BuyButton::STR_BTN_OTHER_FLAVOURS = "Other Soaps";

int BuyButton::m_Rows = 3;
int BuyButton::m_Cols = 2;

TgBot::GenericReply::Ptr BuyButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"BuyButton::prepareMenu\" onClick\n", time(0)); fflush(fp);
	std::vector<TgBot::KeyboardButton::Ptr> qntyBtns;
	TgBot::KeyboardButton::Ptr 				qntyBtn;

	int iLoop1 = 0, iLoop2, iRun = 0;
	for(iLoop1 = 0, iRun = 0; iLoop1 < (m_Rows*m_Cols); iLoop1++) {
		qntyBtn			= std::make_shared<TgBot::KeyboardButton>();
		qntyBtn->text	= "Buy " + std::to_string(++iRun) + " " + m_Soap->m_Name + " Soap";
		m_QntyEvents[qntyBtn->text]	= std::make_tuple(m_Soap->m_SoapId, iRun);
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

	TgBot::KeyboardButton::Ptr kbBtnVwSoap, kbBtnVwCart, kbBtnChkOut;
	row.clear();
	kbBtnVwSoap 		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnVwSoap->text	= STR_BTN_OTHER_FLAVOURS;
	listAuraBtns[kbBtnVwSoap->text] = listAuraBtns[StartButton::STR_BTN_VIEW_SOAPS];
	row.push_back(kbBtnVwSoap);
	kbBtnVwCart 		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnVwCart->text	= StartButton::STR_BTN_VIEW_CART;
	row.push_back(kbBtnVwCart);
	kbBtnChkOut 		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnChkOut->text	= ViewCart::STR_BTN_PURCHASE;
	if(listAuraBtns.end() == listAuraBtns.find(ViewCart::STR_BTN_PURCHASE)) {
		listAuraBtns[ViewCart::STR_BTN_PURCHASE]	= std::make_shared<ShippingAddress>(getDBHandle());
	}
	row.push_back(kbBtnChkOut);
	pQntyMenu->keyboard.push_back(row);
	fprintf(fp, "AURA %ld: Finishing prepareMenu\n", time(0)); fflush(fp);
	return pQntyMenu;
}

void BuyButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld: \"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
	std::map<std::string, int>::iterator itBuy = m_BuyEvents.find(pMsg->text);
	if(m_BuyEvents.end() != itBuy) {
		std::vector<Soap::Ptr> flvrs = getDBHandle()->getFlavours();
		int iSoapId 	= m_BuyEvents[pMsg->text];
		for(auto &flvr : flvrs) {
			if(iSoapId == flvr->m_SoapId)
				m_Soap = flvr;
		}
		m_MsgToUser		= "Choose quantity";
		fprintf(fp, "AURA %ld: flavour selected %s\n", time(0), m_Soap->m_Name.c_str()); fflush(fp);
	}
	else if(m_QntyEvents.end() != m_QntyEvents.find(pMsg->text)) {
		fprintf(fp, "AURA %ld: Getting Quantity %d\n", time(0), std::get<1>(m_QntyEvents[pMsg->text])); fflush(fp);
		std::vector<Soap::Ptr> flvrs = getDBHandle()->getFlavours();
		int iSoapId 	= std::get<0>(m_QntyEvents[pMsg->text]);
		for(auto &flvr : flvrs) {
			if(iSoapId == flvr->m_SoapId)
				m_Soap = flvr;
		}

		getDBHandle()->addToCart(m_Soap->m_SoapId, pMsg->chat->id, std::get<1>(m_QntyEvents[pMsg->text]));
		m_MsgToUser		= "Your cart is added with " +
							std::to_string(std::get<1>(m_QntyEvents[pMsg->text])) +
							std::string(" ") +
							m_Soap->m_Name +
							" Soap(s). To change quantity, choose again.";
	}
}

void BuyButton::setEvent(std::string clickMsg, int soapId) {
	m_BuyEvents[clickMsg] = soapId;
}