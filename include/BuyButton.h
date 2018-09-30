//sgn

#ifndef __BUY_BUTTON__
#define __BUY_BUTTON__

#include <iostream>
#include <vector>

#include <DBInterface.h>
#include <AuraButton.h>
#include <map>


class BuyButton :
	public AuraButton,
	public std::enable_shared_from_this<BuyButton> {
	static std::string STR_CHOOSE_A_SOAP;
	std::map<std::string, Soap::Ptr> m_Soaps;

public:
	BuyButton(DBInterface::Ptr hDB) : AuraButton(hDB) {}
	virtual ~BuyButton() {}

	std::string getMsg() { return STR_CHOOSE_A_SOAP;}
	TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, TgBot::Message::Ptr pMsg, FILE *fp);

	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}
};


#endif