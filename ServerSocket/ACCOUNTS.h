#pragma once
#include <iostream>
#include <stdio.h>


class ACCOUNTS
{
public:
	ACCOUNTS();
	virtual ~ACCOUNTS();
	//virtual void setName(std::string aName);
	virtual bool isRegisterd();
	virtual void registerAccount();
private:
	bool registerd;
	std::string name;
	//TODO set password
};

