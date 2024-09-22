#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include <regex>
#include <sstream>
#include <exception>

using json = nlohmann::json;

class error : public std::exception
{
public:
    error(const std::string& message) : message{ message }
    {}
    const char* what() const noexcept override
    {
        return message.c_str();     // получаем из std::string строку const char*
    }
private:
    std::string message;    // сообщение об ошибке
};

void splitLine(std::vector<std::string>& vec, const std::string line, const char delimeters);
template<class T>
bool in_range(const T t, const T min, const T max);
json getFile(const std::string file);
void toFile(const std::string file, json data);
void checkOptions(std::vector<std::string>& argv, int& argc, const std::map < std::string, int >& commands, const int count, const bool exist);

void help(int argc, std::vector<std::string> argv);
void info(int argc, std::vector<std::string> argv);
void addExpense(int argc, std::vector<std::string> argv);
void editExpense(int argc, std::vector<std::string> argv);
void delite(int argc, std::vector<std::string> argv);
void removeObj(int argc, std::vector<std::string> argv);

int main() {
    setlocale(LC_ALL, "en_US.UTF8");

    std::string command = "";
    int argc = 0;
    std::vector<std::string> argv{};

    while (true) {
        std::cout << "Введите команду: ";
        std::getline(std::cin, command);
        splitLine(argv, command, ' ');

        argc = argv.size();

        

        std::map < std::string, std::function<void(int argc, std::vector<std::string> argv)>> map = {
        {"help", help},
        {"info", info},
        {"addExpense", addExpense},
        {"editExpense", editExpense},
        {"delite", delite},
        {"remove", removeObj}
        };

        if (map.find(argv[0]) != map.end()) {
            try
            {
                map[argv[0]](argc, argv);
            }
            catch (const error& err) {
                std::cout << err.what() << std::endl;
            }
            catch (const std::exception ex) {
                std::cout << ex.what() << std::endl;
            }
        }
        else {
            std::cout << "Команда не найдена\nВведите help для помощи\n\n";
        }

        argc = 0;
        argv.clear();
    }
    

    system("pause");
    return 0;
}

void splitLine(std::vector<std::string>& vec, const std::string line, const char delimeters) {
    if (line == "") {
        throw error("Ошибка!\nВы не ввели команду!\n");
    }
    int j = 0;
    int count = 0;
    while (j < line.size()) {
        std::string sub_str = "";
        int i = j;
        while ((line[i] != delimeters) && (i < line.size())) {
            sub_str += line[i];
            i++;
            count++;
        }
        if (count > 0) {
            vec.push_back(sub_str);
        }
        count = 0;
        j = i + 1;
    }
}
template<class T>
bool in_range(const T t, const T min, const T max) {
    return t >= min && t <= max;
}
json getFile(const std::string file) {
    json data;
    std::ifstream input(file);
    if (input.is_open()) {
        if (!(input.peek() == -1)) {
            input >> data;
        }
        input.close();
        return data;
    }
    else {
        throw error("Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n");
    }

    
}
void toFile(const std::string file, json data) {
    std::ofstream output(file);
    if (!output.is_open())
    {
        throw error("Ошибка!\nНе удалось открыть файл data.json или вы ничего не запланировали\n");
    }
    output << data.dump(4);
    output.close();
}
void checkOptions(std::vector<std::string>& argv, int& argc, const std::map<std::string, int>& commands, const int count, const bool exist) {
    for (int i = count; i < argv.size() - 1; i++) {
        if (argv[i][0] == '-')
                continue;
        if (argv[i + 1][0] != '-') {
            argv[i] += (" " + argv[i + 1]);
            argv.erase(argv.begin() + i + 1);
            i--;
        }
    }
    argc = argv.size();

    /*for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << std::endl;
    }*/
    int c = 0;
    for (int i = count; i < argc; i++) {
        auto iter = commands.find(argv[i]);
        c++;
        if (iter == commands.end()) {
            throw error("Ошибка!\nОпции " + argv[i] + " не существует!\n");
        }
        else {
            int option_argc = 0;
            if ((i + 1) < argc) {
                if (argv[i + 1][0] == '-') {
                    continue;
                }
                std::vector<std::string> option_argv = {};
                splitLine(option_argv, argv[i + 1], ' ');
                option_argc = option_argv.size();
            }
            if (option_argc != iter->second) {
                throw error("Ошибка!\nНекорректное количество аргументов опции " + argv[i] + "\n");
            }
            else {
                i++;
            }
        }
    }
    if (exist && c == 0) {
        throw error("Ошибка!\nНедостаточно опций\n");
    }
}

