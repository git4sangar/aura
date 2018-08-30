//SGN

#include <ShippingAddress.h>
#include <ViewCart.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <SQLiteCpp/SQLiteCpp.h>

	std::string ShippingAddress::STR_BTN_SSM	= "SSM Nagar";
	std::string ShippingAddress::STR_BTN_BRKFLD	= "Navins Brookfield";
	std::string ShippingAddress::STR_BTN_GARUDA	= "Garuda Avenue";

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::renderFlatMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::renderFlatMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnFlat;
	std::vector<TgBot::KeyboardButton::Ptr> kbBtnFlats;

	m_Rows = 4;
	m_Cols = 5;

	int iLoop1 = 0, iLoop2 = 0, iRun = 0;
	for(iLoop1 = 0, iRun = 1; iLoop1 < (m_Rows*m_Cols); iLoop1++, iRun++) {
		kbBtnFlat					= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFlat->text				= std::to_string(iRun);
		m_Flats[kbBtnFlat->text]	= std::make_tuple(m_Cache, iRun);
		listAuraBtns[kbBtnFlat->text]	= shared_from_this();
		kbBtnFlats.push_back(kbBtnFlat);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlatsMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	//	Add other Floor numbers
	for(iLoop1 = 0, iRun = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(kbBtnFlats[iRun++]);
		}
		pFlatsMenu->keyboard.push_back(row);
	}

	pFlatsMenu->keyboard.push_back(getMainMenu());
	return pFlatsMenu;
}

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::renderFloorMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::renderFloorMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnFloor;
	std::vector<TgBot::KeyboardButton::Ptr> kbBtnFloors;
	std::stringstream ss;

	m_Rows = 3;
	m_Cols = 3;

	int iLoop1 = 0, iLoop2 = 0, iRun = 0;
	for(iLoop1 = 0, iRun = 0; iLoop1 < (m_Rows*m_Cols); iLoop1++, iRun++) {
		kbBtnFloor						= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFloor->text				= m_Cache + " Floor " + std::to_string(iRun);
		m_Floors[kbBtnFloor->text]		= std::make_tuple(m_Cache, iRun);
		listAuraBtns[kbBtnFloor->text]	= shared_from_this();
		kbBtnFloors.push_back(kbBtnFloor);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFloorsMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	//	Add other Floor numbers
	for(iLoop1 = 0, iRun = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(kbBtnFloors[iRun++]);
		}
		pFloorsMenu->keyboard.push_back(row);
	}

	pFloorsMenu->keyboard.push_back(getMainMenu());
	return pFloorsMenu;
}

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::renderBlockNoMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::renderBlockNoMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnBlockNo;
	std::vector<TgBot::KeyboardButton::Ptr> kbBtnBlockNos;
	std::stringstream ss;

	m_Rows = 4;
	m_Cols = 5;

	int iLoop1 = 0, iLoop2 = 0, iRun = 0;
	for(iLoop1 = 0, iRun = 1; iLoop1 < (m_Rows*m_Cols); iLoop1++, iRun++) {
		kbBtnBlockNo					= std::make_shared<TgBot::KeyboardButton>();
		kbBtnBlockNo->text				= m_Cache + std::to_string(iRun);
		m_BlockNos[kbBtnBlockNo->text]	= std::make_tuple(m_Cache, iRun);
		listAuraBtns[kbBtnBlockNo->text]= shared_from_this();
		kbBtnBlockNos.push_back(kbBtnBlockNo);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pBlockNosMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	//	Add the skip button first
	TgBot::KeyboardButton::Ptr kbBtnSkip	= std::make_shared<TgBot::KeyboardButton>();
	kbBtnSkip->text					= std::string("Just ") + m_Cache + std::string(" Block");
	listAuraBtns[kbBtnSkip->text]	= shared_from_this();
	m_BlockNos[kbBtnSkip->text]		= std::make_tuple(m_Cache, 0);
	row.clear();
	row.push_back(kbBtnSkip);
	pBlockNosMenu->keyboard.push_back(row);

	//	Add other block numbers
	iRun = 0;
	for(iLoop1 = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(kbBtnBlockNos[iRun++]);
		}
		pBlockNosMenu->keyboard.push_back(row);
	}

	pBlockNosMenu->keyboard.push_back(getMainMenu());
	return pBlockNosMenu;
}

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::renderBlockMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::renderBlockMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnBlock;
	std::vector<TgBot::KeyboardButton::Ptr> kbBtnBlocks;
	std::stringstream ss;

	m_Rows = 3;
	m_Cols = 6;

	int iLoop1 = 0, iLoop2 = 0, iRun = 0;
	for(iLoop1 = 0, iRun = 0; iLoop1 < (m_Rows*m_Cols); iLoop1++, iRun++) {
		kbBtnBlock		= std::make_shared<TgBot::KeyboardButton>();
		ss.str(std::string());
		char alpha = 'A' + iRun;
		ss << alpha;
		kbBtnBlock->text			= ss.str();
		m_Blocks.push_back(kbBtnBlock->text);
		listAuraBtns[kbBtnBlock->text]	= shared_from_this();
		kbBtnBlocks.push_back(kbBtnBlock);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pBlockMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	iRun = 0;
	for(iLoop1 = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(kbBtnBlocks[iRun++]);
		}
		pBlockMenu->keyboard.push_back(row);
	}
	pBlockMenu->keyboard.push_back(getMainMenu());
	return pBlockMenu;
}

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::renderAptMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::renderAptMenu\" rendering\n"); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnSSM, kbBtnBrkFld, kbBtnGaruda;

	kbBtnSSM		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnBrkFld		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnGaruda		= std::make_shared<TgBot::KeyboardButton>();

	kbBtnSSM->text		= STR_BTN_SSM;
	kbBtnBrkFld->text	= STR_BTN_BRKFLD;
	kbBtnGaruda->text	= STR_BTN_GARUDA;

	listAuraBtns[kbBtnSSM->text]	= shared_from_this();
	listAuraBtns[kbBtnBrkFld->text]	= shared_from_this();
	listAuraBtns[kbBtnGaruda->text]	= shared_from_this();

	m_Apts.push_back(kbBtnSSM->text);
	m_Apts.push_back(kbBtnBrkFld->text);
	m_Apts.push_back(kbBtnGaruda->text);

	TgBot::ReplyKeyboardMarkup::Ptr pAptMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row0, row1, row2;
	row0.push_back(kbBtnSSM);
	row1.push_back(kbBtnBrkFld);
	row2.push_back(kbBtnGaruda);

	pAptMenu->keyboard.push_back(row0);
	pAptMenu->keyboard.push_back(row1);
	pAptMenu->keyboard.push_back(row2);

	pAptMenu->keyboard.push_back(getMainMenu());
	return pAptMenu;
}

