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
	std::string ShippingAddress::STR_BTN_BRKFLD	= "Brookfield";
	std::string ShippingAddress::STR_BTN_GARUDA	= "Other";
	std::string ShippingAddress::STR_BTN_PURVA	= "Fountain SQ";
	std::string ShippingAddress::STR_BTN_CONTACT= "Contact";
	std::string ShippingAddress::STR_BTN_BACK	= "<-Address";
	std::string ShippingAddress::STR_BTN_PAYTM	= "Paytm to 98406 25165";
	std::string ShippingAddress::STR_BTN_TEZ	= "Google Pay / Tez to 98406 25165";
	std::string ShippingAddress::STR_BTN_ON_DELIVERY	= "Cash on Delivery";

void ShippingAddress::init(TgBot::Message::Ptr pMsg, FILE *fp) {
	m_RenderMenu = MenuRenderer::NOTA;
	m_ChatId = m_Rows = m_Cols = m_FloorNo = 0;
	m_StrMsg.clear(); m_Cache.clear(); m_NotifyStr.clear(); 
}

void ShippingAddress::clearAuraButtons(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, MenuRenderer item, FILE *fp) {
	fprintf(fp, "AURA %ld: ShippingAddress::clearAuraButtons { size: %ld\n", time(0), listAuraBtns.size()); fflush(fp);
	switch(item) {
		case MenuRenderer::BLOCK:
			for(auto &block : m_Blocks) listAuraBtns.erase(block);
			m_Blocks.clear();
			break;
		case MenuRenderer::BLOCK_NO:
			for(auto &blockNo : m_BlockNos) listAuraBtns.erase(blockNo.first);
			m_BlockNos.clear();
		break;
		case MenuRenderer::FLOOR:
			for(auto &floor : m_Floors) listAuraBtns.erase(floor.first);
			m_Floors.clear();
		break;
		case MenuRenderer::FLAT_NO:
			for(auto &flat : m_Flats) listAuraBtns.erase(flat.first);
			m_Flats.clear();
		break;
		default:
		case MenuRenderer::APARTMENT:
		case MenuRenderer::CONTACT:
		case MenuRenderer::CONFIRM:
		break;
	}
	fprintf(fp, "AURA %ld: ShippingAddress::clearAuraButtons size: %ld }\n", time(0), listAuraBtns.size()); fflush(fp);
}

std::vector<TgBot::KeyboardButton::Ptr> ShippingAddress::getLastRow(
			std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns,
			std::vector<TgBot::KeyboardButton::Ptr>&& lastRow) {
	TgBot::KeyboardButton::Ptr kbBtnBack	= std::make_shared<TgBot::KeyboardButton>();
	kbBtnBack->text	= STR_BTN_BACK;
	listAuraBtns[kbBtnBack->text]	= shared_from_this();
	lastRow.push_back(kbBtnBack);
	return lastRow;
}

