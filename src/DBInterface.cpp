//sgn

#include <iostream>
#include <DBInterface.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>
#include <sstream>

	std::string Soap::DB_TABLE_SOAP_COLUMN_ID	= "soap_id";
	std::string Soap::DB_TABLE_SOAP_COLUMN_NAME	= "name";
	std::string Soap::DB_TABLE_SOAP_COLUMN_WEIGHT	= "weight";
	std::string Soap::DB_TABLE_SOAP_COLUMN_DESC	= "description";
	std::string Soap::DB_TABLE_SOAP_COLUMN_PIC_ID	= "pic_id";
	std::string Soap::DB_TABLE_SOAP_COLUMN_STOCK	= "stock";
	std::string Soap::DB_TABLE_SOAP_COLUMN_PRICE	= "price";
	std::string Soap::DB_TABLE_SOAP_COLUMN_PIC_FILE	= "pic_file";

	std::string User::DB_TABLE_USER_COLUMN_ID 		= "user_id";
	std::string User::DB_TABLE_USER_COLUMN_FNAME		= "first_name";
	std::string User::DB_TABLE_USER_COLUMN_LNAME		= "last_name";
	std::string User::DB_TABLE_USER_COLUMN_ADDRRESS		= "address";
	std::string User::DB_TABLE_USER_COLUMN_CHAT_ID		= "chat_id";
	std::string User::DB_TABLE_USER_COLUMN_MOBILE		= "mobile";
	std::string User::DB_TABLE_USER_COLUMN_ORDER_NO		= "order_no";

	std::string Cart::DB_TABLE_CART_COLUMN_CART_ID		= "cart_id";
	std::string Cart::DB_TABLE_CART_COLUMN_SOAP_ID		= "soap_id";
	std::string Cart::DB_TABLE_CART_COLUMN_CHAT_ID		= "chat_id";
	std::string Cart::DB_TABLE_CART_COLUMN_STATUS		= "status";
	std::string Cart::DB_TABLE_CART_COLUMN_QNTY		= "quantity";
	std::string Cart::DB_TABLE_CART_COLUMN_ORDER_NO		= "order_no";

	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_SHIP_ID	= "ship_id";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_CHAT_ID	= "chat_id";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME	= "apt_name";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO	= "block_no";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO	= "flat_no";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO	= "order_no";

	std::string POrder::DB_TABLE_PORDER_COLUMN_ORDER_ID	= "order_id";
	std::string POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO	= "order_no";
	std::string POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID	= "chat_id";
	std::string POrder::DB_TABLE_PORDER_COLUMN_DATE	= "date";
	std::string POrder::DB_TABLE_PORDER_COLUMN_TIME	= "time";
	std::string POrder::DB_TABLE_PORDER_COLUMN_DATE_TIME	= "date_time";
	std::string POrder::DB_TABLE_PORDER_COLUMN_PAY_GW	= "payment_gw";
	std::string POrder::DB_TABLE_PORDER_COLUMN_OTP = "otp";
	std::string POrder::DB_TABLE_PORDER_COLUMN_STATUS	= "status";

	std::string DBInterface::DB_TABLE_NOTIFY_COLUMN_CHAT_ID = "chat_id";

DBInterface::DBInterface(std::string dbFileName, FILE *fp) {
	m_hDB   = std::make_shared<SQLite::Database>(dbFileName, SQLite::OPEN_READWRITE);
	m_Fp	= fp;
}

DBInterface::~DBInterface() {}

void DBInterface::deletePOrder(unsigned int chatId) {
	fprintf(m_Fp, "AURA: deletePOrder: %d\n", chatId); fflush(m_Fp);
	unsigned int order_no	= getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "DELETE from POrder WHERE " << POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
}

void DBInterface::cancelPOrder(unsigned int order_no) {
	fprintf(m_Fp, "AURA: cancelPOrder: %d\n", order_no); fflush(m_Fp);
	SQLite::Transaction transaction(*m_hDB);
	std::stringstream ss;

	ss << "DELETE from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	m_hDB->exec(ss.str());

	ss.str(std::string());
	ss << "DELETE from POrder WHERE " << POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	m_hDB->exec(ss.str());

	transaction.commit();
}

void DBInterface::updateOTP(unsigned int order_no, int otp) {
	fprintf(m_Fp, "AURA: updateOTP: %d for order_no %d\n", otp, order_no); fflush(m_Fp);
	SQLite::Transaction transaction(*m_hDB);
	std::stringstream ss;

	ss << "UPDATE POrder set "
		<< POrder::DB_TABLE_PORDER_COLUMN_OTP << " = " << otp
		<< " WHERE " << POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	m_hDB->exec(ss.str());

	transaction.commit();
}

