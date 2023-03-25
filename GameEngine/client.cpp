#include <deque>
#include <array>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <vector>
#include "protocol.hpp"
#include "../GameLogic/GameFactors.h"

using boost::asio::ip::tcp;

class client
{
public:
    client(const std::array<char, MAX_NICKNAME>& nickname,
            boost::asio::io_service& io_service,
            tcp::resolver::iterator endpoint_iterator) :
            io_service_(io_service), socket_(io_service)
    {

        strcpy(nickname_.data(), nickname.data());
        memset(read_msg_.data(), '\0', MAX_IP_PACK_SIZE);
        boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&client::onConnect, this, _1));
    }

    void write(const std::array<char, MAX_IP_PACK_SIZE>& msg)
    {
        io_service_.post(boost::bind(&client::writeImpl, this, msg));
    }

    void close()
    {
        io_service_.post(boost::bind(&client::closeImpl, this));
    }

private:

    void onConnect(const boost::system::error_code& error)
    {
        if (!error)
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(nickname_, nickname_.size()),
                                     boost::bind(&client::readHandler, this, _1));
        }
    }

    void readHandler(const boost::system::error_code& error)
    {
        //std::cout << read_msg_.data() << std::endl;
        if (!error)
        {
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg_, read_msg_.size()),
                                    boost::bind(&client::readHandler, this, _1));
        } else
        {
            closeImpl();
        }
    }

    void writeImpl(std::array<char, MAX_IP_PACK_SIZE> msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                     boost::bind(&client::writeHandler, this, _1));
        }
    }

    void writeHandler(const boost::system::error_code& error)
    {
        if (!error)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                boost::asio::async_write(socket_,
                                         boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                         boost::bind(&client::writeHandler, this, _1));
            }
        } else
        {
            closeImpl();
        }
    }

    void closeImpl()
    {
        socket_.close();
    }

    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    std::array<char, MAX_IP_PACK_SIZE> read_msg_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> write_msgs_;
    std::array<char, MAX_NICKNAME> nickname_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: ./chat_client <nickname> <host> <port>\n";
            return 1;
        }
        std::ifstream file;
        file.open("../player"+std::string(argv[1])+"/"+std::string(argv[1])+".txt");
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[2], argv[3]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        std::array<char, MAX_NICKNAME> nickname;
        strcpy(nickname.data(), argv[1]);

        client cli(nickname, io_service, iterator);

        std::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::array<char, MAX_IP_PACK_SIZE> msg;
        /*
        while (true)
        {
            memset(msg.data(), '\0', msg.size());
            if (!std::cin.getline(msg.data(), MAX_IP_PACK_SIZE - PADDING - MAX_NICKNAME))
            {
                std::cin.clear(); //clean up error bit and try to finish reading
            }
            cli.write(msg);
        }
        */
        if(file.is_open()){
            std::string msg;
            while(std::getline(file, msg)){
                sleep(1);
                char s[msg.length()+1];
                for(int i =0; i<msg.length(); i++){
                    s[i] = msg[i];
                }
                //std::cout<<s<<std::endl;
                char *c1 = strtok(s, ":");
                char *c2 = strtok(NULL, ":");
                char *c3 = strtok(NULL, ":");
                char *c4 = strtok(NULL, ":");
                int b = 0;
                std::vector<std::string> vec;
                  /*
                    for x: 
                        0 - food
                        1 - farmer
                        2 - engineer
                        3 - troop
                        4 - miner
                        5 - spy

                    for y
                        0 - ==
                        1 - <
                        2 - >
                */
                if(std::strcmp(c1,"loop") == 0){
                    char *a1 = new char();
                    char *a2 = new char();
                    char *a3 = new char();
                    if(c2 != NULL)
                        strcpy(a1, c2);
                    if(c3 != NULL)
                        strcpy(a2, c3);
                    if(c4 != NULL)
                        strcpy(a3, c4);
                    while(true){
                        std::getline(file, msg);
                        if(msg == "loop:"){
                            //std::cout<<"lol"<<std::endl;
                            break;
                        }
                        else if(std::strcmp(c1,"sleep")==0){
                            vec.push_back(msg);
                        }
                        else{
                            std::string l = msg;
                            msg = l;
                            vec.push_back(msg);
                        }
                    }
                    if(std::strcmp(a1,"troops") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].num_troops != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }std::vector<std::string> vec;
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].num_troops < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].num_troops > std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"farmers") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].num_farmers != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].num_farmers < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].num_farmers > std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"food") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].food != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }std::vector<std::string> vec;
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].food < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].food > std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"engineers") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].num_engineers != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }std::vector<std::string> vec;
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].num_engineers < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].num_engineers> std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"miners") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].num_miners != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }std::vector<std::string> vec;
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].num_miners < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].num_miners > std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"spies") == 0){
                        if(std::strcmp(a2,"!=") == 0){
                            while(arr[std::stoi(nickname.data())].num_spies != std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }std::vector<std::string> vec;
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,"<") == 0){
                            while(arr[std::stoi(nickname.data())].num_spies < std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                        else if(std::strcmp(a2,">") == 0){
                            while(arr[std::stoi(nickname.data())].num_spies > std::stoi(a3)){
                                for(unsigned int i = 0; i<vec.size(); i++){
                                    char t[vec[i].length()+1];
                                    for(int j =0; j<vec[i].length(); j++){
                                        t[j] = vec[i][j];
                                    }
                                    char *c6 = strtok(t,":");
                                    char *c7 = strtok(NULL,":");
                                    if(std::strcmp(c6,"sleep")==0){
                                        sleep(std::stof(c7));
                                    }
                                    std::array<char, 512UL> mesg;
                                    for(int j = 0; j<msg.length(); j++){
                                        mesg[j] = vec[i][j];
                                    }
                                    cli.write(mesg);
                                    sleep(1);
                                }
                            }
                        }
                    }
                    else if(std::strcmp(a1,"iter")==0){
                        std::cout<<"Hi"<<std::endl;
                        int l = 0;
                        while(l<std::stoi(a2)){
                            for(unsigned int i = 0; i<vec.size(); i++){
                                char t[vec[i].length()+1];
                                for(int j =0; j<vec[i].length(); j++){
                                    t[j] = vec[i][j];
                                }
                                char *c6 = strtok(t,":");
                                char *c7 = strtok(NULL,":");
                                if(std::strcmp(c6,"sleep")==0){
                                    sleep(std::stof(c7));
                                }
                                std::array<char, 512UL> mesg;
                                for(int j = 0; j<msg.length(); j++){
                                    mesg[j] = vec[i][j];
                                    std::cout<<mesg[j];
                                }
                                std::cout<<std::endl;
                                cli.write(mesg);
                                sleep(1);
                            }
                            l++;
                        }
                    }
                    else{
                        //std::cout<<vec.size()<<std::endl;
                        //std::cout<<vec[0]<<std::endl;
                        while(true){
                            for(unsigned int i = 0; i<vec.size(); i++){
                                char t[vec[i].length()+1];
                                for(int j =0; j<vec[i].length(); j++){
                                    t[j] = vec[i][j];
                                }
                                char *c6 = strtok(t,":");
                                char *c7 = strtok(NULL,":");
                                if(std::strcmp(c6,"sleep")==0){
                                    sleep(std::stof(c7));
                                }
                                std::array<char, 512UL> mesg;
                                for(int j = 0; j<msg.length(); j++){
                                    mesg[j] = vec[i][j];
                                }
                                cli.write(mesg);
                                sleep(1);
                            }
                        }
                    }
                }
                else if(std::strcmp(c1,"sleep")==0){
                    sleep(std::stof(c2));
                }
                else{
                    std::string l = msg;
                    msg = l;
                    std::array<char, 512UL> mesg;
                    for(int i = 0; i<msg.length(); i++){
                        mesg[i] = msg[i];
                    }
                    cli.write(mesg);
                    sleep(1);
                }
            }
        }
        //=======================================================
        /*
         FOR TESTING THE ABOVE while LOOP WILL BE CHANGED TO
            while(true){
                memset(msg.data(), '\0', msg.size());
                msg.data() = function1();
                cli.write(msg);
                msg.data() = function2();
                cli.write(msg);
                msg.data() = function3();
                cli.write(msg);
                //And so on
            }
        */
        //=======================================================
        
        while(true){

        }
        cli.close();
        t.join();
    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}