TgBot::GenericReply::Ptr ShippingAddress::shareContactMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	TgBot::KeyboardButton::Ptr btnContact	= std::make_shared<TgBot::KeyboardButton>();
	btnContact->requestContact	= true;
	btnContact->text	= "Your Mobile no pls...";
	listAuraBtns[ShippingAddress::STR_BTN_CONTACT] = shared_from_this();
	std::vector<TgBot::KeyboardButton::Ptr> row;
	row.push_back(btnContact);
	TgBot::ReplyKeyboardMarkup::Ptr pContactsMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	pContactsMenu->keyboard.push_back(row);
	pContactsMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	return pContactsMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderCheckoutMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderConfirmMenu\" rendering\n", time(0)); fflush(fp);
	if(0 == m_FlatsRendered) clearAuraButtons(listAuraBtns, MenuRenderer::FLAT_NO, fp);
	TgBot::KeyboardButton::Ptr kbBtnPaytm, kbBtnTez, kbBtnOnDelivery;

	kbBtnPaytm 			= std::make_shared<TgBot::KeyboardButton>();
	kbBtnPaytm->text	= STR_BTN_PAYTM;
	listAuraBtns[kbBtnPaytm->text] = shared_from_this();

	kbBtnTez 			= std::make_shared<TgBot::KeyboardButton>();
	kbBtnTez->text		= STR_BTN_TEZ;
	listAuraBtns[kbBtnTez->text] = shared_from_this();

	kbBtnOnDelivery 					= std::make_shared<TgBot::KeyboardButton>();
	kbBtnOnDelivery->text				= STR_BTN_ON_DELIVERY;
	listAuraBtns[kbBtnOnDelivery->text] = shared_from_this();

	TgBot::ReplyKeyboardMarkup::Ptr pPayMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	row.clear(); row.push_back(kbBtnOnDelivery);
	pPayMenu->keyboard.push_back(row);
	row.clear(); row.push_back(kbBtnPaytm);
	pPayMenu->keyboard.push_back(row);
	row.clear(); row.push_back(kbBtnTez);
	pPayMenu->keyboard.push_back(row);

	pPayMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	fprintf(fp, "AURA %ld: Finishing renderCheckoutMenu\n", time(0)); fflush(fp);
	return pPayMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderFlatMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, std::string aptName, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderFlatMenu\" rendering\n", time(0)); fflush(fp);
	if(0 == m_FloorsRendered) clearAuraButtons(listAuraBtns, MenuRenderer::FLOOR, fp);
	TgBot::KeyboardButton::Ptr kbBtnFlat;
	std::vector<TgBot::KeyboardButton::Ptr> kbBtnFlats;
	std::vector<std::string> flrLetters = {"G", "F", "S", "T", "L"};

	m_Rows = 5;
	m_Cols = 4;

	int iLoop1 = 0, iLoop2 = 0, iRun = 0;
	fprintf(fp, "AURA %ld: aptName %s\n", time(0), aptName.c_str());
	if(std::string::npos != aptName.find(STR_BTN_BRKFLD)) {
		m_FloorNo = (MAX_NAVINS_FLOORS < m_FloorNo) ? MAX_NAVINS_FLOORS : m_FloorNo;
		m_Cache += flrLetters[m_FloorNo];
		iRun = 1;
	} else {
		iRun = (m_FloorNo * 100)+1;
	}
	for(iLoop1 = 0; iLoop1 < (m_Rows*m_Cols); iLoop1++, iRun++) {
		kbBtnFlat					= std::make_shared<TgBot::KeyboardButton>();
		kbBtnFlat->text				= m_Cache + " " + std::to_string(iRun);
		m_Flats[kbBtnFlat->text]	= std::make_tuple(m_Cache, iRun);
		listAuraBtns[kbBtnFlat->text]	= shared_from_this();
		kbBtnFlats.push_back(kbBtnFlat);
	}

	TgBot::ReplyKeyboardMarkup::Ptr pFlatsMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row;

	for(iLoop1 = 0, iRun = 0; iLoop1 < m_Rows; iLoop1++) {
		row.clear();
		for(iLoop2 = 0; iLoop2 < m_Cols; iLoop2++) {
			if(iRun < (m_Rows*m_Cols)) row.push_back(kbBtnFlats[iRun++]);
		}
		pFlatsMenu->keyboard.push_back(row);
	}

	pFlatsMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	m_FlatsRendered++;
	fprintf(fp, "AURA %ld: Finishing renderFlatMenu\n", time(0)); fflush(fp);
	return pFlatsMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderFloorMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderFloorMenu\" rendering\n", time(0)); fflush(fp);
	if(0 == m_BlockNosRendered) clearAuraButtons(listAuraBtns, MenuRenderer::BLOCK_NO, fp);
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

	pFloorsMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	m_FloorsRendered++;
	fprintf(fp, "AURA %ld: Finishing renderFloorMenu\n", time(0)); fflush(fp);
	return pFloorsMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderBlockNoMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderBlockNoMenu\" rendering\n", time(0)); fflush(fp);
	if(0 == m_BlocksRendered) clearAuraButtons(listAuraBtns, MenuRenderer::BLOCK, fp);
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

	pBlockNosMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	m_BlockNosRendered++;
	fprintf(fp, "AURA %ld: Finishing renderBlockNoMenu\n", time(0)); fflush(fp);
	return pBlockNosMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderBlockMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderBlockMenu\" rendering\n", time(0)); fflush(fp);
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
	pBlockMenu->keyboard.push_back(getLastRow(listAuraBtns,getMainMenu()));
	m_BlocksRendered++;
	fprintf(fp, "AURA %ld: Finishing renderBlockMenu\n", time(0)); fflush(fp);
	return pBlockMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::renderAptMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::renderAptMenu\" rendering\n", time(0)); fflush(fp);
	TgBot::KeyboardButton::Ptr kbBtnSSM = nullptr,
								kbBtnBrkFld = nullptr,
								kbBtnGaruda = nullptr,
								kbBtnPurva = nullptr,
								kbBtnPrev = nullptr;

	//	m_Addr is Full-address-string & order-no paid.
	if(!std::get<0>(m_Addr).empty()) {
		kbBtnPrev	= std::make_shared<TgBot::KeyboardButton>();
		kbBtnPrev->text	= std::get<0>(m_Addr);
		listAuraBtns[kbBtnPrev->text]	= shared_from_this();
		m_PreDfnd[kbBtnPrev->text]		= std::get<1>(m_Addr);
	}

	kbBtnSSM		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnBrkFld		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnPurva		= std::make_shared<TgBot::KeyboardButton>();
	kbBtnGaruda		= std::make_shared<TgBot::KeyboardButton>();

	kbBtnSSM->text		= STR_BTN_SSM;
	kbBtnBrkFld->text	= STR_BTN_BRKFLD;
	kbBtnPurva->text	= STR_BTN_PURVA;
	kbBtnGaruda->text	= STR_BTN_GARUDA;

	listAuraBtns[kbBtnSSM->text]	= shared_from_this();
	listAuraBtns[kbBtnBrkFld->text]	= shared_from_this();
	listAuraBtns[kbBtnPurva->text]	= shared_from_this();
	listAuraBtns[kbBtnGaruda->text]	= shared_from_this();

	m_Apts.push_back(kbBtnSSM->text);
	m_Apts.push_back(kbBtnBrkFld->text);
	m_Apts.push_back(kbBtnPurva->text);
	m_Apts.push_back(kbBtnGaruda->text);

	TgBot::ReplyKeyboardMarkup::Ptr pAptMenu	= std::make_shared<TgBot::ReplyKeyboardMarkup>();
	std::vector<TgBot::KeyboardButton::Ptr> row0, row1, row2;
	if(!std::get<0>(m_Addr).empty()) row0.push_back(kbBtnPrev);
	row1.push_back(kbBtnSSM); row1.push_back(kbBtnBrkFld);
	row2.push_back(kbBtnPurva); row2.push_back(kbBtnGaruda);

	if(!std::get<0>(m_Addr).empty()) pAptMenu->keyboard.push_back(row0);
	pAptMenu->keyboard.push_back(row1);
	pAptMenu->keyboard.push_back(row2);

	pAptMenu->keyboard.push_back(getMainMenu());
	fprintf(fp, "AURA %ld: Finishing renderAptMenu\n", time(0)); fflush(fp);
	return pAptMenu;
}

