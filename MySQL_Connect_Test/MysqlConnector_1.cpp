#include <stdlib.h>
#include <iostream>

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

using namespace std;

int main()
{
	sql::Driver* driver;
	sql::Connection* conn;
	sql::Statement* stmt;
	sql::ResultSet* res;
	
	driver = get_driver_instance();
	conn = driver->connect("127.0.0.1:3306", "root", "");
	conn->setSchema("open_lobby");

	stmt = conn->createStatement();
	res = stmt->executeQuery("SELECT * FROM RED_USER");
	while (res->next()) 
	{
		cout << "\t... MySQL replies: ";
		cout << res->getString("ACCOUNT_ID") << endl;
		cout << "\t... MySQL says it again: ";
		cout << res->getString(1) << endl;
	}

	delete res;
	delete stmt;
	delete conn;
	
	std::cout << endl;

	int a;
	cin >> a;

	return EXIT_SUCCESS;
}