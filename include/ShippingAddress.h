//SGN

#ifndef __SHIPPING_ADDRESS_H__
#define __SHIPPING_ADDRESS_H__

#include <iostream>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>

#include <AuraButton.h>
#include <DBInterface.h>

#define MAX_NAVINS_FLOORS (4)
enum class MenuRenderer {APARTMENT, BLOCK, BLOCK_NO, FLOOR, FLAT_NO, CONTACT, CONFIRM, ADDRESS, DONE, NOTA};

enum class UserContext {ADDRESS, REVIEW, NOTA};

class ShippingAddress :
		public AuraButton,
		public std::enable_shared_from_this<ShippingAddress> {
	std::string m_StrMsg, m_Cache, m_NotifyStr;
	int m_Rows, m_Cols, m_FloorNo;
	int m_FlatsRendered, m_FloorsRendered, m_BlockNosRendered, m_BlocksRendered;
	unsigned int m_ChatId;
	std::vector<std::string> m_Apts;
	std::vector<std::string> m_Blocks;
	std::map<std::string, std::tuple<std::string,int>> m_BlockNos;
	std::map<std::string, std::tuple<std::string,int>> m_Floors;
	std::map<std::string, std::tuple<std::string,int>> m_Flats;
	std::map<std::string, unsigned int> m_PreDfnd;
	std::map<unsigned int, UserContext> m_Context;
	std::tuple<std::string,unsigned int> m_Addr;
	MenuRenderer m_RenderMenu;

	TgBot::GenericReply::Ptr renderAptMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::GenericReply::Ptr renderBlockMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::GenericReply::Ptr renderBlockNoMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::GenericReply::Ptr renderFloorMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::GenericReply::Ptr renderFlatMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, std::string aptName, FILE *fp);
	TgBot::GenericReply::Ptr shareContactMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::GenericReply::Ptr renderCheckoutMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	std::string floorNoToString(int iFloorNo);
	std::vector<TgBot::KeyboardButton::Ptr> getLastRow(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns,
														std::vector<TgBot::KeyboardButton::Ptr>&& lastRow);
	void clearAuraButtons(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, MenuRenderer item, FILE *fp);
	std::string getPaymentString(unsigned int chatId, std::string fname);
public:
	typedef std::shared_ptr<ShippingAddress> Ptr;

	ShippingAddress(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) {
		m_Rows = m_Cols = m_FloorNo = m_ChatId = 0;
		m_FlatsRendered = m_FloorsRendered = m_BlockNosRendered = m_BlocksRendered = 0;
	}
	virtual ~ShippingAddress() {}

	void init(TgBot::Message::Ptr pMsg, FILE *fp);
	std::string getStr4Admins(unsigned int userParam);
	std::string prepareNotifyStr(unsigned int chatId);
	std::string getMsg() { return m_StrMsg;}
	std::string getParseMode() {return "HTML";}
	TgBot::GenericReply::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);
	std::shared_ptr<AuraButton> getSharedPtr() {return shared_from_this();}

	static std::string STR_BTN_SSM;
	static std::string STR_BTN_BRKFLD;
	static std::string STR_BTN_GARUDA;
	static std::string STR_BTN_PURVA;
	static std::string STR_BTN_CONTACT;
	static std::string STR_BTN_BACK;
	static std::string STR_BTN_PAYTM;
	static std::string STR_BTN_TEZ;
	static std::string STR_BTN_ON_DELIVERY;
};



#endif