#include "Database.h"

Database::Database():row(0), res(0), qstate(0)   {
	conn = mysql_init(0);
}

Database::~Database(){}

void Database::connectDatabase() {
	conn = mysql_real_connect(conn, "localhost", "root", "admin", "testdb", 3306, nullptr, 0);

	if (conn) {
		std::cout << "succesful connection to database!" << std::endl;
	}
	else {
		std::cout << "Connection to database has failed!" << std::endl;
	}
}

void Database::showTable(){
	std::string query = "SELECT * FROM test";
	const char* q = query.c_str();
	qstate = mysql_query(conn, q);
	if (!qstate) {
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res)) {
			std::cout << "ID: " << row[0] << ", Name: " << row[1] << ", Value: " << row[2] << std::endl;
		}
	}
	else {
		std::cout << "Query failed: " << mysql_error(conn) << std::endl;
	}
}

void Database::insertPerson() {
	std::string name = "Ramon";
	const int size = sizeof(name);

	char n[size + 1];

	strcpy_s(n, name.c_str());



	int id = 4;
	int score = 100;

	std::ostringstream oss;
	oss << "INSERT INTO test(id, name, score) VALUES('" << id << "', '" << n << "', '" << score << "')";

	std::string insertq = oss.str();
	const char* q = insertq.c_str();
	qstate = mysql_query(conn, q);
	if (!qstate) {
		std::cout << std::endl << "Registration success!" << std::endl;
		showTable();
	}
	else {
		std::cout << "Query execution error!" << mysql_error(conn) << std::endl;
		std::cout << insertq << std::endl;
	}
	//showTable();
}