void DBInterface::updatePOrderStatus(unsigned int order_no, CartStatus status) {
	fprintf(m_Fp, "AURA: updatePOrderStatus order_no: %d, status: %d\n", order_no, getIntStatus(status)); fflush(m_Fp);
	SQLite::Transaction transaction(*m_hDB);
	std::stringstream ss;

	ss << "UPDATE POrder set "
		<< POrder::DB_TABLE_PORDER_COLUMN_STATUS << " = " << getIntStatus(status)
		<< " WHERE " << POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	m_hDB->exec(ss.str());

	transaction.commit();
}

void DBInterface::updatePOrderPayGW(unsigned int chatId, std::string payGw) {
	fprintf(m_Fp, "AURA: updatePOrderPayGW: %d\n", chatId); fflush(m_Fp);
	unsigned int order_no	= getOrderNoForUser(chatId);
	std::stringstream ss;
	if(std::string("Cash") == payGw) {
		ss << "UPDATE POrder set " << POrder::DB_TABLE_PORDER_COLUMN_PAY_GW << " = \"" << payGw << "\", "
			<< POrder::DB_TABLE_PORDER_COLUMN_STATUS << " = " << getIntStatus(CartStatus::READY_FOR_DELIVERY) << " WHERE "
			<< POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	} else {
		ss << "UPDATE POrder set " << POrder::DB_TABLE_PORDER_COLUMN_PAY_GW << " = \"" << payGw << "\" WHERE "
			<< POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	}
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
}

std::vector<POrder::Ptr> DBInterface::getPOrdersForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getPOrdersForUser: %d\n", chatId); fflush(m_Fp);
	POrder::Ptr pOrder;
	std::vector<POrder::Ptr> pOrders;
	std::stringstream ss;

	ss << "SELECT * from POrder WHERE " << POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID << " = " << chatId <<
		" ORDER BY " << POrder::DB_TABLE_PORDER_COLUMN_DATE_TIME << " DESC;";
	SQLite::Statement query(*m_hDB, ss.str());
	while(query.executeStep()) {
		pOrder = std::make_shared<POrder>();
		pOrder->m_OrderId	= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_ORDER_ID.c_str()).getUInt();
		pOrder->m_OrderNo	= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO.c_str()).getUInt();
		pOrder->m_ChatId	= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID.c_str()).getUInt();
		pOrder->m_OTP		= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_OTP.c_str()).getUInt();
		pOrder->m_Date		= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_DATE.c_str()).getString();
		pOrder->m_Time		= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_TIME.c_str()).getString();
		pOrder->m_DateTime	= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_DATE_TIME.c_str()).getUInt();
		pOrder->m_PayGW		= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_PAY_GW.c_str()).getString();
		int iStat		= query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_STATUS.c_str()).getUInt();
		pOrder->m_Status	= getCartStatus(iStat);
		pOrders.push_back(pOrder);
	}
	return pOrders;
}

void DBInterface::createPOrder(unsigned int chatId) {
	fprintf(m_Fp, "AURA: createOrder: %d\n", chatId); fflush(m_Fp);
	unsigned int order_no	= getOrderNoForUser(chatId);
	std::stringstream ss;

	time_t tnow = time(NULL);
	struct tm ltm = *localtime(&tnow);
	//printf("now: %d-%d-%d %d:%d:%d\n", ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

	ss.str(std::string());
	ss << std::setfill('0') << std::setw(2) << ltm.tm_mday << "-" <<
		std::setfill('0') << std::setw(2) << ltm.tm_mon + 1 << "-" <<
		std::setfill('0') << std::setw(4) << ltm.tm_year + 1900;
	std::string strDate = ss.str();

	ss.str(std::string());
	ss << std::setfill('0') << std::setw(2) << ltm.tm_hour << ":" <<
			std::setfill('0') << std::setw(2) << ltm.tm_min << ":" <<
			std::setfill('0') << std::setw(2) << ltm.tm_sec;
	std::string strTime = ss.str();

	ss.str(std::string());
	ss << "INSERT INTO POrder (" <<
		POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_DATE << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_TIME << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_PAY_GW << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_DATE_TIME << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_OTP << ", " <<
		POrder::DB_TABLE_PORDER_COLUMN_STATUS << ") VALUES (" <<
		order_no << ", " << chatId << ", \"" << strDate << "\", \"" <<
		strTime << "\", \"Unknown\"," << tnow << ", " << 0 << ", "
		<< getIntStatus(CartStatus::PENDING) << ");";

	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
}

