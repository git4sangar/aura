//sgn

#ifndef __BUY_BUTTON__
#define __BUY_BUTTON__

#include <iostream>
#include <tuple>

#include <AuraButton.h>
#include <DBInterface.h>

class BuyButton :
		public AuraButton,
		public std::enable_shared_from_this<BuyButton> {

	//	The flavour chosen by User
	Soap::Ptr m_Soap;

	//	Rows and Columns to render the buttons
	static int m_Rows, m_Cols;

	//	Button name and Id of the chosen soap in SQLite
	std::map<std::string, int> m_BuyEvents;

	//	Button name and quantity of recently-chosen-soap by User
	std::map<std::string, std::tuple<unsigned int, unsigned int>> m_QntyEvents;

	std::string m_MsgToUser;

	static std::string STR_BTN_OTHER_FLAVOURS;

public:
	BuyButton(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) { m_MsgToUser = "Choose quantity";}
	virtual ~BuyButton() {}

	std::string getMsg() { return m_MsgToUser;}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

	void setEvent(std::string clickMsg, int soapId);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
};


#endif