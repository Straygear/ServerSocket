#include "ACCOUNTS.h"

ACCOUNTS::ACCOUNTS() :name(""), registerd(false) {

}

ACCOUNTS::~ACCOUNTS(){}

bool ACCOUNTS::isRegisterd() {
	return registerd;
}

void ACCOUNTS::registerAccount() {
	registerd = true;
}