bool DBInterface::updateShippingFromPrevOrder(unsigned int chatId, unsigned int order_no) {
	fprintf(m_Fp, "AURA: repeatShipping: %d\n", chatId); fflush(m_Fp);
	unsigned int newOrderNo	= getOrderNoForUser(chatId);

	//	No need to update database
	if(newOrderNo == order_no) {
		fprintf(m_Fp, "AURA: Chosen address already exists for order_no %d\n", order_no); fflush(m_Fp);
		return false;
	}

	std::stringstream ss;
	ss << "INSERT INTO Shipping (" <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_CHAT_ID << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO << ")" <<
		" SELECT " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_CHAT_ID << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO << ", " <<
			newOrderNo << ", " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO << " FROM Shipping WHERE " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
	return true;
}

std::tuple<std::string, unsigned int> DBInterface::getShippingForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getShippingForUser: %d\n", chatId); fflush(m_Fp);
	std::stringstream ss;
	std::string strAddr;
	std::tuple<std::string, unsigned int> tplShip;
	unsigned int order_no = 0;

	ss << "SELECT * from Shipping WHERE " << Shipping::DB_TABLE_SHIPPING_COLUMN_CHAT_ID << " = " << chatId << 
		" ORDER BY " << Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " DESC;";
	SQLite::Statement query(*m_hDB, ss.str());
	if(query.executeStep()) {
		order_no = query.getColumn(Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO.c_str()).getUInt();
		strAddr = query.getColumn(Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO.c_str()).getString() + "-";
		strAddr += std::to_string(query.getColumn(Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO.c_str()).getUInt()) + ", ";
		strAddr += query.getColumn(Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME.c_str()).getString();
		tplShip = std::make_tuple(strAddr, order_no);
	}
	return tplShip;
}

void DBInterface::updateMobileNo(unsigned int chatId, std::string strMobileNo) {
	fprintf(m_Fp, "AURA: updateMobileNo: %s\n", strMobileNo.c_str()); fflush(m_Fp);
	unsigned long long mobileNo;
	SQLite::Transaction transaction(*m_hDB);

	mobileNo = std::stoull(strMobileNo);

	std::stringstream ss;
	ss << "UPDATE User set " << User::DB_TABLE_USER_COLUMN_MOBILE << " = " << mobileNo <<
		" WHERE " << User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " << chatId << ";";
	m_hDB->exec(ss.str());
	transaction.commit();
}

Soap::Ptr DBInterface::getSoapById(unsigned int soapId) {
	fprintf(m_Fp, "AURA: getSoapById: %d\n", soapId); fflush(m_Fp);
	Soap::Ptr soap = nullptr;
	std::stringstream ss;
	ss << "SELECT * from Soap WHERE soap_id = " << soapId << ";";
	SQLite::Statement   query(*m_hDB, ss.str());
	if(query.executeStep()) {
		soap = std::make_shared<Soap>();
		soap->m_SoapId		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_ID.c_str()).getUInt();
		soap->m_Weight		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_WEIGHT.c_str()).getUInt();
		soap->m_Price		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PRICE.c_str()).getUInt();
		soap->m_PicId		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PIC_ID.c_str()).getUInt();
		soap->m_Stock		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_STOCK.c_str()).getUInt();
		soap->m_Name		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_NAME.c_str()).getString();
		soap->m_Desc		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_DESC.c_str()).getString();
		soap->m_PicFile		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PIC_FILE.c_str()).getString();
	}
	return soap;
}

CartStatus DBInterface::getCartStatus(int iStat) {
	CartStatus stat = CartStatus::NOTA;
	switch(iStat) {
		case 1:
			stat = CartStatus::PENDING;
		break;
		case 2:
			stat = CartStatus::READY_FOR_DELIVERY;
		break;
		case 3:
			stat = CartStatus::DELIVERED;
		break;
		case 0:
		default:
			stat = CartStatus::NOTA;
		break;
	} 
	return stat;
}