void help(int argc, std::vector<std::string> argv) {
    std::cout <<std::setw(20) << std::left << "info" << std::setw(20) << "Получение информации\n";
    std::cout << std::setw(20) << std::left << "addExpense" << std::setw(20) << "Добвление покупки в планировщик\n";
    std::cout << std::setw(20) << std::left << "editExpense" << std::setw(20) << "Редактирование покупки\n";
    std::cout << std::setw(20) << std::left << "delite" << std::setw(20) << "Удаление траты\n";
    std::cout << std::setw(20) << std::left << "remove" << std::setw(20) << "Восстановление удаленной траты\n";
    std::cout << std::endl;
}

void info(int argc, std::vector<std::string> argv) {
    if (argc == 1) {
        std::cout << "Вывод различной информации\n\ninfo [<name>] [-a] [-e | --expense] [-s | --savings] \n\nОпции\n"
            << "\t[<name>]" << "\t\tНазвание траты; если не указано — все траты\n"
            << "\t[-a]" << "\t\t\tВся доступная информация\n"
            << "\t[-e | --expense]" << "\tСписок всех расходов\n"
            << "\t[-s | --savings]" << "\tИнформация о накоплениях\n\n";
        return;
    }
    std::map < std::string, int > options = {
            {"-a", 0},
            {"-e", 0},
            {"--expense", 0},
            {"-s", 0},
            {"--savings", 0},
    };

    if (argc > 1 && argv[1][0] != '-') {
        checkOptions(argv, argc, options, 2, true);
    }
    else {
        checkOptions(argv, argc, options, 1, true);
    }

    int i_begin = -1;
    int i_end = -1;

    if ((argc > 1 && argv[1][0] != '-')) {
        json data = getFile("data.json");

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                i_begin = i;
                i_end = i + 1;
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    time_t seconds = time(NULL);
    char time_line[43];
    tm* timeinfo = localtime(&seconds);

    strftime(time_line, 43, "Сейчас: %d.%m.%Y Время: %H:%M", timeinfo);
    std::cout << time_line << std::endl << std::endl;

    if ((std::find(argv.begin(), argv.end(), "-a") != argv.end()) || (std::find(argv.begin(), argv.end(), "--savings") != argv.end()) || (std::find(argv.begin(), argv.end(), "-s") != argv.end())) {
        
        json data = getFile("data.json");
        
        if (data["Expense"].size() == 0) {
            std::cout << "Покупок не запланированно\n\n";
        }
        else {
            int sum = 0;
            std::cout << "Название" << "\t    " << "Отложить" << "\t     " << "Остаток" << "\t      " << "Накопленно\n";
            if (i_begin == -1 || i_end == -1) {
                i_begin = 0;
                i_end = data["Expense"].size();
            }
            int mon_ex = 0;
            int mon_now = 0;
            int mon_start = 0;
            int mon_end = 0;

            for (int i = 0; i < data["Expense"].size(); i++) {
                std::vector<std::string> start_time{};
                std::vector<std::string> end_time{};
                splitLine(end_time, data["Expense"][i]["date"], '.');
                splitLine(start_time, data["Expense"][i]["start"], '.');

                mon_ex = ((atoi(end_time[2].c_str()) - atoi(start_time[2].c_str())) * 12) + (atoi(end_time[1].c_str()) - atoi(start_time[1].c_str()));
                
                mon_now = timeinfo->tm_year * 12 + timeinfo->tm_mon;
                mon_start = (atoi(start_time[2].c_str()) - 1900) * 12 + atoi(start_time[1].c_str()) - 1;
                mon_end = (atoi(end_time[2].c_str()) - 1900) * 12 + atoi(end_time[1].c_str()) - 1;

                if (in_range(mon_now, mon_start, mon_end)) {
                    data["Expense"][i]["accumulated"] = (int)(floor((float)data["Expense"][i]["price"] / mon_ex) * (float)(mon_now - mon_start));
                    sum += ceil(((float)data["Expense"][i]["price"] - (float)data["Expense"][i]["accumulated"]) / (float)(mon_ex - (mon_now - mon_start)));
                }

            }
            if (in_range(mon_now, mon_start, mon_end)) {
                for (int i = i_begin; i < i_end; i++) {
                    std::cout << std::setw(20) << std::left << (std::string)data["Expense"][i]["name"] << std::setw(20) << std::to_string((int)ceil(((float)data["Expense"][i]["price"] - (((float)data["Expense"][i]["accumulated"]) + ((float)data["Expense"][i]["custom_coef"]))) / (float)mon_ex)) + " руб" << std::setw(20) << std::to_string((int)data["Expense"][i]["price"] - (((int)data["Expense"][i]["accumulated"]) + ((int)data["Expense"][i]["custom_coef"]))) + " руб" << std::setw(20) << std::to_string((((int)data["Expense"][i]["accumulated"]) + ((int)data["Expense"][i]["custom_coef"]))) + " руб" << "\n";
                }
            }
            std::cout << "\nВ этом месяце требуется отложить " << sum << " руб\n\n";

            toFile("data.json", data);
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-a") != argv.end()) || (std::find(argv.begin(), argv.end(), "-e") != argv.end()) || (std::find(argv.begin(), argv.end(), "--expense") != argv.end())) {

        json data = getFile("data.json");

        if (!data.contains("Expense")) {
            std::cout << "Покупок не запланированно\n";
        }
        else {
            std::cout << "Всего запланированно " << data["Expense"].size() << " покупок:\n";
            std::cout << "№ " << "Название" << "\t      " << "Стоимость" << "\t       " << "Дата покупки" << "\t   " << "Дата накопления" << "     " << "Описание\n";
            for (int i = i_begin; i < i_end; i++) {
                std::cout << i << " " << std::setw(20) << std::left << (std::string)data["Expense"][i]["name"] << std::setw(20) << (std::to_string((int)data["Expense"][i]["price"]) + " руб") << std::setw(20) << (std::string)data["Expense"][i]["date"] << std::setw(20) << (std::string)data["Expense"][i]["start"] << std::setw(20) << (std::string)data["Expense"][i]["description"] << "\n";
            }
            std::cout << std::endl;
        }
    }

}

void addExpense(int argc, std::vector<std::string> argv) {

    if (argc == 1) {
        std::cout << "Добавление покупки в планировщик\n\naddExpense <name> <price> <date> <description>[-sd | --startdate]\n\nАргументы\n"
            << "\t<name>\t\t\t\t\tНазвание покупки\n"
            << "\t<price>\t\t\t\t\tСтоимость покупки\n"
            << "\t<date>\t\t\t\t\tДата планируемого совершения покупки\n"
            << "\t<description>\t\t\t\tОписание покупки\n\n"
            << "Опции\n"
            << "\t[-sd | --startdate] <start date>\tДата, с которой вы планируете начать копить\n\n";
        return;
    }
    if (argc < 5) {
        throw error("Ошибка!\nнедостаточно аргументов\n");
    }

    for (int i = 5; i < argc; i++) {
        if (argv[5][0] == '-')
            break;
        argv[4] += (" " + argv[5]);
        argv.erase(argv.begin() + 5);
    }

    std::map < std::string, int > options = {
            {"-sd", 1},
            {"--startdate", 1}
    };

    checkOptions(argv, argc, options, 5, false);



    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);

    std::regex num("[0-9]*");

    std::ifstream input("data.json");
    json data;

    if (!input.is_open()) {
        data["Expense"] = json::array();
    }
    else {
        if (!(input.peek() == std::ifstream::traits_type::eof())) {
            input >> data;
        }
    }
    input.close();

    for (int i = 0; i < data["Expense"].size(); i++) {
        if (data["Expense"][i]["name"] == argv[1]) {
            std::cout << "Покупка " << argv[1] << " уже существует!\nЕсли вы хотите изменить существующую покупку то используйте команду editExpense\n\n";
            return;
        }
    }
    

    json object;

    object["name"] = argv[1];

    if (regex_match(argv[2], num)) {
        object["price"] = atoi(argv[2].c_str());
    }
    else {
        throw error("Ошибка!\nСтоимость должна состоять только из цифр\n");
    }

    std::vector<std::string> date_vec{};
    splitLine(date_vec, argv[3], '.');
    if (date_vec.size() == 3 &&
        (date_vec[0].size() == 2 &&
            date_vec[1].size() == 2 &&
            date_vec[2].size() == 4) &&
        (regex_match(date_vec[0], num) &&
            regex_match(date_vec[1], num) &&
            regex_match(date_vec[2], num)) &&
        (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
            in_range(atoi(date_vec[1].c_str()), 1, 12)) &&
        ((atoi(date_vec[2].c_str()) - timeinfo->tm_year - 1900) * 12) + (atoi(date_vec[1].c_str()) - 1 - timeinfo->tm_mon) >= 1)
    {

        object["date"] = argv[3];
    }
    else {
        throw error("Ошибка!\nДата введена не правильно\nДата должна быть в будущем минимум на 1 месяц\nФормат даты:    dd.mm.yyyy\n");
    }

    object["description"] = argv[4];

    if ((std::find(++argv.begin(), argv.end(), "-sd") != argv.end()) || (std::find(++argv.begin(), argv.end(), "--startdate") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 5, argv.end(), "-sd") != argv.end())) {

            iter = std::find(argv.begin() + 5, argv.end(), "-sd");
        }
        else {
            iter = std::find(argv.begin() + 5, argv.end(), "--startdate");
        }
        if (iter == --argv.end()) {
            throw error("Ошибка!\nВведите дату начала накопления\n");
        }

        std::string start_date = *(++iter);

        date_vec.clear();
        splitLine(date_vec, start_date, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)))
        {

            object["start"] = start_date;
            
        }
        else {
            throw error("Ошибка!\nДата введена не правильно\nФормат даты:    dd.mm.yyyy\n");
        }
    }
    else {
        char time_line[11]{};
        strftime(time_line, 11, "%d.%m.%Y", timeinfo);
        object["start"] = time_line;
    }

    object["accumulated"] = 0;
    object["custom_coef"] = 0;

    data["Expense"].push_back(object);

    
    toFile("data.json", data);

    data = getFile("data.json");
    std::cout << "Покупка добавлена!\n";
    std::cout << "Название:\t\t" << (std::string)data["Expense"].back()["name"] << std::endl;
    std::cout << "Стоимость:\t\t" << data["Expense"].back()["price"] << std::endl;
    std::cout << "Дата покупки:\t\t" << (std::string)data["Expense"].back()["date"] << std::endl;
    std::cout << "Старт накопления:\t" << (std::string)data["Expense"].back()["start"] << std::endl;
    std::cout << "Описание:\t\t" << (std::string)data["Expense"].back()["description"] << std::endl << std::endl;
}

