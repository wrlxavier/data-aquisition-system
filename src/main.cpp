/* 
Automação em Tempo Real (ATR)
Exercicio Computacional IV

Autores:
- Warley Fernandes Xavier da Silva
- Lucas Emanuel Lobo Costa
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

struct LogRecord {
    char sensor_id[32];
    std::time_t timestamp;
    double value;
};

class SensorSession : public std::enable_shared_from_this<SensorSession> {
public:
    SensorSession(tcp::socket socket, std::map<std::string, std::vector<LogRecord>>& sensor_data)
        : socket_(std::move(socket)), sensor_data_(sensor_data) {}

    void start() {
        read_message();
    }

private:
    void read_message() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, buffer_, "\r\n",
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string message(std::istreambuf_iterator<char>(is), {});
                    std::cout << "Mensagem recebida: " << message << std::endl;
                    handle_message(message);
                    read_message();
                }
            });
    }

    void handle_message(const std::string& message) {
        std::istringstream iss(message);
        std::string type;
        std::getline(iss, type, '|');

        if (type == "LOG") {
            std::cout << "Processando mensagem de log" << std::endl;
            std::string sensor_id, timestamp_str, value_str;
            std::getline(iss, sensor_id, '|');
            std::getline(iss, timestamp_str, '|');
            std::getline(iss, value_str, '\r');
            LogRecord record;

            std::strncpy(record.sensor_id, sensor_id.c_str(), sizeof(record.sensor_id) - 1);
            record.sensor_id[sizeof(record.sensor_id) - 1] = 0;
            record.timestamp = string_to_time_t(timestamp_str);
            record.value = std::stod(value_str);

            std::lock_guard<std::mutex> lock(mutex_);
            sensor_data_[sensor_id].emplace_back(record);

            write_to_file(sensor_id, record);
        }
        else if (type == "GET") {
            std::cout << "Processando solicitação de registros" << std::endl;
            std::string sensor_id, num_records_str;
            std::getline(iss, sensor_id, '|');
            std::getline(iss, num_records_str, '\r');
            int num_records = std::stoi(num_records_str);

            std::lock_guard<std::mutex> lock(mutex_);
            if (sensor_data_.find(sensor_id) == sensor_data_.end()) {
                std::string response = "ERROR|INVALID_SENSOR_ID\r\n";
                std::cout << "Erro: ID de sensor inválido - " << sensor_id << std::endl;
                boost::asio::write(socket_, boost::asio::buffer(response));
            } else {
                auto& records = sensor_data_[sensor_id];
                int total_records = records.size();
                int start_index = std::max(0, total_records - num_records);
                std::ostringstream response;
                response << total_records;
                for (int i = start_index; i < total_records; ++i) {
                    response << ";" << time_t_to_string(records[i].timestamp) << "|" << records[i].value;
                }
                response << "\r\n";
                boost::asio::write(socket_, boost::asio::buffer(response.str()));
            }
        }
    }

    void write_to_file(const std::string& sensor_id, const LogRecord& record) {
        std::ofstream file(sensor_id + ".log", std::ios::binary | std::ios::app);
        file.write(reinterpret_cast<const char*>(&record), sizeof(record));
    }

    std::time_t string_to_time_t(const std::string& time_string) {
        std::tm tm = {};
        std::istringstream ss(time_string);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        return std::mktime(&tm);
    }

    std::string time_t_to_string(std::time_t time) {
        std::tm* tm = std::localtime(&time);
        std::ostringstream ss;
        ss << std::put_time(tm, "%Y-%m-%dT%H:%M:%S");
        return ss.str();
    }

    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::vector<LogRecord> logs_;
    std::map<std::string, std::vector<LogRecord>>& sensor_data_;
    std::mutex mutex_;
};

class SensorServer {
public:
    SensorServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        accept();
    }

private:
    void accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "Nova conexão aceita: " << socket.remote_endpoint() << std::endl;
                    std::make_shared<SensorSession>(std::move(socket), sensor_data_)->start();
                }
                accept();
            });
    }

    tcp::acceptor acceptor_;
    std::map<std::string, std::vector<LogRecord>> sensor_data_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: sensor_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;
        SensorServer server(io_context, std::atoi(argv[1]));
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}