std::string DBInterface::getStrStatus(CartStatus stat) {
	std::string srStat;
	switch(stat) {
		case CartStatus::PENDING:
			srStat = "Pending Payment";
			break;
		case CartStatus::READY_FOR_DELIVERY:
			srStat = "Payment Received, Delivery Pending";
			break;
		case CartStatus::DELIVERED:
			srStat = "Delivered";
			break;
		case CartStatus::NOTA:
		default:
			srStat = "Unknown";
	}
	return srStat;
}

int DBInterface::getIntStatus(CartStatus stat) {
	int iStat;
	switch(stat) {
		case CartStatus::PENDING:
			iStat = 1;
			break;
		case CartStatus::READY_FOR_DELIVERY:
			iStat = 2;
			break;
		case CartStatus::DELIVERED:
			iStat = 3;
			break;
		case CartStatus::NOTA:
		default:
			iStat = 0;
	}
	return iStat;
}

void DBInterface::addBlockNoToShipping(unsigned int chatId, std::string blkNo) {
	fprintf(m_Fp, "AURA: addBlockNoToShipping blkNo: %s\n", blkNo.c_str()); fflush(m_Fp);
	int order_no = getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "SELECT * from Shipping WHERE " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement   query(*m_hDB, ss.str());
	if(query.executeStep()) {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "UPDATE Shipping set " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO << " = \"" << blkNo << "\" WHERE "<<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
		m_hDB->exec(ss.str());
		transaction.commit();
	} else {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "INSERT INTO Shipping (" <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO << ", " <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO <<  ") VALUES (\"" <<
				blkNo << "\", " << order_no << ");";
		m_hDB->exec(ss.str());
		transaction.commit();
	}
}

void DBInterface::addFlatNoToShipping(unsigned int chatId, unsigned int flatNo) {
	fprintf(m_Fp, "AURA: addFlatNoToShipping flatNo: %d\n", flatNo); fflush(m_Fp);
	int order_no = getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "SELECT * from Shipping WHERE " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement   query(*m_hDB, ss.str());
	if(query.executeStep()) {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "UPDATE Shipping set " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO << " = " << flatNo << " WHERE "<<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
		m_hDB->exec(ss.str());
		transaction.commit();
	} else {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "INSERT INTO Shipping (" <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO << ", " <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO <<  ") VALUES (" <<
				flatNo << ", " << order_no << ");";
		m_hDB->exec(ss.str());
		transaction.commit();
	}
}

void DBInterface::addAptNameToShipping(unsigned int chatId, std::string aptName) {
	fprintf(m_Fp, "AURA: addAptNameToShipping aptName: %s\n", aptName.c_str()); fflush(m_Fp);
	int order_no = getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "SELECT * from Shipping WHERE " <<
			Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement   query(*m_hDB, ss.str());
	if(query.executeStep()) {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "UPDATE Shipping set " <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME << " = \"" << aptName << "\" WHERE "<<
				Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO << " = " << order_no << ";";
		m_hDB->exec(ss.str());
		transaction.commit();
	} else {
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << " INSERT INTO Shipping (" << Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME << ", " <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_CHAT_ID << ", " <<
				Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO <<  ") VALUES (\"" <<
				aptName << "\", " << chatId << ", " << order_no << ");";
		m_hDB->exec(ss.str());
		transaction.commit();
	}
}

std::vector<unsigned int> DBInterface::getNotifyUsers() {
	fprintf(m_Fp, "AURA: getNotifyUsers\n"); fflush(m_Fp);
	std::vector<unsigned int> chatIds;
	unsigned int chatId;
	std::stringstream ss;
	ss << "SELECT * from Notify;";
	SQLite::Statement query(*m_hDB, ss.str());
	while(query.executeStep()) {
		chatId = query.getColumn(DB_TABLE_NOTIFY_COLUMN_CHAT_ID.c_str()).getUInt();
		chatIds.push_back(chatId);
	}
	return chatIds;
}

std::vector<Soap::Ptr> DBInterface::getFlavours() {
	fprintf(m_Fp, "AURA: getFlavours\n"); fflush(m_Fp);
	std::vector<Soap::Ptr> soaps;
	Soap::Ptr soap;

	SQLite::Statement   query(*m_hDB, "SELECT * FROM Soap;");
	while(query.executeStep()) {
		soap = std::make_shared<Soap>();
		soap->m_SoapId		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_ID.c_str()).getUInt();
		soap->m_Weight		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_WEIGHT.c_str()).getUInt();
		soap->m_Price		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PRICE.c_str()).getUInt();
		soap->m_PicId		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PIC_ID.c_str()).getUInt();
		soap->m_Stock		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_STOCK.c_str()).getUInt();
		soap->m_Name		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_NAME.c_str()).getString();
		soap->m_Desc		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_DESC.c_str()).getString();
		soap->m_PicFile		= query.getColumn(Soap::DB_TABLE_SOAP_COLUMN_PIC_FILE.c_str()).getString();
		soaps.push_back(soap);
	}
	return soaps;
}