TgBot::ReplyKeyboardMarkup::Ptr ShippingAddress::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA: \"ShippingAddress::prepareMenu\" onClick\n"); fflush(fp);
	TgBot::ReplyKeyboardMarkup::Ptr pMenu;
	switch(m_RenderMenu) {
		case MenuRenderer::APARTMENT:
			pMenu = renderAptMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::BLOCK:
			pMenu = renderBlockMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::BLOCK_NO:
			pMenu = renderBlockNoMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::FLOOR:
			pMenu = renderFloorMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::FLAT_NO:
			pMenu = renderFlatMenu(listAuraBtns, fp);
		break;
	}
	return pMenu;
}

std::string ShippingAddress::floorNoToString(int iFloorNo) {
	switch(iFloorNo) {
		case 0: return "Ground Floor";
		case 1: return "First Floor";
		case 2: return "Second Floor";
		case 3: return "Third Floor";
		case 4: return "Fourth Floor";
		case 5: return "Fifth Floor";
		case 6: return "Sixth Floor";
		case 7: return "Seventh Floor";
		case 8: return "Eighth Floor";
		case 9: return "Ninth Floor";
	}
	return std::string();
}

void ShippingAddress::onClick(TgBot::Message::Ptr pMsg, FILE *fp) {
	fprintf(fp, "AURA: \"%s\" onClick\n", pMsg->text.c_str()); fflush(fp);
	std::map<std::string, std::tuple<std::string,int>>::iterator itrBlkNos;
	std::map<std::string, std::tuple<std::string,int>>::iterator itrFlrNos;
	
	//	Get the Apartment name first
	if(ViewCart::STR_BTN_PURCHASE == pMsg->text) {
		m_RenderMenu	= MenuRenderer::APARTMENT;
		m_StrMsg		= "Shipping Address: Choose your Apartment";
	}

	//	Get the Block Letter
	else if(std::find(m_Apts.begin(), m_Apts.end(), pMsg->text) != m_Apts.end()) {
		m_RenderMenu	= MenuRenderer::BLOCK;
		m_StrMsg		= "Shipping Address: Choose your Block Letter\nEg: For C13, Choose C";
	}

	//	Get the Block number
	else if(std::find(m_Blocks.begin(), m_Blocks.end(), pMsg->text) != m_Blocks.end()) {
		m_RenderMenu	= MenuRenderer::BLOCK_NO;
		m_Cache			= pMsg->text;
		m_StrMsg		= "Shipping Address: Choose your Block No";
	}

	//	Get the Floor
	else if(m_BlockNos.end() != (itrBlkNos = m_BlockNos.find(pMsg->text))) {
		m_RenderMenu	= MenuRenderer::FLOOR;
		//	Parse the block number & cache
		std::string strBlockNo = std::get<0>(itrBlkNos->second);
		int iBlkNo = std::get<1>(itrBlkNos->second);
		if(0 < iBlkNo) strBlockNo += std::to_string(iBlkNo);
		m_Cache = strBlockNo;
		m_StrMsg		= "Shipping Address: Choose your Floor";
	}

	//	Get the Flat number
	else if(m_Floors.end() != (itrFlrNos  = m_Floors.find(pMsg->text))) {
		m_RenderMenu	= MenuRenderer::FLAT_NO;
		//	Parse the Floor number & cache
		std::string strBlkNFlr = std::get<0>(itrFlrNos->second);
		int iFlrNo = std::get<1>(itrFlrNos->second);
		m_Cache = strBlkNFlr + std::string(" ") + floorNoToString(iFlrNo);
		m_StrMsg		= "Shipping Address: Choose your Flat No\nEg: For 101, Choose 1 & so on";
	}
}