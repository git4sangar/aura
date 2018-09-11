//sgn

#ifndef __DB_NAMES_H__
#define __DB_NAMES_H__

#include <iostream>
#include <vector>
#include <memory>

#include <SQLiteCpp/SQLiteCpp.h>
#include <tgbot/tgbot.h>

class User {
public:
	typedef std::shared_ptr<User> Ptr;
	unsigned int m_UserId, m_ChatId, m_OrderNo;
	unsigned long long m_Mobile;
	std::string m_FName, m_LName, m_Address;

	static std::string DB_TABLE_USER_COLUMN_ID;
	static std::string DB_TABLE_USER_COLUMN_FNAME;
	static std::string DB_TABLE_USER_COLUMN_LNAME;
	static std::string DB_TABLE_USER_COLUMN_ADDRRESS;
	static std::string DB_TABLE_USER_COLUMN_CHAT_ID;
	static std::string DB_TABLE_USER_COLUMN_MOBILE;
	static std::string DB_TABLE_USER_COLUMN_ORDER_NO;
};

class Soap {
public:
	typedef std::shared_ptr<Soap> Ptr;
	unsigned int m_SoapId, m_Weight, m_Price, m_PicId, m_Stock;
	std::string m_Name, m_Desc;

	static std::string DB_TABLE_SOAP_COLUMN_ID;
	static std::string DB_TABLE_SOAP_COLUMN_NAME;
	static std::string DB_TABLE_SOAP_COLUMN_WEIGHT;
	static std::string DB_TABLE_SOAP_COLUMN_DESC;
	static std::string DB_TABLE_SOAP_COLUMN_PIC_ID;
	static std::string DB_TABLE_SOAP_COLUMN_STOCK;
	static std::string DB_TABLE_SOAP_COLUMN_PRICE;
};

enum class CartStatus{PENDING, PAID, DELIVERED, NOTA};

class POrder {
public:
	typedef std::shared_ptr<POrder> Ptr;
	unsigned int m_OrderId, m_OrderNo, m_ChatId, m_DateTime;
	std::string m_Date, m_Time;
	CartStatus m_Status;

	static std::string DB_TABLE_PORDER_COLUMN_ORDER_ID;
	static std::string DB_TABLE_PORDER_COLUMN_ORDER_NO;
	static std::string DB_TABLE_PORDER_COLUMN_CHAT_ID;
	static std::string DB_TABLE_PORDER_COLUMN_DATE;
	static std::string DB_TABLE_PORDER_COLUMN_TIME;
	static std::string DB_TABLE_PORDER_COLUMN_DATE_TIME;
	static std::string DB_TABLE_PORDER_COLUMN_PAY_GW;
	static std::string DB_TABLE_PORDER_COLUMN_STATUS;
};

class Cart {
public:
	typedef std::shared_ptr<Cart> Ptr;
	unsigned int m_CartId, m_SoapId, m_ChatId, m_Qnty, m_OrderNo;
	CartStatus m_Status;

	static std::string DB_TABLE_CART_COLUMN_CART_ID;
	static std::string DB_TABLE_CART_COLUMN_SOAP_ID;
	static std::string DB_TABLE_CART_COLUMN_CHAT_ID;
	static std::string DB_TABLE_CART_COLUMN_STATUS;
	static std::string DB_TABLE_CART_COLUMN_QNTY;
	static std::string DB_TABLE_CART_COLUMN_ORDER_NO;
};

class Shipping {
public:
	typedef std::shared_ptr<Shipping> Ptr;
	unsigned int m_ShippingId, m_ChatId, m_FlatNo, m_OrderNo;
	std::string m_AptName, m_BlockNo;

	static std::string DB_TABLE_SHIPPING_COLUMN_SHIP_ID;
	static std::string DB_TABLE_SHIPPING_COLUMN_CHAT_ID;
	static std::string DB_TABLE_SHIPPING_COLUMN_APT_NAME;
	static std::string DB_TABLE_SHIPPING_COLUMN_BLOCK_NO;
	static std::string DB_TABLE_SHIPPING_COLUMN_FLAT_NO;
	static std::string DB_TABLE_SHIPPING_COLUMN_ORDER_NO;
};

class DBInterface {
	std::shared_ptr<SQLite::Database> m_hDB;
	FILE *m_Fp;

public:
	typedef std::shared_ptr<DBInterface> Ptr;

	DBInterface(std::string dbFileName, FILE *fp);
	~DBInterface();

	void updatePOrderPayGW(unsigned int chatId, std::string payGw);
	void deletePOrder(unsigned int chatId);
	void createPOrder(unsigned int chatId);
	void updateOrderNoForUser(unsigned int chatId);
	Soap::Ptr getSoapById(unsigned int soapId);
	std::vector<Soap::Ptr> getFlavours();
	//User::Ptr getUserForChatId(unsigned int chatId);
	void addToCart(unsigned int soapId, unsigned int chatId, unsigned int qnty, CartStatus stat = CartStatus::PENDING);
	int getIntStatus(CartStatus stat);
	std::tuple<std::string, unsigned int> getShippingForUser(unsigned int chatId);
	std::vector<Cart::Ptr> getUserCart(unsigned int chatId);
	unsigned int generateOrderNo();
	void addBlockNoToShipping(unsigned int chatId, std::string blkNo);
	void addFlatNoToShipping(unsigned int chatId, unsigned int flatNo);
	void addAptNameToShipping(unsigned int chatId, std::string aptName);
	bool updateShippingFromPrevOrder(unsigned int chatId, unsigned int m_OrderNo);
	bool emptyCartForUser(unsigned int chatId);
	unsigned int getOrderNoForUser(unsigned int chatId);
	void updateMobileNo(unsigned int chatId, std::string mobileNo);
	bool addNewUser(
			int64_t chatId,
			std::string fname,
			std::string lname = "",
			int64_t mobile = 0
		);
};

#endif
