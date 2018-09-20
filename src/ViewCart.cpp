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
	fprintf(fp, "AURA %ld: \"ViewCart::prepareMenu\" onClick\n", time(0)); fflush(fp);
	TgBot::ReplyKeyboardMarkup::Ptr pViewCartMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	if(!m_IsCartEmpty) {
		TgBot::KeyboardButton::Ptr kbBtnEmpty, kbBtnChkOut;
		kbBtnEmpty		= std::make_shared<TgBot::KeyboardButton>();
		kbBtnChkOut		= std::make_shared<TgBot::KeyboardButton>();

		kbBtnEmpty->text	= STR_BTN_EMPTY_CART;
		kbBtnChkOut->text	= STR_BTN_PURCHASE;

		listAuraBtns[STR_BTN_EMPTY_CART]	= shared_from_this();
		listAuraBtns[STR_BTN_PURCHASE]		= std::make_shared<ShippingAddress>(getDBHandle());

		std::vector<TgBot::KeyboardButton::Ptr> row0, row1;
		row0.push_back(kbBtnChkOut);
		row1.push_back(kbBtnEmpty);

		pViewCartMenu->keyboard.push_back(row0);
		pViewCartMenu->keyboard.push_back(row1);
		pViewCartMenu->keyboard.push_back(getMainMenu());
	} else {
		pViewCartMenu = listAuraBtns[StartButton::STR_BTN_VIEW_SOAPS]->prepareMenu(listAuraBtns, fp);
	}

	fprintf(fp, "AURA %ld: Finishing ViewCart::prepareMenu::onClick\n", time(0)); fflush(fp);
	return pViewCartMenu;
}

void ViewCart::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	if(StartButton::STR_BTN_VIEW_CART == pMsg->text) {
		fprintf(fp, "AURA %ld: \"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
		std::vector<Cart::Ptr> items = getDBHandle()->getUserCart(pMsg->chat->id);
		std::stringstream ss;
		int iTotal = 0;
		for(auto &item : items) {
			Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
			ss << std::setw(15) << soap->m_Name << " - "
				<< std::setw(2) << item->m_Qnty << " - "
				<< std::setw(7) << "₹ " << (soap->m_Price * item->m_Qnty) << "\n";
			iTotal += (soap->m_Price * item->m_Qnty);
		}
		if(0 == items.size()) {
			ss << "Your cart is empty! Pls buy Soap to get that into Cart.";
		} else {
			ss << std::setw(18) << "Total = ₹ " << iTotal << "\n";
		}
		m_StrCart = ss.str();
		m_IsCartEmpty = (0 == items.size());
	} else if(STR_BTN_EMPTY_CART == pMsg->text) {
		fprintf(fp, "AURA %ld: \"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
		m_IsCartEmpty = getDBHandle()->emptyCartForUser(pMsg->chat->id);
		m_StrCart = "Your Cart is empty";
	}
}