void editExpense(int argc, std::vector<std::string> argv) {
    if (argc == 1) {
        std::cout << "Редактирование покупки\n\neditExpense <name> [-n | --name] [-p | --price] [-t | --time] [-d | --description]\n\t\t   [-sd | --startdate] [-ac | --accumulated] <args> \n\nАргументы\n"
            << "\t<name>" << "\t\t\t\t\t\tНазвание покупки\n"
            << "\t<args>" << "\t\t\t\t\t\tАргументы опций\n"
            << "Опции\n"
            << "\t[-n | --name] <new name>" << "\t\t\tВся доступная информация\n"
            << "\t[-p | --price] <new price>" << "\t\t\tНовое название покупки\n"
            << "\t[-t | --time]  <new time>" << "\t\t\tНовая дата покупки\n"
            << "\t[-d | --description]  <new description>" << "\t\tНовое описание покупки\n"
            << "\t[-sd | --startdate]  <new startdate>" << "\t\tНовая дата начала накопления\n"
            << "\t[-ac | --accumulated]  <new accumulated>" << "\tНовая накопленная сумма\n"
            << "\t[-df | --difference]  <difference>" << "\t\tРазность между накопленным в приложении и реальностью.\n\n";
        return;
    }

    if (argc < 2 || argv[1][0] == '-') {
        throw error("Ошибка!\nНедостаточно аргументов\n");
    }

    std::map < std::string, int > options = {
            {"-p", 1},
            {"--price", 1},
            {"-t", 1},
            {"--time", 1},
            {"-d", 1},
            {"--description", 1},
            {"-sd", 1},
            {"--startdate", 1},
            {"-ac", 1},
            {"--accumulated", 1},
            {"-df", 1},
            {"--difference", 1},
            {"-n", 1},
            {"--name", 1},
    };

    checkOptions(argv, argc, options, 2, true);

    json data = getFile("data.json");

    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    std::regex num("[0-9-]*");
    std::regex pattern(R"(^-([a-zA-Z]+|--[a-zA-Z]+)$)");

    std::vector<std::string> cout{};

    if ((std::find(argv.begin(), argv.end(), "-p") != argv.end()) || (std::find(argv.begin(), argv.end(), "--price") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-p") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-p");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--price");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новую стоимость\n");
        }
        --iter;

        std::string new_price = *(++iter);

        if (!regex_match(new_price, num)) {
            throw error("Ошибка!\nСтоимость должна состоять только из цифр\n");
        }

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                int old_price = data["Expense"][i]["price"];
                data["Expense"][i]["price"] = atoi(new_price.c_str());
                cout.push_back("Стоимость изменена\n" + std::to_string(old_price) + " -> " + new_price);
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-t") != argv.end()) || (std::find(argv.begin(), argv.end(), "--time") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-t") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-t");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--time");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новую дату покупки\n");
        }
        --iter;

        std::string new_time = *(++iter);

        std::vector<std::string> date_vec{};
        splitLine(date_vec, new_time, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)) &&
            ((atoi(date_vec[2].c_str()) - timeinfo->tm_year - 1900) * 12) + (atoi(date_vec[1].c_str()) - 1 - timeinfo->tm_mon) >= 1)
        {

            for (int i = 0; i < data["Expense"].size(); i++) {
                if (data["Expense"][i]["name"] == argv[1]) {
                    std::string old_time = (std::string)data["Expense"][i]["date"];
                    data["Expense"][i]["date"] = new_time;
                    cout.push_back("Дата покупки изменена\n" + old_time + " -> " + new_time);
                    break;
                }
                else if (i == data["Expense"].size() - 1) {
                    throw error("Ошибка!\nТакой покупки не существует\n");
                }
                continue;
            }

        }
        else {
            throw error("Ошибка!\nДата введена не правильно\nДата должна быть в будущем минимум на 1 месяц\nФормат даты:    dd.mm.yyyy\n");
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-d") != argv.end()) || (std::find(argv.begin(), argv.end(), "--description") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-d") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-d");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--description");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новое описание\n");
        }
        --iter;

        std::vector<std::string>::iterator unite_iter = iter;

        while (unite_iter != --argv.end()) {
            if ((*(++unite_iter))[0] != '-') {
                *(--unite_iter) += " " + *(unite_iter);
                ++unite_iter;
            }
            else {
                break;
            }
        }

        std::string new_desc = *(++iter);

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                std::string old_desc = (std::string)data["Expense"][i]["description"];
                data["Expense"][i]["description"] = new_desc;
                cout.push_back("Описание изменено\n" + old_desc + " -> " + new_desc);
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-sd") != argv.end()) || (std::find(argv.begin(), argv.end(), "--startdate") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-sd") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-sd");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--startdate");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новую дату покупки\n");
        }
        --iter;

        std::string new_date = *(++iter);

        std::vector<std::string> date_vec{};
        splitLine(date_vec, new_date, '.');
        if (date_vec.size() == 3 &&
            (date_vec[0].size() == 2 &&
                date_vec[1].size() == 2 &&
                date_vec[2].size() == 4) &&
            (regex_match(date_vec[0], num) &&
                regex_match(date_vec[1], num) &&
                regex_match(date_vec[2], num)) &&
            (in_range(atoi(date_vec[0].c_str()), 1, 31) &&
                in_range(atoi(date_vec[1].c_str()), 1, 12)))
        {

            for (int i = 0; i < data["Expense"].size(); i++) {
                if (data["Expense"][i]["name"] == argv[1]) {
                    std::string old_date = (std::string)data["Expense"][i]["start"];
                    data["Expense"][i]["start"] = new_date;
                    cout.push_back("Дата начала накопления изменена\n" + old_date + " -> " + new_date);
                    break;
                }
                else if (i != data["Expense"].size() - 1) {
                    continue;
                }
                throw error("Ошибка!\nТакой покупки не существует\n");
            }

        }
        else {
            throw error("Ошибка!\nДата введена не правильно\nФормат даты:    dd.mm.yyyy\n");
        }

    }

    if ((std::find(argv.begin(), argv.end(), "-ac") != argv.end()) || (std::find(argv.begin(), argv.end(), "--accumulated") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-ac") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-ac");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--accumulated");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новое накопление\n");
        }
        --iter;

        std::string new_accumulated = *(++iter);

        if (!regex_match(new_accumulated, num)) {
            throw error("Ошибка!\nСумма накопления должна состоять только из цифр\n");
        }

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                int old_accumulated = (int)data["Expense"][i]["accumulated"];
                data["Expense"][i]["custom_coef"] = atoi(new_accumulated.c_str()) - (int)data["Expense"][i]["accumulated"];
                cout.push_back("Накопленная сумма изменена\n" + std::to_string(old_accumulated) + " -> " + std::to_string((int)data["Expense"][i]["accumulated"] + (int)data["Expense"][i]["custom_coef"]));
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-df") != argv.end()) || (std::find(argv.begin(), argv.end(), "--difference") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-df") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-df");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--difference");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите разницу\n");
        }
        --iter;

        std::string new_accumulated = *(++iter);

        if (!regex_match(new_accumulated, num)) {
            throw error("Ошибка!\nРазница должна состоять только из цифр\n");
        }

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                int old_accumulated = (int)data["Expense"][i]["accumulated"] + (int)data["Expense"][i]["custom_coef"];
                data["Expense"][i]["custom_coef"] = (int)data["Expense"][i]["custom_coef"] + atoi(new_accumulated.c_str());
                cout.push_back("Накопленная сумма изменена\n" + std::to_string(old_accumulated) + " -> " + std::to_string((int)data["Expense"][i]["accumulated"] + (int)data["Expense"][i]["custom_coef"]));
                break;
            }
            else if (i != data["Expense"].size() - 1) {
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    if ((std::find(argv.begin(), argv.end(), "-n") != argv.end()) || (std::find(argv.begin(), argv.end(), "--name") != argv.end())) {

        std::vector<std::string>::iterator iter = argv.begin();
        if ((std::find(argv.begin() + 2, argv.end(), "-n") != argv.end())) {

            iter = std::find(argv.begin() + 2, argv.end(), "-n");
        }
        else {
            iter = std::find(argv.begin() + 2, argv.end(), "--name");
        }
        if (iter == --argv.end() || std::regex_match((*(++iter)), pattern)) {
            throw error("Ошибка!\nВведите новое название\n");
        }
        --iter;

        std::string new_name = *(++iter);

        for (int i = 0; i < data["Expense"].size(); i++) {
            if (data["Expense"][i]["name"] == argv[1]) {
                data["Expense"][i]["name"] = new_name;
                cout.push_back("Покупка  успешно переименована\n" + argv[1] + " -> " + new_name);
                break;
            }
            else if (i != data["Expense"].size() - 1){
                continue;
            }
            throw error("Ошибка!\nТакой покупки не существует\n");
        }
    }

    toFile("data.json", data);

    std::cout << "Покупка " << argv[1] << " успешно изменена!\n\n";
    for (int i = 0; i < cout.size(); i++) {
        std::cout << cout[i] << std::endl;
    }

    std::cout << std::endl;
}

