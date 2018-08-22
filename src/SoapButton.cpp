//sgn

#include <iostream>
#include <memory>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

#include <SoapButton.h>
#include <DBNames.h>
#include <FlavoursButton.h>


std::string SoapButton::STR_CHOOSE_A_SOAP = "Choose a soap from below list";

TgBot::ReplyKeyboardMarkup::Ptr SoapButton::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	std::vector<TgBot::KeyboardButton::Ptr> btnSoaps;
	TgBot::KeyboardButton::Ptr btnSoap;
	std::shared_ptr<FlavoursButton> flvrsBtn = std::make_shared<FlavoursButton>(getDBHandle());

	for(auto &soapName : soapNames) {
		btnSoap 				= std::make_shared<TgBot::KeyboardButton>();
		btnSoap->text			= soapName;
		listAuraBtns[soapName]	= flvrsBtn;
		btnSoaps.push_back(btnSoap);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlavoursMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	for(auto &btnSoap : btnSoaps) {
		row.push_back(btnSoap);
		pFlavoursMenu->keyboard.push_back(row);
	}
	return pFlavoursMenu;
}

void SoapButton::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	SQLite::Statement   query(*m_hDB, "SELECT * FROM Soap;");

	int		iColNo		= 0;
	bool	isColSet	= false;
	while(query.executeStep()) {

		//	First find the column number for Soap Names
		while(!isColSet && iColNo < query.getColumnCount()) {
			if(DB_TABLE_SOAP_COLUMN_NAME == query.getColumnName(iColNo)) {
				isColSet = true;
				break;
			}
			iColNo++;
		}
		const std::string soapName = query.getColumn(iColNo);
		fprintf(fp, "AURA: Queried a flavour \"%s\" from Soap Table\n", soapName.c_str());
		soapNames.push_back(soapName);
	}
}