User::Ptr DBInterface::getUserForChatId(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getUserForChatId : %d\n", chatId); fflush(m_Fp);
	std::stringstream ss;
	ss << "SELECT * FROM User WHERE " << User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " <<
		chatId << ";";
	fprintf(m_Fp, "AURA: getUserForChatId %s\n", ss.str().c_str()); fflush(m_Fp);
	User::Ptr user = std::make_shared<User>();
	SQLite::Statement   query(*m_hDB, ss.str());
	if(query.executeStep()) {
		user->m_UserId	= query.getColumn(User::DB_TABLE_USER_COLUMN_ID.c_str()).getUInt();
		user->m_ChatId	= query.getColumn(User::DB_TABLE_USER_COLUMN_CHAT_ID.c_str()).getUInt();
		user->m_Mobile	= query.getColumn(User::DB_TABLE_USER_COLUMN_MOBILE.c_str()).getInt64();
		user->m_FName	= query.getColumn(User::DB_TABLE_USER_COLUMN_FNAME.c_str()).getString();
		user->m_LName	= query.getColumn(User::DB_TABLE_USER_COLUMN_LNAME.c_str()).getString();
		user->m_OrderNo	= query.getColumn(User::DB_TABLE_USER_COLUMN_ORDER_NO.c_str()).getUInt();
	}
	return user;
}

bool DBInterface::emptyCartForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: emptyCartForUser\n"); fflush(m_Fp);
	int order_no = getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "DELETE from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
	return true;
}

void DBInterface::addToCart(unsigned int soapId, unsigned int chatId, unsigned int qnty, CartStatus stat) {
	fprintf(m_Fp, "AURA: addToCart %d : %d\n", soapId, qnty); fflush(m_Fp);
	std::stringstream ss;
	int order_no = getOrderNoForUser(chatId);
	ss << "SELECT * from Cart WHERE " <<
			Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no <<
					" AND " <<
			Cart::DB_TABLE_CART_COLUMN_SOAP_ID <<" = " << soapId << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	if(query.executeStep()) {
		fprintf(m_Fp, "AURA: Updating Cart : %d - %d\n", soapId, qnty); fflush(m_Fp);
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string()); ss << "UPDATE Cart set " << Cart::DB_TABLE_CART_COLUMN_QNTY << " = " << qnty << ";";
		m_hDB->exec(ss.str());
		transaction.commit();
	} else {
		fprintf(m_Fp, "AURA: Inserting into Cart : %d - %d\n", soapId, qnty); fflush(m_Fp);
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "INSERT INTO Cart (" <<
				Cart::DB_TABLE_CART_COLUMN_SOAP_ID << ", " <<
				Cart::DB_TABLE_CART_COLUMN_CHAT_ID << ", " <<
				Cart::DB_TABLE_CART_COLUMN_QNTY << ", " <<
				Cart::DB_TABLE_CART_COLUMN_STATUS << ", " <<
				Cart::DB_TABLE_CART_COLUMN_ORDER_NO << ") VALUES (" <<
				soapId << "," << chatId << "," << qnty << "," <<
				getIntStatus(stat) << "," << order_no << ");";
		m_hDB->exec(ss.str());
		transaction.commit();
	}
}

std::vector<Cart::Ptr> DBInterface::getCartForOrderNo(unsigned int order_no) {
	fprintf(m_Fp, "AURA: getCartForOrderNo(%d)\n", order_no); fflush(m_Fp);
	std::vector<Cart::Ptr> items;
	Cart::Ptr item;
	std::stringstream ss;
	ss << "SELECT * from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	while(query.executeStep()) {
		item = std::make_shared<Cart>();
		item->m_CartId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_CART_ID.c_str()).getUInt();
		item->m_SoapId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_SOAP_ID.c_str()).getUInt();
		item->m_ChatId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_CHAT_ID.c_str()).getUInt();
		item->m_Qnty		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_QNTY.c_str()).getUInt();
		item->m_OrderNo		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_ORDER_NO.c_str()).getUInt();
		item->m_Status		= CartStatus::PENDING;
		items.push_back(item);
	}
	return items;
}

