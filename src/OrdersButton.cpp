//sgn

#include <iostream>
#include <OrdersButton.h>
#include <DBInterface.h>

std::string OrdersButton::getOrderString(DBInterface::Ptr hDB, POrder::Ptr pOrder) {
	std::stringstream ss;
	int iTotal = 0;

	ss << "Choose an oder to View/Cancel.";
	if(nullptr != pOrder) {
		ss.str(std::string()); ss << "<b>Order No: " << pOrder->m_OrderNo <<
			"</b>\nDate: " << pOrder->m_Date << ", " << pOrder->m_Time <<
			"\nStatus: " << hDB->getStrStatus(pOrder->m_Status) <<
			"\nOTP: " << pOrder->m_OTP <<
			"\nPayment: " << pOrder->m_PayGW <<
			"\n<b>Items Ordered</b>\n";
		std::vector<Cart::Ptr> items = hDB->getCartForOrderNo(pOrder->m_OrderNo);
		iTotal = 0;
		for(auto &item : items) {
			Soap::Ptr soap = hDB->getSoapById(item->m_SoapId);
			ss << soap->m_Name << " - " << item->m_Qnty << " - ₹ " << (soap->m_Price * item->m_Qnty) << "\n";
			iTotal += (soap->m_Price * item->m_Qnty);
		}
		ss << "Total = ₹ " << iTotal << "\n\n\n";
	}
	return ss.str();
}

TgBot::ReplyKeyboardMarkup::Ptr OrdersButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"OrdersButton::prepareMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnVwOdr, kbBtnCnclOdr;
	std::vector<TgBot::KeyboardButton::Ptr> row;
	POrder::Ptr pOrder = nullptr;
	std::stringstream ss;
	int iLoop1 = 0, iRows	= 3, iTotal = 0;

	fprintf(fp, "AURA: View/Cancel Orders, before clearing listAuraBtns size = %ld\n", listAuraBtns.size());
	for(auto& vwOrdr : m_VwOrders) {
		listAuraBtns.erase(vwOrdr.first);
	}
	m_VwOrders.clear();

	for(auto& cnclOrdr : m_CnclOrders) {
		listAuraBtns.erase(cnclOrdr.first);
	}
	m_CnclOrders.clear();
	fprintf(fp, "AURA: View/Cancel Orders, after clearing listAuraBtns size = %ld\n", listAuraBtns.size());

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

	pOrdrMenu->keyboard.push_back(getMainMenu());
	return pOrdrMenu;
}

void OrdersButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: OrdersButton onClick\n"); fflush(fp);

	std::map<std::string, POrder::Ptr>::iterator itr;
	m_StrOrder = "Choose an oder to View/Cancel.";
	bool isViewCancelSet = false;

	//	Check if this menu is becoz of clicking existing order
	if(m_VwOrders.end() != (itr = m_VwOrders.find(pMsg->text))) {
		isViewCancelSet = true;
		m_StrOrder = getOrderString(getDBHandle(), itr->second);
	}

	//	Cancel the order and clear the cart items
	else if(m_CnclOrders.end() != (itr = m_CnclOrders.find(pMsg->text))) {
		isViewCancelSet = true;
		m_StrOrder = std::string("<b>Cancelled</b>\n") + getOrderString(getDBHandle(), itr->second);
		getDBHandle()->cancelPOrder(itr->second->m_OrderNo);
	}

	//	Populate all the orders to render the menu
	//	m_Orders contain most recent orders after cancel operation if any
	m_Orders	= getDBHandle()->getPOrdersForUser(pMsg->chat->id);

	//	Let's take the most recent order if view/cancel is not chosen
	if( !isViewCancelSet && (0 < m_Orders.size()) )  {
		m_StrOrder = getOrderString(getDBHandle(), m_Orders[0]);
	}
}
