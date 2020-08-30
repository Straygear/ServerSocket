#pragma once


#include <mysql.h>
#include <iostream>
#include <string>
#include <sstream>

class Database
{
public:
	Database();
	virtual ~Database();
	virtual void connectDatabase();
	virtual void showTable();
	virtual void insertPerson();
private:
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;
	int qstate;

};