TgBot::GenericReply::Ptr ShippingAddress::prepareMenu(std::map<std::string, std::shared_ptr<AuraButton>>& listAuraBtns, FILE *fp) {
	fprintf(fp, "AURA %ld: \"ShippingAddress::prepareMenu\" onClick\n", time(0)); fflush(fp);
	TgBot::GenericReply::Ptr pMenu;
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
		{
			auto addr = getDBHandle()->getShippingForUser(m_ChatId);
			pMenu = renderFlatMenu(listAuraBtns, std::get<0>(addr), fp);
		}
		break;
		case MenuRenderer::CONTACT:
			pMenu = shareContactMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::CONFIRM:
			pMenu = renderCheckoutMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::DONE:
			pMenu = listAuraBtns["Main Menu"]->prepareMenu(listAuraBtns, fp);
		break;
		case MenuRenderer::ADDRESS:
			listAuraBtns[std::to_string(m_ChatId)] = shared_from_this();
			m_Context[m_ChatId] = UserContext::ADDRESS;
			pMenu = std::make_shared<TgBot::ReplyKeyboardRemove>();
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
	fprintf(fp, "AURA %ld: \"%s\" onClick\n", time(0), pMsg->text.c_str()); fflush(fp);
	std::map<std::string, std::tuple<std::string, int>>::iterator itr;
	std::map<std::string, unsigned int>::iterator itrPreDfnd;
	
	if(m_PreDfnd.end() != (itrPreDfnd = m_PreDfnd.find(pMsg->text))) {
		getDBHandle()->updateShippingFromPrevOrder(pMsg->chat->id, itrPreDfnd->second);
		m_RenderMenu	= MenuRenderer::CONFIRM;
		m_StrMsg		= getPaymentString(pMsg->chat->id, pMsg->from->firstName);
		getDBHandle()->deletePOrder(pMsg->chat->id);
		getDBHandle()->createPOrder(pMsg->chat->id);
	}

	//	Get the Apartment name first
	else if(ViewCart::STR_BTN_PURCHASE == pMsg->text || STR_BTN_BACK == pMsg->text) {
		std::vector<Cart::Ptr> items = getDBHandle()->getUserCart(pMsg->chat->id);
		if(items.size() == 0) {
			m_StrMsg		= "Your Cart is empty! Pls buy Soap to get that into Cart before purchase.";
		} else {
			m_Addr			= getDBHandle()->getShippingForUser(pMsg->chat->id);
			m_RenderMenu	= MenuRenderer::APARTMENT;
			m_StrMsg		= "Shipping Address: Choose your Apartment";
		}
	}

	//	Get the Block Letter
	else if(std::find(m_Apts.begin(), m_Apts.end(), pMsg->text) != m_Apts.end()) {
		if(pMsg->text == STR_BTN_GARUDA) {
			m_RenderMenu	= MenuRenderer::ADDRESS;
			m_ChatId		= pMsg->chat->id;
			m_StrMsg		= "Type your address in a single line & send.";
		} else {
			getDBHandle()->addAptNameToShipping(pMsg->chat->id, pMsg->text);
			m_RenderMenu	= MenuRenderer::BLOCK;
			m_StrMsg		= "Shipping Address: Choose your Block Letter\nEg: For C13, Choose C";
		}
	}

	//	Get the Block number
	else if(std::find(m_Blocks.begin(), m_Blocks.end(), pMsg->text) != m_Blocks.end()) {
		m_BlocksRendered--;
		m_RenderMenu	= MenuRenderer::BLOCK_NO;
		m_Cache			= pMsg->text;
		m_StrMsg		= "Shipping Address: Choose your Block No";
	}

	//	Get the Floor
	else if(m_BlockNos.end() != (itr = m_BlockNos.find(pMsg->text))) {
		m_BlockNosRendered--;
		m_RenderMenu	= MenuRenderer::FLOOR;
		//	Parse the block number & cache
		std::string strBlockNo = std::get<0>(itr->second);
		int iBlkNo = std::get<1>(itr->second);
		if(0 < iBlkNo) strBlockNo += std::to_string(iBlkNo);
		m_Cache = strBlockNo;
		getDBHandle()->addBlockNoToShipping(pMsg->chat->id, m_Cache);
		m_StrMsg		= "Shipping Address: Choose your Floor";
	}

	//	Get the Flat number
	else if(m_Floors.end() != (itr  = m_Floors.find(pMsg->text))) {
		m_FloorsRendered--;
		m_RenderMenu	= MenuRenderer::FLAT_NO;
		//	Parse the Floor number & cache
		m_Cache 	= std::get<0>(itr->second);
		m_FloorNo	= std::get<1>(itr->second);
		m_ChatId	= pMsg->chat->id;
		m_StrMsg	= "Shipping Address: Choose your Flat No\nIf it's 3rd floor, flat no 6, Choose 306 & so on";
	}

	//	Put the address in database & get contact
	else if(m_Flats.end() != (itr = m_Flats.find(pMsg->text))) {
		m_FlatsRendered--;
		getDBHandle()->addBlockNoToShipping(pMsg->chat->id, std::get<0>(itr->second));
		getDBHandle()->addFlatNoToShipping(pMsg->chat->id, std::get<1>(itr->second));
		/*User::Ptr user = getDBHandle()->getUserForChatId(pMsg->chat->id);
		if(!user->m_Mobile) {
			m_RenderMenu	= MenuRenderer::CONTACT;
			m_StrMsg		= "Share your contact";
		} else*/ {
			m_RenderMenu	= MenuRenderer::CONFIRM;
			m_StrMsg		= getPaymentString(pMsg->chat->id, pMsg->from->firstName);

			//	delete the old details
			getDBHandle()->deletePOrder(pMsg->chat->id);

			//	create new one
			getDBHandle()->createPOrder(pMsg->chat->id);
		}
	}

	// Put the mobile no in database & go to payments
	else if(STR_BTN_CONTACT == pMsg->text) {
		if(pMsg->contact) getDBHandle()->updateMobileNo(pMsg->chat->id, pMsg->contact->phoneNumber);
		m_RenderMenu	= MenuRenderer::CONFIRM;
		m_StrMsg		= getPaymentString(pMsg->chat->id, pMsg->from->firstName);
		getDBHandle()->deletePOrder(pMsg->chat->id);
		getDBHandle()->createPOrder(pMsg->chat->id);
	}

	else if(STR_BTN_PAYTM == pMsg->text || STR_BTN_TEZ == pMsg->text || STR_BTN_ON_DELIVERY == pMsg->text) {
		std::stringstream ss;
		m_RenderMenu	= MenuRenderer::DONE;
		if(STR_BTN_PAYTM == pMsg->text) {
			ss << "Please go to paytm app and transfer above mentioned amount to 98406 25615."
				<< "\n\nYou will receive an OTP from AURA bot with delivery details after the payment is received."
				<<"\nProvide the OTP during delivery.";
			getDBHandle()->updatePOrderPayGW(pMsg->chat->id, "Paytm");
			m_Cache = "Paytm";
		} else if(STR_BTN_TEZ == pMsg->text) {
			ss << "Please go to Google Pay/Tez app and transfer above mentioned amount to 98406 25615."
				<< "\n\nYou will receive an OTP from AURA bot with delivery details after the payment is received."
				<<"\nProvide the OTP during delivery.";
			getDBHandle()->updatePOrderPayGW(pMsg->chat->id, "Tez");
			m_Cache = "Tez";
		} else if(STR_BTN_ON_DELIVERY == pMsg->text) {
			ss << "You will receive a call in 24 hrs reg delivery.";
			getDBHandle()->updatePOrderPayGW(pMsg->chat->id, "Cash");
			m_Cache = "Cash";
		}
		m_StrMsg	= ss.str();
		m_NotifyStr = prepareNotifyStr(pMsg->chat->id);
		getDBHandle()->updateOrderNoForUser(pMsg->chat->id);
	}

	else {
		switch(m_Context[pMsg->chat->id]) {
			case UserContext::ADDRESS:
				fprintf(fp, "AURA %ld: Address is %s\n", time(0), pMsg->text.c_str());
				getDBHandle()->addAddressToShipping(pMsg->chat->id, pMsg->text);
				m_RenderMenu	= MenuRenderer::CONFIRM;
				m_StrMsg		= getPaymentString(pMsg->chat->id, pMsg->from->firstName);
				m_Context.erase(pMsg->chat->id);
			break;
		}
	}
	fprintf(fp, "AURA %ld: Finishing ShippingAddress::onClick\n", time(0)); fflush(fp);
}

