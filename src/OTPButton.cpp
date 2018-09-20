//sgn

#include <iostream>
#include <OTPButton.h>
#include <DBInterface.h>
#include <sstream>

void OTPButton::init(TgBot::Message::Ptr pMessage, FILE *fp) {
	m_ChatId = m_Amount = m_OTP = m_OrderNo = 0;
}

unsigned int OTPButton::getChatIdForNotification(TgBot::Message::Ptr pMessage, FILE *fp) {
	return getDBHandle()->getChatIdForOrderNo(m_OrderNo);;
}

std::string OTPButton::getNotifyStrForCustomer() {
	std::stringstream ss;
	ss << "Payment ₹ " << m_Amount << " received successfully against Order No: " 
		<< m_OrderNo << ".\n"
		<< "Your OTP is " << m_OTP <<".\nPls provide it during delivery.\nThanks for the purchase.";
	return ss.str();
}

TgBot::ReplyKeyboardMarkup::Ptr OTPButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	m_StrOTP = "Notification sent to user.";
	return nullptr;
}

void OTPButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: OTPButton onClick\n"); fflush(fp);

	std::vector<unsigned int> chatIds	= getDBHandle()->getNotifyUsers();
	if(chatIds.end() == std::find(chatIds.begin(), chatIds.end(), pMsg->chat->id)) {
		m_StrOTP = "Authorization denied";
		return;
	}

	// Split the command string delimited by spaces
	std::istringstream iss(pMsg->text);
	std::vector<std::string> words(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

	if(1 < words.size()) {
		m_OrderNo	= std::stoul(words[1]);
		getDBHandle()->updatePOrderStatus(m_OrderNo, CartStatus::READY_FOR_DELIVERY);

		m_OTP = 1234;
		getDBHandle()->updateOTP(m_OrderNo, m_OTP);
	}
	if(2 < words.size()) {
		m_Amount	= std::stoul(words[2]);
	}
}
