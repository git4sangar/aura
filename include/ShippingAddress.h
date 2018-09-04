//SGN

#ifndef __SHIPPING_ADDRESS_H__
#define __SHIPPING_ADDRESS_H__

#include <iostream>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>

#include <AuraButton.h>
#include <DBInterface.h>

enum class MenuRenderer {APARTMENT, BLOCK, BLOCK_NO, FLOOR, FLAT_NO, CONTACT, DONE};

class ShippingAddress :
		public AuraButton,
		public std::enable_shared_from_this<ShippingAddress> {
	std::string m_StrMsg, m_Cache;
	int m_Rows, m_Cols, m_FloorNo;
	std::vector<std::string> m_Apts;
	std::vector<std::string> m_Blocks;
	std::map<std::string, std::tuple<std::string,int>> m_BlockNos;
	std::map<std::string, std::tuple<std::string,int>> m_Floors;
	std::map<std::string, std::tuple<std::string,int>> m_Flats;
	std::map<std::string, unsigned int> m_PreDfnd;
	std::tuple<std::string,unsigned int> m_Addr;
	MenuRenderer m_RenderMenu;

	TgBot::ReplyKeyboardMarkup::Ptr renderAptMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr renderBlockMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr renderBlockNoMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr renderFloorMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr renderFlatMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	TgBot::ReplyKeyboardMarkup::Ptr shareContactMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	std::string floorNoToString(int iFloorNo);
	std::vector<TgBot::KeyboardButton::Ptr> getLastRow(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns,
														std::vector<TgBot::KeyboardButton::Ptr>&& lastRow);
public:
	typedef std::shared_ptr<ShippingAddress> Ptr;
	ShippingAddress(std::shared_ptr<DBInterface> hDB) : AuraButton(hDB) {m_Rows = m_Cols = m_FloorNo = 0;}
	virtual ~ShippingAddress() {}

	std::string getMsg() { return m_StrMsg;}
	TgBot::ReplyKeyboardMarkup::Ptr prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp);
	void onClick(TgBot::Message::Ptr pMessage, FILE *fp);

	static std::string STR_BTN_SSM;
	static std::string STR_BTN_BRKFLD;
	static std::string STR_BTN_GARUDA;
	static std::string STR_BTN_CONTACT;
	static std::string STR_BTN_BACK;
};



#endif