//SGN

#include <iostream>
#include <ReminderButton.h>
#include <DBInterface.h>
#include <sstream>

TgBot::ReplyKeyboardMarkup::Ptr ReminderButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listKBBtns, FILE *fp) {
	//	Dont allow access if unauthorized
	if(!m_IsAuthorized) m_StrReminder = "Unauthorized access";
	else m_StrReminder = "Notification sent to user.";

	fprintf(fp, "AURA %ld: Finishing ReminderButton::prepareMenu\n", time(0)); fflush(fp);
	return nullptr;
}

std::vector<unsigned int> ReminderButton::getChatIdsForNotification(TgBot::Message::Ptr pMessage, FILE *fp) {
	std::vector<unsigned int> chatIds;

	//	Dont allow access if unauthorized
	if(!m_IsAuthorized) return chatIds;

	m_Orders = getDBHandle()->getPendingOrders();
	for(auto& order : m_Orders) {
		chatIds.push_back(order->m_ChatId);
	}
	return chatIds;
}

std::string ReminderButton::getNotifyStrForCustomer(unsigned int chatId) {
	std::stringstream ss;
	std::vector<POrder::Ptr>::iterator itr;

	//	Dont allow access if unauthorized
	if(!m_IsAuthorized) return ss.str();

	int iTotal = 0;
	for(itr = m_Orders.begin(); itr != m_Orders.end(); iTotal = 0) {
		if( (*itr)->m_ChatId == chatId ) {
			User::Ptr user = getDBHandle()->getUserForChatId(chatId);
			ss << "\n\nHi " << user->m_FName << ", a polite reminder to make payment against your order number: "
				<< (*itr)->m_OrderNo << " via " << (*itr)->m_PayGW;
			std::vector<Cart::Ptr> items = getDBHandle()->getCartForOrderNo((*itr)->m_OrderNo);
			ss << "\n<b>Cart</b>\n";
			for(auto &item : items) {
				Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
				ss << std::setw(15) << soap->m_Name << " - "
					<< std::setw(2) << item->m_Qnty << " - "
					<< std::setw(3) << "₹ "
					<< std::setw(4)<< (soap->m_Price * item->m_Qnty) << "\n";
				iTotal += (soap->m_Price * item->m_Qnty);
			}
			ss << std::setw(20) << "Total = ₹ " << iTotal << "\n";
			itr = m_Orders.erase(itr);
		} else { itr++; }
	}
	return ss.str();
}

void ReminderButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld: ReminderButton::onClick\n", time(0));

	std::vector<unsigned int> chatIds	= getDBHandle()->getNotifyUsers();
	if(chatIds.end() != std::find(chatIds.begin(), chatIds.end(), pMsg->chat->id)) {
		m_IsAuthorized = true;
	} else {
		fprintf(fp, "AURA %ld: ReminderButton unauthorized access\n", time(0));
	}
}
