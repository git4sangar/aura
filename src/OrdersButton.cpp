//sgn

#include <iostream>
#include <OrdersButton.h>
#include <DBInterface.h>

TgBot::ReplyKeyboardMarkup::Ptr OrdersButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"OrdersButton::prepareMenu\" rendering\n"); fflush(fp);
	std::stringstream ss;
	int iCount = 3, iTotal = 0;
	for(auto &order : m_Orders) {
		if(0 >= iCount) break;
		ss << "<b>Order No: " << order->m_OrderNo <<
			"</b>\nDate: " << order->m_Date << ", " << order->m_Time <<
			"\nStatus: " << getDBHandle()->getStrStatus(order->m_Status) <<
			"\nOTP: " << order->m_OTP <<
			"\nPayment: " << order->m_PayGW <<
			"\n<b>Items Ordered</b>\n";
		std::vector<Cart::Ptr> items = getDBHandle()->getCartForOrderNo(order->m_OrderNo);
		iTotal = 0;
		for(auto &item : items) {
			Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
			ss << soap->m_Name << " - " << item->m_Qnty << " - ₹ " << (soap->m_Price * item->m_Qnty) << "\n";
			iTotal += (soap->m_Price * item->m_Qnty);
		}
		ss << "Total = ₹ " << iTotal << "\n\n\n";
		iCount--;
	}
	m_StrOrder	= ss.str();

	TgBot::ReplyKeyboardMarkup::Ptr pMainMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	pMainMenu->keyboard.push_back(getMainMenu());
	return pMainMenu;
}

void OrdersButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: OrdersButton onClick\n"); fflush(fp);
	m_Orders	= getDBHandle()->getPOrdersForUser(pMsg->chat->id);
}