std::string ShippingAddress::getNotifyStr(unsigned int chatId) {
	return m_NotifyStr;
}

std::string ShippingAddress::prepareNotifyStr(unsigned int chatId) {
	int iTotal = 0;
	std::tuple<std::string, int> delAddr = getDBHandle()->getShippingForUser(chatId);
	User::Ptr user = getDBHandle()->getUserForChatId(chatId);
	std::stringstream ss;
	ss << user->m_FName << " has made an Order No: " << user->m_OrderNo
		<< " via " << m_Cache << "\n";
	std::vector<Cart::Ptr> items = getDBHandle()->getUserCart(chatId);
	for(auto &item : items) {
		Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
		ss << std::setw(15) << soap->m_Name << " - "
			<< std::setw(2) << item->m_Qnty << " - "
			<< std::setw(3) << "₹ "
			<< std::setw(4)<< (soap->m_Price * item->m_Qnty) << "\n";
		iTotal += (soap->m_Price * item->m_Qnty);
	}
	ss << std::setw(20) << "Total = ₹ " << iTotal << "\n\n";
	ss << "<b>Shipping Address</b>\n" << std::get<0>(delAddr);
	return ss.str();
}

std::string ShippingAddress::getPaymentString(unsigned int chatId, std::string fname) {
	std::stringstream ss;
	int iTotal = 0;
	std::tuple<std::string, int> delAddr = getDBHandle()->getShippingForUser(chatId);

	ss << "<b>Cart</b>\n";
	std::vector<Cart::Ptr> items = getDBHandle()->getUserCart(chatId);

	for(auto &item : items) {
		Soap::Ptr soap = getDBHandle()->getSoapById(item->m_SoapId);
		ss << std::setw(15) << soap->m_Name << " - "
			<< std::setw(2) << item->m_Qnty << " - "
			<< std::setw(3) << "₹ "
			<< std::setw(4)<< (soap->m_Price * item->m_Qnty) << "\n";
		iTotal += (soap->m_Price * item->m_Qnty);
	}
	ss << std::setw(20) << "Total = ₹ " << iTotal << "\n";
	ss << "<b>Shipping Address</b>\n" << std::get<0>(delAddr);
	ss << "\n\nHi " << fname <<
				", Please choose a payment method to <b>CONFIRM your order</b>" <<
				" and mention Order number: " <<
				getDBHandle()->getOrderNoForUser(chatId) <<
				" while paying.\n\nPls use Paytm or GPay/Tez app to transfer ₹ " << iTotal <<
				" to 98406 25165. A confirmation \"telegram msg\" reaches you in 24hrs after your payment." <<
				"\n\n<b>*</b> AURA bot is <b>not</b> integrated with payment banks like Paytm, GPay/Tez, etc...\n";
	return ss.str();
}