std::vector<Cart::Ptr> DBInterface::getUserCart(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getUserCart(%d)\n", chatId); fflush(m_Fp);
	return getCartForOrderNo(getOrderNoForUser(chatId));
	/*std::vector<Cart::Ptr> items;
	Cart::Ptr item;
	std::stringstream ss;
	int order_no = getOrderNoForUser(chatId);
	ss << "SELECT * from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	while(query.executeStep()) {
		item = std::make_shared<Cart>();
		item->m_CartId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_CART_ID.c_str()).getUInt();
		item->m_SoapId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_SOAP_ID.c_str()).getUInt();
		item->m_ChatId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_CHAT_ID.c_str()).getUInt();
		item->m_Qnty		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_QNTY.c_str()).getUInt();
		item->m_OrderNo		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_ORDER_NO.c_str()).getUInt();
		item->m_Status		= CartStatus::PENDING;
		items.push_back(item);
	}
	return items;*/
}

unsigned int DBInterface::getChatIdForOrderNo(unsigned int order_no) {
	fprintf(m_Fp, "AURA: getChatIdForOrderNo order_no: %d\n", order_no); fflush(m_Fp);
	unsigned int chatId = 0;
	std::stringstream ss;
	ss << "SELECT " << POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID << " FROM POrder WHERE " <<
		POrder::DB_TABLE_PORDER_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	if(query.executeStep()) {
		chatId = query.getColumn(POrder::DB_TABLE_PORDER_COLUMN_CHAT_ID.c_str()).getUInt();
	}
	return chatId;
}

unsigned int DBInterface::getOrderNoForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getOrderNoForUser chatId: %d\n", chatId); fflush(m_Fp);
	unsigned int order_no = 0;
	std::stringstream ss;
	ss << "SELECT " << User::DB_TABLE_USER_COLUMN_ORDER_NO << " FROM User WHERE " <<
		User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " << chatId << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	if(query.executeStep()) {
		order_no = query.getColumn(User::DB_TABLE_USER_COLUMN_ORDER_NO.c_str()).getUInt();
	}
	return order_no;
}

unsigned int DBInterface::generateOrderNo() {
	fprintf(m_Fp, "AURA: generateOrderNo\n"); fflush(m_Fp);
	int invoice_no = 0;
	std::stringstream ss;
	ss << "SELECT MAX(" << User::DB_TABLE_USER_COLUMN_ORDER_NO << ") from User;";
	SQLite::Statement query(*m_hDB, ss.str());
	if(query.executeStep()) {
		ss.str(std::string()); ss << "MAX(" << User::DB_TABLE_USER_COLUMN_ORDER_NO << ")";
		invoice_no = query.getColumn(ss.str().c_str()).getUInt();
	}
	return invoice_no + 1;
}

void DBInterface::updateOrderNoForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: updateOrderNoForUser chatId: %d\n", chatId); fflush(m_Fp);
	std::stringstream ss;
	ss << "UPDATE User SET " << User::DB_TABLE_USER_COLUMN_ORDER_NO << " = " << generateOrderNo()
		<< " WHERE " << User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " << chatId << ";";
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str());
	transaction.commit();
}

bool DBInterface::addNewUser(int64_t chatId, std::string fname, std::string lname, int64_t mobile) {
	fprintf(m_Fp, "AURA: addNewUser chatId: %lld, fname :%s\n", chatId, fname.c_str()); fflush(m_Fp);
	unsigned int order_no = generateOrderNo();
	std::stringstream ss;
	ss << "SELECT * FROM User WHERE " << User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " << chatId << ";";
	SQLite::Statement   query(*m_hDB, ss.str());
	if(!query.executeStep()) {
		fprintf(m_Fp, "AURA: New User %s\n", fname.c_str()); fflush(m_Fp);
		//	A new user. So add him / her to database
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << " INSERT INTO User (" << User::DB_TABLE_USER_COLUMN_FNAME << ", " <<
				User::DB_TABLE_USER_COLUMN_LNAME << ", " <<
				User::DB_TABLE_USER_COLUMN_CHAT_ID << ", " <<
				User::DB_TABLE_USER_COLUMN_ORDER_NO << ") VALUES (\"" <<
				fname << "\", \"" << lname << "\", " << chatId << ", " << order_no << ");";
		m_hDB->exec(ss.str());
		transaction.commit();
	} else {
		fprintf(m_Fp, "AURA: User %s alreay existing\n", fname.c_str()); fflush(m_Fp);
		return false;
	}
	return true;
}