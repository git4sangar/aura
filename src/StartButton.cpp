//sgn

#include <iostream>
#include "AuraButton.h"
#include "StartButton.h"
#include "SoapButton.h"
#include "CartButton.h"
#include "OrdersButton.h"


std::string StartButton::STR_BTN_VIEW_SOAPS		= "View Soaps";
std::string StartButton::STR_BTN_VIEW_CART		= "View Cart";
std::string StartButton::STR_BTN_VIEW_ORDERS	= "View Orders";
std::string StartButton::STR_MSG_DEFF_RELEASE	= "I am the AuraSoap Bot & am getting ready now..!\nBear with me for \"just a few more days\",\nI'll intimate you...";

TgBot::ReplyKeyboardMarkup::Ptr StartButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	TgBot::KeyboardButton::Ptr btnView, btnCart, btnOrder;
	btnView		= std::make_shared<TgBot::KeyboardButton>();
	btnCart		= std::make_shared<TgBot::KeyboardButton>();
	btnOrder	= std::make_shared<TgBot::KeyboardButton>();

	btnView->text	= STR_BTN_VIEW_SOAPS;
	btnCart->text	= STR_BTN_VIEW_CART;
	btnOrder->text	= STR_BTN_VIEW_ORDERS;

	//	Overwrite any previous values at the index
	listAuraBtns[STR_BTN_VIEW_SOAPS]	= std::make_shared<SoapButton>(getDBHandle());
	listAuraBtns[STR_BTN_VIEW_CART]		= std::make_shared<CartButton>(getDBHandle());
	listAuraBtns[STR_BTN_VIEW_ORDERS]	= std::make_shared<OrdersButton>(getDBHandle());

	TgBot::ReplyKeyboardMarkup::Ptr pMainMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row0, row1, row2;
	row0.push_back(btnView);
	row1.push_back(btnCart);
	row2.push_back(btnOrder);

	pMainMenu->keyboard.push_back(row0);
	pMainMenu->keyboard.push_back(row1);
	pMainMenu->keyboard.push_back(row2);
	return pMainMenu;
}

void StartButton::onClick(TgBot::Message::Ptr pMessage, FILE *fp) {
	fprintf(fp, "AURA: StartButton onClick\n"); fflush(fp);
	getDBHandle()->addNewUser(pMessage->chat->id, pMessage->from->firstName, pMessage->from->lastName);
}