json memory_object = nullptr;
int memory_iter = NULL;

void delite(int argc, std::vector<std::string> argv) {
    if (argc == 1) {
        std::cout << "Удаление траты\n\ndelite <number>\n\nАргументы\n"
            << "\t<number>" << "\tНомер траты. Что бы узнать номер воспользуйтесь командой info -e\n\n";
        return;
    }

    if (argc < 2) {
        throw error("Ошибка!\nНедостаточно аргументов\n");
    }

    json data = getFile("data.json");

    std::regex num("[0-9]*");

    if (!regex_match(argv[1], num)) {
        throw error("Ошибка!\nНомер должен состоять только из цифр\n");
    }

    if (atoi(argv[1].c_str()) > data["Expense"].size() - 1 || atoi(argv[1].c_str()) < 0) {
        throw error("Ошибка!\nПокупки с таким номером не существует\n");
    }

    memory_object = data["Expense"][atoi(argv[1].c_str())];
    memory_iter = atoi(argv[1].c_str());

    data["Expense"].erase(atoi(argv[1].c_str()));

    toFile("data.json", data);

    std::cout << "Трата " << (std::string)memory_object["name"] << " успешно удалена!\nЕсли вы хотите вернуть трату то используйте команду remove\n\n";  
}

void removeObj(int argc, std::vector<std::string> argv) {

    if (memory_object == nullptr) {
        throw error("В памяти ничего не сохранено\n");
    }

    json data = getFile("data.json");

    json tmp;

    while (data["Expense"].size() != memory_iter) {
        tmp.push_back(data["Expense"].back());
        data["Expense"].erase(data["Expense"].size() - 1);
    }

    data["Expense"].push_back(memory_object);
    if (tmp.size() > 0) {
        int size = tmp.size();
        for (int i = 0; i < size; i++) {
            data["Expense"].push_back(tmp.back());
            tmp.erase(tmp.size() - 1);
        }
    }

    toFile("data.json", data);

    std::cout << "Трата " << (std::string)data["Expense"][memory_iter]["name"] << " успешно восстановлена!\n\n";

    memory_iter = NULL;
    memory_object = nullptr;
}