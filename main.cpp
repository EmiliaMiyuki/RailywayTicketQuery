#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "network.hpp"
#include "json.hpp"

using namespace std;
namespace po = boost::program_options;
namespace asio = boost::asio;

enum {
    TRAIN_TYPE_STUDENT = 0,
    TRAIN_TYPE_NORMAL
};

void print_help() {
	printf("Usage:  railway <time> <from> <to> [-adghkosz]\n\n");
    printf("Parameters: \n");
    printf("    -h  --help      显示此帮助信息\n");
    printf("    -d              显示动车车票\n");
    printf("    -g              显示城际/高铁车票\n");
    printf("    -z              显示直达车票\n");
    printf("    -k              显示快速车票\n");
    printf("    -o              显示其他车票\n");
    printf("    -a  --all       显示全部车票 (此选项的优先级最高)\n");
    printf("    -s  --student   查询学生票\n");
}

char * get_station_code(const char * station) {

}

void get_train_list(const char * start, const char * end, const char * date, int type = TRAIN_TYPE_NORMAL) {
    char str[20000];
    http_get("kyfw.12306.cn", 
        (string("https://kyfw.12306.cn/otn/leftTicket/queryT?leftTicketDTO.train_date=")+date+"&leftTicketDTO.from_station="+start+"&leftTicketDTO.to_station="+end+"&purpose_codes="+"ADULT").c_str(), 
        str, 
        nullptr, 
        "https://kyfw.12306.cn/otn/leftTicket/init"
    );
    //cout << str;
    try {
        Json::JsonScanner json(str, false);
        if (json["status"]) {
            Json::Array & trains = json["data"].asArray();
            int result_len = trains.size();
            cout << "共查询到" << result_len << "个结果。\n";
            for (int i=0; i<result_len; i++) {
                Json::Json& train = trains[i]["queryLeftNewDTO"].asJson();
                cout << "车次：" << train["station_train_code"]  << " (" << train["start_station_name"] << " -> " << train["to_station_name"] << ")" << endl
                    << "出发时间：" << train["start_time"] << "，到达时间：" << train["arrive_time"] << "，用时：" << train["lishi"] << endl
                    << "剩余位置：\n  商务座：" << train["swz_num"] << "，特等座：" << train["tz_num"] << "，一等座：" << train["zy_num"] 
                    << "，二等座：" << train["yw_num"] << "，高级软卧：" << train["gr_num"] << "，软卧：" << train["rw_num"] << "，硬卧：" << train["yw_num"]
                    << "，软座：" << train["rz_num"] << "，硬座：" << train["yz_num"] << "，无座：" << train["wz_num"] << "，其他：" << train["qt_num"] << endl << endl;  
            }
        }
        else {
            cout << "查询失败。\n";
            return;
        }
    }
    catch(Json::Exception * e) {
        cout << "Syntax error: ";
        if (e) {
            cout << e->what();
            delete e;
        }
    }
    catch (const char * e) {
        cout << e;
    }
}

int main(int argc, const char ** argv){
    //get_train_list("PRW", "CDW", "2016-09-05", TRAIN_TYPE_NORMAL);
    //return 0;

	po::options_description desc("参数说明");  
    desc.add_options()  
    	("to,o", po::value<string>(), "终点(必须)")
		("from,f", po::value<string>(), "起始地 (必须)")
		("time,t", po::value<string>(), "触发时间 (必须)")
		("help,h", "显示此帮助信息")  
		("d", "显示动车车票")
		("g", "显示城际/高铁车票")
		("z", "显示直达车票")
		("k", "显示快速车票")
		("o", "显示其他车票")
		("all,a", "显示全部车票 (此选项的优先级最高)")
		("student,s", "查询学生票");

	boost::program_options::positional_options_description poptd;
	poptd.add("time", 1);
	poptd.add("from", 1);
	poptd.add("to", 1);

	try {
    	po::variables_map vm;  
    	po::store(po::command_line_parser(argc, argv).options(desc).positional(poptd).run(), vm);  
    	po::notify(vm);  

    	if (vm.count("help")) {
    		print_help();
    		return 0;
    	}
    	if (!vm.count("time")) {
    		print_help();
    		return 2;
    	}
    	if (!vm.count("from")) {
    		print_help();
    		return 1;
    	}
    	if (!vm.count("to")) {
    		print_help();
    		return 3;
    	}
    	if (vm.count("d")) {

    	}
    	if (vm.count("g")) {

    	}
		if (vm.count("z")) {

    	}
    	if (vm.count("k")) {

    	}
    	if (vm.count("o")) {

    	}
    	if (vm.count("all")) {
    		
    	}
    	if (vm.count("student")) {

    	}
    	cout << "你查询的是于"
    		<< vm["time"].as<string>()
			<< "从" << vm["from"].as<string>()
			<< "到" << vm["to"].as<string>() << "的火车" << endl;
        get_train_list(vm["from"].as<string>().c_str(), vm["to"].as<string>().c_str(), vm["time"].as<string>().c_str(), TRAIN_TYPE_NORMAL);
    }
 	catch(exception& e)
    {
        cout << e.what() << "\nType 'railway --help' for a list of allowed parameters.\n";
        return 1;
    }
}