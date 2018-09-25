//sgn

#include <iostream>
#include <OTPButton.h>
#include <DBInterface.h>
#include <sstream>

void OTPButton::init(TgBot::Message::Ptr pMessage, FILE *fp) {
	m_ChatId = m_Amount = m_OTP = m_OrderNo = 0;
	m_IsAuthorized = false;
}

TgBot::ReplyKeyboardMarkup::Ptr OTPButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	if(!m_IsAuthorized) m_StrOTP = "Unauthorized access";
	else m_StrOTP = "Notification sent to user.";

	fprintf(fp, "AURA %ld: Finishing prepareMenu\n", time(0)); fflush(fp);
	return nullptr;
}

std::vector<unsigned int> OTPButton::getChatIdsForNotification(TgBot::Message::Ptr pMessage, FILE *fp) {
	std::vector<unsigned int> chatIds;

	//	Dont allow access if unauthorized
	if(!m_IsAuthorized) return chatIds;

	chatIds.push_back(getDBHandle()->getChatIdForOrderNo(m_OrderNo));
	return chatIds;
}

std::string OTPButton::getNotifyStrForCustomer(unsigned int chatId) {
	std::stringstream ss;

	//	Dont allow access if unauthorized
	if(!m_IsAuthorized) return ss.str();

	ss << "Payment ₹ " << m_Amount << " received successfully against Order No: " 
		<< m_OrderNo << ".\n"
		<< "Your OTP is " << m_OTP <<".\nPls provide it during delivery.\nThanks for the purchase.";
	return ss.str();
}

void OTPButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA %ld: OTPButton onClick\n", time(0)); fflush(fp);

	std::vector<unsigned int> chatIds	= getDBHandle()->getNotifyUsers();
	if(chatIds.end() != std::find(chatIds.begin(), chatIds.end(), pMsg->chat->id)) {
		m_IsAuthorized = true;
		// Split the command string delimited by spaces
		std::istringstream iss(pMsg->text);
		std::vector<std::string> words(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

		if(1 < words.size()) {
			m_OrderNo	= std::stoul(words[1]);
			getDBHandle()->updatePOrderStatus(m_OrderNo, CartStatus::READY_FOR_DELIVERY);

			srand(time(0)); m_OTP = rand() % 9999;
			// Check if 4 digits
			while(999 > m_OTP) {
				srand(time(0)); m_OTP = rand() % 9999;
				usleep(100);
			}
			getDBHandle()->updateOTP(m_OrderNo, m_OTP);
		}
		if(2 < words.size()) {
			m_Amount	= std::stoul(words[2]);
		}
	}
}
