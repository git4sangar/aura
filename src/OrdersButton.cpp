//sgn

#include <iostream>
#include <OrdersButton.h>
#include <DBInterface.h>

/*
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
*/

TgBot::ReplyKeyboardMarkup::Ptr OrdersButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"OrdersButton::prepareMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnVwOdr, kbBtnCnclOdr;
	std::vector<TgBot::KeyboardButton::Ptr> row;
	POrder::Ptr pOrder = nullptr;
	std::stringstream ss;
	int iLoop1 = 0, iRows	= 3, iTotal = 0;

	TgBot::ReplyKeyboardMarkup::Ptr pOrdrMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	for(iLoop1 = 0; (iLoop1 < iRows) && (iLoop1 < m_Orders.size()); iLoop1++) {
		row.clear();
		pOrder 	= m_Orders[iLoop1];

		ss.str(std::string()); ss << "View Order No: " << pOrder->m_OrderNo;
		kbBtnVwOdr			= std::make_shared<TgBot::KeyboardButton>();
		kbBtnVwOdr->text	= ss.str();
		listAuraBtns[kbBtnVwOdr->text]	= shared_from_this();
		m_VwOrders[kbBtnVwOdr->text]= pOrder;
		row.push_back(kbBtnVwOdr);

		ss.str(std::string()); ss << "Cancel Order No: " << pOrder->m_OrderNo;
		kbBtnCnclOdr		= std::make_shared<TgBot::KeyboardButton>();
		kbBtnCnclOdr->text	= ss.str();
		listAuraBtns[kbBtnCnclOdr->text]	= shared_from_this();
		m_CnclOrders[kbBtnCnclOdr->text]	= pOrder;
		row.push_back(kbBtnCnclOdr);
		pOrdrMenu->keyboard.push_back(row);
	}

	if(nullptr != m_Order) {
		ss.str(std::string()); ss << "<b>Order No: " << m_Order->m_OrderNo <<
			"</b>\nDate: " << m_Order->m_Date << ", " << m_Order->m_Time <<
			"\nStatus: " << getDBHandle()->getStrStatus(m_Order->m_Status) <<
			"\nOTP: " << m_Order->m_OTP <<
			"\nPayment: " << m_Order->m_PayGW <<
			"\n<b>Items Ordered</b>\n";
		std::vector<Cart::Ptr> items = getDBHandle()->getCartForOrderNo(m_Order->m_OrderNo);
		iTotal = 0;
		for(auto &item : items) {
			Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
			ss << soap->m_Name << " - " << item->m_Qnty << " - ₹ " << (soap->m_Price * item->m_Qnty) << "\n";
			iTotal += (soap->m_Price * item->m_Qnty);
		}
		ss << "Total = ₹ " << iTotal << "\n\n\n";
	} else {ss << "No Orders to display.";}

	m_StrOrder	= ss.str();
	pOrdrMenu->keyboard.push_back(getMainMenu());
	return pOrdrMenu;
}

void OrdersButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: OrdersButton onClick\n"); fflush(fp);
	std::map<std::string, POrder::Ptr>::iterator itr;

	//	Check if this menu is becoz of clicking existing order
	if(m_VwOrders.end() != (itr = m_VwOrders.find(pMsg->text))) {
		m_Order = itr->second;
	}

	//	Cancel the order and clear the cart items
	else if(m_CnclOrders.end() != (itr = m_CnclOrders.find(pMsg->text))) {
		getDBHandle()->cancelPOrder(itr->second->m_OrderNo);
	}

	m_Orders	= getDBHandle()->getPOrdersForUser(pMsg->chat->id);

	//	Let's take the most recent order to render
	if( (nullptr == m_Order) && (0 < m_Orders.size()) ) {
		m_Order	= m_Orders[0];
	}
}
