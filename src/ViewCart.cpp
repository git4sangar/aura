//SGN

#include <ViewCart.h>
#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>
#include <ShippingAddress.h>
#include <StartButton.h>

std::string ViewCart::STR_BTN_EMPTY_CART	= "Empty Cart";
std::string ViewCart::STR_BTN_PURCHASE		= "Purchase";

TgBot::ReplyKeyboardMarkup::Ptr ViewCart::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ViewCart::prepareMenu\" onClick\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnEmpty, kbBtnChkOut;
	kbBtnEmpty		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnChkOut		= std::make_shared<TgBot::KeyboardButton>();

	kbBtnEmpty->text	= STR_BTN_EMPTY_CART;
	kbBtnChkOut->text	= STR_BTN_PURCHASE;

	listAuraBtns[STR_BTN_EMPTY_CART]	= shared_from_this();
	listAuraBtns[STR_BTN_PURCHASE]		= std::make_shared<ShippingAddress>(getDBHandle());

	TgBot::ReplyKeyboardMarkup::Ptr pViewCartMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row0, row1;
	row0.push_back(kbBtnChkOut);
	row1.push_back(kbBtnEmpty);

	pViewCartMenu->keyboard.push_back(row0);
	pViewCartMenu->keyboard.push_back(row1);

	pViewCartMenu->keyboard.push_back(getMainMenu());
	return pViewCartMenu;
}

void ViewCart::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	if(StartButton::STR_BTN_VIEW_CART == pMsg->text) {
		fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
		std::vector<Cart::Ptr> items = getDBHandle()->getUserCart(pMsg->chat->id);
		std::stringstream ss;
		int iTotal = 0;
		for(auto &item : items) {
			Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
			ss << soap->m_Name << "\t: " << item->m_Qnty << " : Rs." << (soap->m_Price * item->m_Qnty) << "\n";
			iTotal += (soap->m_Price * item->m_Qnty);
		}
		if(0 == items.size()) {
			ss << "Your cart is empty";
		} else {
			ss << "Total = " << iTotal << "\n";
		}
		m_StrCart = ss.str();
	} else if(STR_BTN_EMPTY_CART == pMsg->text) {
		fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
		getDBHandle()->emptyCartForUser(pMsg->chat->id);
		m_StrCart = "Your Cart is empty";
	}
}