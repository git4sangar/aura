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

	std::string User::DB_TABLE_USER_COLUMN_ID 		= "user_id";
	std::string User::DB_TABLE_USER_COLUMN_FNAME		= "first_name";
	std::string User::DB_TABLE_USER_COLUMN_LNAME		= "last_name";
	std::string User::DB_TABLE_USER_COLUMN_ADDRRESS		= "address";
	std::string User::DB_TABLE_USER_COLUMN_CHAT_ID		= "chat_id";
	std::string User::DB_TABLE_USER_COLUMN_MOBILE		= "mobile";
	std::string User::DB_TABLE_USER_COLUMN_ORDER_NO		= "order_no";

	std::string Cart::DB_TABLE_CART_COLUMN_CART_ID		= "cart_id";
	std::string Cart::DB_TABLE_CART_COLUMN_SOAP_ID		= "soap_id";
	std::string Cart::DB_TABLE_CART_COLUMN_USER_ID		= "user_id";
	std::string Cart::DB_TABLE_CART_COLUMN_STATUS		= "status";
	std::string Cart::DB_TABLE_CART_COLUMN_QNTY		= "quantity";
	std::string Cart::DB_TABLE_CART_COLUMN_ORDER_NO		= "order_no";

	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_SHIP_ID	= "ship_id";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_USER_ID	= "user_id";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_APT_NAME	= "apt_name";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_BLOCK_NO	= "block_no";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_FLAT_NO	= "flat_no";
	std::string Shipping::DB_TABLE_SHIPPING_COLUMN_ORDER_NO	= "order_no";

DBInterface::DBInterface(std::string dbFileName, FILE *fp) {
	m_hDB   = std::make_shared<SQLite::Database>(dbFileName, SQLite::OPEN_READWRITE);
	m_Fp	= fp;
}

DBInterface::~DBInterface() {}

void DBInterface::updateMobileNo(unsigned int chatId, std::string strMobileNo) {
	fprintf(m_Fp, "AURA: updateMobileNo\n"); fflush(m_Fp);
	unsigned int mobileNo;
	SQLite::Transaction transaction(*m_hDB);

	std::stringstream ss(strMobileNo);
	ss >> mobileNo;

	ss.str(std::string());
	ss << "UPDATE User set " << User::DB_TABLE_USER_COLUMN_MOBILE << " = " << mobileNo <<
		"WHERE " << User::DB_TABLE_USER_COLUMN_CHAT_ID << " = " << chatId << ";";
	m_hDB->exec(ss.str().c_str());
	transaction.commit();
}

Soap::Ptr DBInterface::getSoapById(unsigned int soapId) {
	fprintf(m_Fp, "AURA: getSoapById\n"); fflush(m_Fp);
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
	}
	return soap;
}

int DBInterface::getIntStatus(CartStatus stat) {
	int iStat;
	switch(stat) {
		case CartStatus::PENDING:
			iStat = 1;
			break;
		case CartStatus::PAID:
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

void DBInterface::addToShipping(unsigned int chatId, std::string aptName, std::string blkNo, unsigned int flatNo) {

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
		soaps.push_back(soap);
	}
	return soaps;
}

User::Ptr DBInterface::getUserForChatId(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getUserForChatId\n"); fflush(m_Fp);
	User::Ptr user = std::make_shared<User>();
	SQLite::Statement   query(*m_hDB, "SELECT * FROM User WHERE chat_id = ?");
	long long llid = static_cast<long long>(chatId);
	query.bind(1, llid);
	if(query.executeStep()) {
		user->m_UserId	= query.getColumn(User::DB_TABLE_USER_COLUMN_ID.c_str()).getUInt();
		user->m_ChatId	= query.getColumn(User::DB_TABLE_USER_COLUMN_CHAT_ID.c_str()).getUInt();
		user->m_Mobile	= query.getColumn(User::DB_TABLE_USER_COLUMN_MOBILE.c_str()).getUInt();
		user->m_FName	= query.getColumn(User::DB_TABLE_USER_COLUMN_FNAME.c_str()).getString();
		user->m_LName	= query.getColumn(User::DB_TABLE_USER_COLUMN_LNAME.c_str()).getString();
		user->m_Address	= query.getColumn(User::DB_TABLE_USER_COLUMN_ADDRRESS.c_str()).getString();
		user->m_OrderNo	= query.getColumn(User::DB_TABLE_USER_COLUMN_ORDER_NO.c_str()).getUInt();
	}
	return user;
}

void DBInterface::emptyCartForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: emptyCartForUser\n"); fflush(m_Fp);
	int order_no = getOrderNoForUser(chatId);
	std::stringstream ss;
	ss << "DELETE from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Transaction transaction(*m_hDB);
	m_hDB->exec(ss.str().c_str());
	transaction.commit();
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
		m_hDB->exec(ss.str().c_str());
		transaction.commit();
	} else {
		fprintf(m_Fp, "AURA: Inserting into Cart : %d - %d\n", soapId, qnty); fflush(m_Fp);
		User::Ptr user = getUserForChatId(chatId);
		SQLite::Transaction transaction(*m_hDB);
		ss.str(std::string());
		ss << "INSERT INTO Cart (" <<
				Cart::DB_TABLE_CART_COLUMN_SOAP_ID << ", " <<
				Cart::DB_TABLE_CART_COLUMN_USER_ID << ", " <<
				Cart::DB_TABLE_CART_COLUMN_QNTY << ", " <<
				Cart::DB_TABLE_CART_COLUMN_STATUS << ", " <<
				Cart::DB_TABLE_CART_COLUMN_ORDER_NO << ") VALUES (" <<
				soapId << "," << user->m_UserId << "," << qnty << "," <<
				getIntStatus(stat) << "," << user->m_OrderNo << ");";
		m_hDB->exec(ss.str().c_str());
		transaction.commit();
	}
}

std::vector<Cart::Ptr> DBInterface::getUserCart(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getUserCart(%d)\n", chatId); fflush(m_Fp);
	std::vector<Cart::Ptr> items;
	Cart::Ptr item;
	std::stringstream ss;
	int order_no = getOrderNoForUser(chatId);
	ss << "SELECT * from Cart WHERE " << Cart::DB_TABLE_CART_COLUMN_ORDER_NO << " = " << order_no << ";";
	SQLite::Statement query(*m_hDB, ss.str());
	while(query.executeStep()) {
		item = std::make_shared<Cart>();
		item->m_CartId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_CART_ID.c_str()).getUInt();
		item->m_SoapId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_SOAP_ID.c_str()).getUInt();
		item->m_UserId		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_USER_ID.c_str()).getUInt();
		item->m_Qnty		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_QNTY.c_str()).getUInt();
		item->m_OrderNo		= query.getColumn(Cart::DB_TABLE_CART_COLUMN_ORDER_NO.c_str()).getUInt();
		item->m_Status		= CartStatus::PENDING;
		items.push_back(item);
	}
	return items;
}

unsigned int DBInterface::getOrderNoForUser(unsigned int chatId) {
	fprintf(m_Fp, "AURA: getOrderNoForUser\n"); fflush(m_Fp);
	int order_no = 0;
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

bool DBInterface::addNewUser(int64_t chatId, std::string fname, std::string lname, int64_t mobile) {
	fprintf(m_Fp, "AURA: addNewUser\n"); fflush(m_Fp);
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
		m_hDB->exec(ss.str().c_str());
		transaction.commit();
	} else {
		fprintf(m_Fp, "AURA: User %s alreay existing\n", fname.c_str()); fflush(m_Fp);
		return false;
	}
	return true;
}