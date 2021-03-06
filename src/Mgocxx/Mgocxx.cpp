// Mgocxx.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <bson.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>

using namespace std;

int _finish = 0;
int thread_num = 8;		//默认线程数
string ip = "localhost:27017";
string json = "{\"hello\":\"world\"}";

void output()
{
	mongoc_init();

	string _ip = "mongodb://" + ip + "/";

	mongoc_client_t * client = mongoc_client_new(_ip.data());
	mongoc_collection_t * collection = mongoc_client_get_collection(client, "mydb", "mycoll");

	const char * _json = json.data();

	for (size_t i = 0; i < 100000; i++)
	{
		bson_error_t error;
		bson_t *doc = bson_new_from_json((const uint8_t *)_json, -1, &error);
		
		if (!mongoc_collection_insert_one(
			collection, doc, NULL, NULL, &error)) {
			fprintf(stderr, "%s\n", error.message);
		}

		bson_destroy(doc);
	}

	mongoc_collection_destroy(collection);
	mongoc_client_destroy(client);
	_finish++;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "未发现自定义参数, 以默认参数运行. mongodb://localhost:27017/ with 8 threads." << endl;
	}
	else
	{
		ip = argv[1];
		string _t = argv[2];
		thread_num = atoi(_t.data());
		cout << "发现自定义参数, 以参数运行. mongodb://" + ip +"/ with " + _t + " threads." << endl;
	}

	
	ifstream fin("data.txt");
	if (fin)
	{
		string s;
		getline(fin, s);
		json = s;
	}

	time_t start = time(NULL);

	for (uint8_t i = 0; i < thread_num; i++)
	{
		thread t(output);
		t.detach();
	}

	while (true)
	{
		if (_finish >= thread_num)
		{
			break;
		}
	}


	time_t end = time(NULL);

	cout << "Finish writing " << thread_num << "00000 rows in " << (end - start) << " seconds." << endl;

	cout << "Press any key to exit." << endl;

	mongoc_cleanup();

	getchar();
	return 0;
}

