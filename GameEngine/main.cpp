#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <string>
#include <deque>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <array>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <set>
#include <utility>
#include "protocol.hpp"
#include "../GameLogic/GameFactors.h"
#include <cstring>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <cstdlib>
using boost::asio::ip::tcp;
int lost_arr[4] = {0,0,0,0};
int player_map[4] = {0,0,0,0};
int player_map_inverse[4] = {5,5,5,5};
int number_of_players=0;
int player_max = 4;

// create the window
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "My window");

struct coord{
    int x, y;
    coord(int x, int y){
        this->x=x;
        this->y=y;
    }
};
std::vector <coord> troopCoords1;
std::vector <coord> troopCoords2;
std::vector <coord> troopCoords3;
std::vector <coord> troopCoords4;
std::vector <coord> engineerCoords1;
std::vector <coord> engineerCoords2;
std::vector <coord> engineerCoords3;
std::vector <coord> engineerCoords4;
std::vector <coord> farmerCoords1;
std::vector <coord> farmerCoords2;
std::vector <coord> farmerCoords3;
std::vector <coord> farmerCoords4;
std::vector <coord> minerCoords1;
std::vector <coord> minerCoords2;
std::vector <coord> minerCoords3;
std::vector <coord> minerCoords4;
std::vector <coord> spyCoords1;
std::vector <coord> spyCoords2;
std::vector <coord> spyCoords3;
std::vector <coord> spyCoords4;

/// @brief Creates and handles walls
class Walls{

    sf::Texture* texture;
    sf::RenderWindow* window;
    sf::Sprite* wall= new sf::Sprite[4];
    bool* showWall = new bool [4];

    public:

    /// @brief Constructor for the walls
    /// @param tex Texture of the walls
    /// @param Win Rendering window
    Walls(sf::Texture* tex, sf::RenderWindow *Win){
        texture = tex;
        window = Win;
        for ( int i = 0; i < 4 ; i++ ){
            wall[i].setTexture(*texture);
            wall[i].scale(0.5, 0.06);
            wall[i].setPosition(500,500);
        }
        wall[0].rotate(180);
        wall[1].rotate(90);
        wall[2].rotate(270);
    };

    /// @brief To create or destroy multiple walls, pass a boolean array
    /// @param arr array of size 4, true means walls should be present, false means it is absent. 0-Left, 1-Top, 2-Bottom, 3-Right
    void modifyWalls(bool *arr){
        for ( int i = 0 ; i < 4 ; i++ ){
            showWall[i] = arr[i];
        }
    }

    /// @brief Deletes the walls from the array, wallNumber is the array index of that wall.
    /// @param wallNumber Array index of wall.  0-Left, 1-Top, 2-Bottom, 3-Right
    void destroyWall(int wallNumber){
        showWall[wallNumber]=false;
    }

    /// @brief Creates the walls from the array, wallNumber is the array index of that wall.
    /// @param wallNumber Array index of wall.  0-Left, 1-Top, 2-Bottom, 3-Right
    void createWall(int wallNumber){
        showWall[wallNumber]=true;
    }

    /// @brief Draws the walls to the buffer. Window.display() needs to be called seperately
    void drawWalls(){
        for ( int i = 0 ; i < 4 ; i++ ){
            if ( showWall[i] ){
                window->draw(wall[i]);
            }
        }
    }   

};


class Grass{

    sf::Texture* grassTexture;
    sf::Texture* defenceTexture;
    sf::Texture* attackTexture;
    sf::RenderWindow* window;

    sf::Sprite grass;
    sf::Sprite* attackSprite= new sf::Sprite[4];
    sf::Sprite* defenceSprite = new sf::Sprite[4];
    //0 - no war, 1 - attacking terriroty, 2 - defensive territory
    int* warCondition = new int [4];

    public:
    Grass(sf::Texture* normal, sf::Texture *attack, sf::Texture *defence, sf::RenderWindow* win){
        grassTexture = normal;
        defenceTexture = defence;
        defenceTexture->setRepeated(true);
        attackTexture = attack;
        attackTexture->setRepeated(true);
        window = win;
        grass.setTexture(*normal);
        for ( int i = 0 ; i < 4 ; i++ ){
            attackSprite[i].setTexture(*attack);
            attackSprite[i].scale(500/24.0,500/24.0);
            attackSprite[i].setColor(sf::Color(200,200,200,100));
            defenceSprite[i].setTexture(*defence);
            defenceSprite[i].scale(500/24.0,500/24.0);
            defenceSprite[i].setColor(sf::Color(200,100,100,100));
        }
        attackSprite[0].setPosition(0,0);
        attackSprite[1].setPosition(500,0);
        attackSprite[2].setPosition(0,500);
        attackSprite[3].setPosition(500,500);
        defenceSprite[0].setPosition(0,0);
        defenceSprite[1].setPosition(500,0);
        defenceSprite[2].setPosition(0,500);
        defenceSprite[3].setPosition(500,500);
    }

    /// @brief update the war conditions as necessary
    /// @param index index of the territory
    /// @param value 0-no war, 1-attack, 2-defence
    void updateWarCondition(int index , int value){
        warCondition[index]=value;
    }

    /// @brief Draws the walls to the buffer. Window.display() needs to be called seperately
    void drawGrass(){
        window->draw(grass);
        for ( int i = 0 ; i < 4 ; i++ ){
            if ( warCondition[i] == 1 ){
                window->draw(attackSprite[i]);
            }
        }
        for ( int i = 0 ; i < 4 ; i++ ){
            if ( warCondition[i] == 2){
                window->draw(defenceSprite[i]);
            }
        }
    }
};

class Regions{

    public:
    sf::Texture* redTexture;
    sf::Texture* yellowTexture;
    sf::RenderWindow* window;
    sf::Sprite* redSprite = new sf::Sprite();
    sf::Sprite* yellowSprite = new sf::Sprite();
    int *colors = new int[400];

    Regions(sf::Texture* redTexture, sf::Texture *yellowTexture, sf::RenderWindow* win){
        window = win;
        this->redTexture = redTexture;
        this->yellowTexture = yellowTexture;
        yellowSprite->setTexture(*yellowTexture);
        yellowSprite->scale(50.0/24, 50.0/24);
        yellowSprite->setColor(sf::Color(255,255,10,130));
        redSprite->setTexture(*yellowTexture);
        redSprite->scale(50.0/24, 50.0/24);
        redSprite->setColor(sf::Color(255,0,0,60));
    }

    /// @brief update the colors array as necessary
    /// @param index index of the territory
    /// @param value 0-no war, 1-yellow , 2 - red
    void updateColors(int x, int y , int value){
        colors[20*x+y] = value;
    }

    /// @brief Draws the walls to the buffer. Window.display() needs to be called seperately
    void drawShades(){
        for ( int i = 0 ; i < 400 ; i++ ){
            if ( colors[i] == 1 ){
                redSprite->setPosition( 50*(i/20) , 50*(i%20) );
                window->draw(*redSprite);
            }
            else if ( colors[i] == 2 ){
                yellowSprite->setPosition ( 50*(i/20) , 50*(i%20)  );
                window->draw(*yellowSprite);
            }
        }
    }
};

class Troop{
    int warState;
    sf::RenderWindow *window;
    sf::Texture *attackTexture;
    sf::Texture *defenceTexture;
    sf::Sprite* attackSprite = new sf::Sprite();
    sf::Sprite* defenceSprite = new sf::Sprite();
    
    public:
    Troop(sf::RenderWindow *win, sf::Texture *attackTexture, sf::Texture *defenceTexture){
        warState = 0;
        this->attackTexture = attackTexture;
        this->defenceTexture = defenceTexture;
        this->window = win;
        attackSprite->setTexture(*attackTexture);
        attackSprite->setScale( 80.0/278 , 60.0/181 );
        defenceSprite->setTexture(*defenceTexture);
        defenceSprite->setScale( 50.0/210 , 60.0/240 );
    }

    void changeWarState(int warState){
        this->warState = warState;
    }

    /// @brief x and y are not exact coordinates. Assume that the mapp is divided into 20x20 square
    /// @param x from 0 to 19
    /// @param y from 0 to 19
    void drawTroop(int x, int y){
        if ( warState == 1 ){
            //std::cout<<"drawing troop"<<std::endl;
            attackSprite->setPosition(x*50,y*50);
            window->draw(*attackSprite);
        }
        else if( warState == -1 ){
            defenceSprite->setPosition(x*50,y*50);
            window->draw(*defenceSprite);
        }
    }
};

class Farmer{
    sf::RenderWindow *window;
    sf::Texture *Texture;
    sf::Sprite* sprite = new sf::Sprite();
    
    public:
    Farmer(sf::RenderWindow *win, sf::Texture *Texture){
        this->Texture = Texture;
        this->window = win;
        sprite->setTexture(*Texture);
        sprite->setScale( 40.0/182 , 50.0/277 );
        //std::cout << "here\n";
    }

    /// @brief x and y are not exact coordinates. Assume that the mapp is divided into 20x20 square
    /// @param x from 0 to 19
    /// @param y from 0 to 19
    void drawFarmer(int x, int y){
        sprite->setPosition(x*50,y*50);
        window->draw(*sprite);
    }

};

class Engineer{
    sf::RenderWindow *window;
    sf::Texture *Texture;
    sf::Sprite* sprite = new sf::Sprite();
    
    public:
    Engineer(sf::RenderWindow *win, sf::Texture *Texture){
        this->Texture = Texture;
        this->window = win;
        sprite->setTexture(*Texture);
        sprite->setScale( 40.0/182 , 50.0/277 );
        //std::cout << "here\n";
    }

    /// @brief x and y are not exact coordinates. Assume that the mapp is divided into 20x20 square
    /// @param x from 0 to 19
    /// @param y from 0 to 19
    void drawEngineer(int x, int y){
        sprite->setPosition(x*50,y*50);
        window->draw(*sprite);
    }

};

class Miner{
    sf::RenderWindow *window;
    sf::Texture *Texture;
    sf::Sprite* sprite = new sf::Sprite();
    
    public:
    Miner(sf::RenderWindow *win, sf::Texture *Texture){
        this->Texture = Texture;
        this->window = win;
        sprite->setTexture(*Texture);
        sprite->setScale( 40.0/182 , 50.0/277 );
        //std::cout << "here\n";
    }

    /// @brief x and y are not exact coordinates. Assume that the mapp is divided into 20x20 square
    /// @param x from 0 to 19
    /// @param y from 0 to 19
    void drawMiner(int x, int y){
        sprite->setPosition(x*50,y*50);
        window->draw(*sprite);
    }

};

class Spy{
    sf::RenderWindow *window;
    sf::Texture *Texture;
    sf::Sprite* sprite = new sf::Sprite();
    
    public:
    Spy(sf::RenderWindow *win, sf::Texture *Texture){
        this->Texture = Texture;
        this->window = win;
        sprite->setTexture(*Texture);
        sprite->setScale( 40.0/182 , 50.0/277 );
        //std::cout << "here\n";
    }

    /// @brief x and y are not exact coordinates. Assume that the mapp is divided into 20x20 square
    /// @param x from 0 to 19
    /// @param y from 0 to 19
    void drawSpy(int x, int y){
        sprite->setPosition(x*50,y*50);
        window->draw(*sprite);
    }

};


namespace
{
std::string getTimestamp()
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    std::stringstream ss;
    ss << '[' << (now->tm_year + 1900) << '-' << std::setfill('0')
       << std::setw(2) << (now->tm_mon + 1) << '-' << std::setfill('0')
       << std::setw(2) << now->tm_mday << ' ' << std::setfill('0')
       << std::setw(2) << now->tm_hour << ":" << std::setfill('0')
       << std::setw(2) << now->tm_min << ":" << std::setfill('0')
       << std::setw(2) << now->tm_sec << "] ";

    return ss.str();
}

class workerThread
{
public:
    static void run(std::shared_ptr<boost::asio::io_service> io_service)
    {
        {
            std::lock_guard < std::mutex > lock(m);
            std::cout << "[" << std::this_thread::get_id() << "] Thread starts" << std::endl;
        }

        io_service->run();

        {
            std::lock_guard < std::mutex > lock(m);
            std::cout << "[" << std::this_thread::get_id() << "] Thread ends" << std::endl;
        }

    }
private:
    static std::mutex m;
};

std::mutex workerThread::m;
}

class participant
{
public:
    virtual ~participant() {}
    virtual void onMessage(std::array<char, MAX_IP_PACK_SIZE> & msg) = 0;
    Kingdom k;
    bool lost = false;
    int num;
};

class chatRoom {
public:
    void enter(std::shared_ptr<participant> participant, const std::string & nickname)
    {
        participants_.insert(participant);
        name_table_[participant] = nickname;
        if(number_of_players<=4){
            arr[std::stoi(nickname.data())] = participant->k;
            player_map[number_of_players] = std::stoi(nickname.data());
            //std::cout<<"Player map "<<number_of_players<<" is "<<std::stoi(nickname.data())<<std::endl;
            player_map_inverse[std::stoi(nickname.data())] = number_of_players;
            //std::cout<<"Here"<<std::endl;
            participant->num=std::stoi(nickname.data());
            arr[std::stoi(nickname.data())].num = std::stoi(nickname.data());
            //arr[number_of_players].in = true;
            //std::cout<<"wow"<<std::endl;
            number_of_players++;
        }
        else{
            std::cout<<"Not enough space for participants"<<std::endl;
            return;
        }
        std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                      boost::bind(&participant::onMessage, participant, _1));
    }

    void leave(std::shared_ptr<participant> participant)
    {
        participants_.erase(participant);
        name_table_.erase(participant);
    }

    void broadcast(std::array<char, MAX_IP_PACK_SIZE>& msg, std::shared_ptr<participant> participant)
    {
        std::string timestamp = getTimestamp();
        std::string nickname = getNickname(participant);
        std::array<char, MAX_IP_PACK_SIZE> formatted_msg;

        // boundary correctness is guarded by protocol.hpp
        strcpy(formatted_msg.data(), timestamp.c_str());
        strcat(formatted_msg.data(), nickname.c_str());
        strcat(formatted_msg.data(), msg.data());

        recent_msgs_.push_back(formatted_msg);
        while (recent_msgs_.size() > max_recent_msgs)
        {
            recent_msgs_.pop_front();
        }

        std::for_each(participants_.begin(), participants_.end(),
                      boost::bind(&participant::onMessage, _1, std::ref(formatted_msg)));
    }

    std::string getNickname(std::shared_ptr<participant> participant)
    {
        return name_table_[participant];
    }

private:
    enum { max_recent_msgs = 100 };
    std::unordered_set<std::shared_ptr<participant>> participants_;
    std::unordered_map<std::shared_ptr<participant>, std::string> name_table_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE>> recent_msgs_;
};

class personInRoom: public participant,
                    public std::enable_shared_from_this<personInRoom>
{
public:
    personInRoom(boost::asio::io_service& io_service,
                 boost::asio::io_service::strand& strand, chatRoom& room)
                 : socket_(io_service), strand_(strand), room_(room)
    {
    }

    tcp::socket& socket() { return socket_; }

    void start()
    {
        boost::asio::async_read(socket_,
                                boost::asio::buffer(nickname_, nickname_.size()),
                                strand_.wrap(boost::bind(&personInRoom::nicknameHandler, shared_from_this(), _1)));
    }

    void onMessage(std::array<char, MAX_IP_PACK_SIZE>& msg)
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(socket_,
                                     boost::asio::buffer(write_msgs_.front(), write_msgs_.front().size()),
                                     strand_.wrap(boost::bind(&personInRoom::writeHandler, shared_from_this(), _1)));
        }
    }

private:
    void nicknameHandler(const boost::system::error_code& error)
    {
        if (strlen(nickname_.data()) <= MAX_NICKNAME - 2)
        {
            strcat(nickname_.data(), ": ");
        }
        else
        {
            //cut off nickname if too long
            nickname_[MAX_NICKNAME - 2] = ':';
            nickname_[MAX_NICKNAME - 1] = ' ';
        }

        room_.enter(shared_from_this(), std::string(nickname_.data()));

        boost::asio::async_read(socket_,
                                boost::asio::buffer(read_msg_, read_msg_.size()),
                                strand_.wrap(boost::bind(&personInRoom::readHandler, shared_from_this(), _1)));
    }

    void readHandler(const boost::system::error_code& error)
    {
        //if(number_of_players == player_max){
            if (!error)
            {
                room_.broadcast(read_msg_, shared_from_this());
                //std::cout<<"Shared from this "<<shared_from_this()->num<<std::endl;
                char s[100];
                int k=0;
                while(read_msg_[k] != '\0'){
                    s[k]= read_msg_[k];
                    k++;
                }
                //std::cout<<s<<std::endl;
                char *c1 = strtok(s, ":");
                char *c2 =strtok(NULL, ":");
                char *c3 = strtok(NULL, ":");
                char *c4 = strtok(NULL, ":");
                //std::cout<<std::strcmp(c1,"rt")<<std::endl;
                //std::cout<<c2<<std::endl;
                //std::cout<<c1<<std::endl;

                //if the message sent is "pf:3:10" then player wants to produce food worth land area 3 and 10 farmers
                //if(c1 == "pf"){
                //    shared_from_this()->k.produce_food(std::stoi(c2), std::stoi(c3));
                //}
                //if(c1 == "pm"){
                //    shared_from_this()->k.produce_material(std::stoi(c2), std::stoi(c3));
                //}

                if(!arr[shared_from_this()->num].lost && number_of_players == player_max){
                    if(std::strcmp(c1,"rf") == 0){
                        bool x = arr[shared_from_this()->num].recruit_farmers(std::stoi(c2));
                        if(x){
                            if(shared_from_this()->num == 0)
                                farmerCoords1.push_back(coord(rand()%2 +5, rand()%8 + 1));
                            else if(shared_from_this()->num == 1)
                                farmerCoords2.push_back(coord((rand()%2)+15, rand()%8 + 1));
                            else if(shared_from_this()->num == 2)
                                farmerCoords3.push_back(coord(rand()%2 + 5, (rand()%8)+11));
                            else if(shared_from_this()->num == 3)
                                farmerCoords4.push_back(coord((rand()%2 + 15), (rand()%8)+11));
                        }
                    }
                    else if(std::strcmp(c1,"re") == 0){
                        bool x = arr[shared_from_this()->num].recruit_engineers(std::stoi(c2));
                        if(x){
                            if(shared_from_this()->num == 0)
                                engineerCoords1.push_back(coord(rand()%2 +1, rand()%8 + 1));
                            else if(shared_from_this()->num == 1)
                                engineerCoords2.push_back(coord((rand()%2)+11, rand()%8 + 1));
                            else if(shared_from_this()->num == 2)
                                engineerCoords3.push_back(coord(rand()%2 + 1, (rand()%8)+11));
                            else if(shared_from_this()->num == 3)
                                engineerCoords4.push_back(coord((rand()%2 + 11), (rand()%8)+11));  
                        }
                    }
                    else if(std::strcmp(c1,"rt") == 0){
                        //std::cout<<"Here"<<std::endl;
                        bool x = arr[shared_from_this()->num].recruit_troops(std::stoi(c2));
                        if(x){
                            if(shared_from_this()->num == 0)
                                troopCoords1.push_back(coord(rand()%2 +7, rand()%8 + 1));
                            else if(shared_from_this()->num == 1)
                                troopCoords2.push_back(coord((rand()%2)+17, rand()%8 + 1));
                            else if(shared_from_this()->num == 2)
                                troopCoords3.push_back(coord(rand()%2 + 7, (rand()%8)+11));
                            else if(shared_from_this()->num == 3)
                                troopCoords4.push_back(coord((rand()%2 + 17), (rand()%8)+11));
                        }
                    }
                    else if(std::strcmp(c1,"rm") == 0){
                        bool x = arr[shared_from_this()->num].recruit_miners(std::stoi(c2));
                        if(x){
                            if(shared_from_this()->num == 0)
                                minerCoords1.push_back(coord(rand()%2 +3, rand()%8 + 1));
                            else if(shared_from_this()->num == 1)
                                minerCoords2.push_back(coord((rand()%2)+13, rand()%8 + 1));
                            else if(shared_from_this()->num == 2)
                                minerCoords3.push_back(coord(rand()%2 + 3, (rand()%8)+11));
                            else if(shared_from_this()->num == 3)
                                minerCoords4.push_back(coord((rand()%2 + 13), (rand()%8)+11));
                        }
                    }
                    else if(std::strcmp(c1,"rs") == 0){
                        bool x = arr[shared_from_this()->num].recruit_spies(std::stoi(c2));
                        if(x){
                            if(shared_from_this()->num == 0)
                                spyCoords1.push_back(coord(rand()%2 +5, rand()%8 + 1));
                            else if(shared_from_this()->num == 1)
                                spyCoords2.push_back(coord((rand()%2)+15, rand()%8 + 1));
                            else if(shared_from_this()->num == 2)
                                spyCoords3.push_back(coord(rand()%2 + 5, (rand()%8)+11));
                            else if(shared_from_this()->num == 3)
                                spyCoords4.push_back(coord((rand()%2 + 15), (rand()%8)+11));
                        }
                    }
                    else if(std::strcmp(c1,"ia") == 0){
                        arr[shared_from_this()->num].improve_attack(std::stof(c2));
                    }
                    else if(std::strcmp(c1,"id") == 0){
                        arr[shared_from_this()->num].improve_defense(std::stof(c2));
                    }
                    /*
                    else if(std::strcmp(c1,"sa") == 0){
                        shared_from_this()->k.square_assign(std::stoi(c2), std::stoi(c3), *c4);
                    }
                    */
                    else if(std::strcmp(c1,"sc") == 0){
                        int z = std::stoi(c2);
                        int i = 0;
                        int check = 0;
                        while(arr[z].lost || z == shared_from_this()->num || player_map_inverse[z] > number_of_players){
                            z=(z+1)%number_of_players;
                            i++;
                            if(i==number_of_players){
                                check = 1;
                                break;
                            }
                        }
                        if(check == 0)
                        arr[shared_from_this()->num].spy_capture(&arr[std::stoi(c2)], std::stoi(c3));
                    }
                    else if(std::strcmp(c1, "at") == 0){
                        int z = std::stoi(c2);
                        int i = 0;
                        int check = 0;
                        while(arr[player_map[z]].lost || player_map[z] == shared_from_this()->num || z >= number_of_players){
                            z=(z+1)%number_of_players;
                            i++;
                            if(i==number_of_players){
                                check = 1;
                                //std::cout<<"Break"<<std::endl;
                                break;
                            }
                        }
                       //std::cout<<shared_from_this()->num<<" "<<z<<std::endl;
                        if(check == 0){
                            int x = arr[shared_from_this()->num].attack(&(arr[player_map[z]]), std::stoi(c3));

                            if(x>100){
                                x=100;
                            }
                            if(x>0){
                                if(shared_from_this()->num == 0)
                                    for(int l = 0; l< troopCoords1.size()*(x/100); l++){
                                        troopCoords1.erase(troopCoords1.begin());
                                    }
                                else if(shared_from_this()->num == 1)
                                    for(int l = 0; l< troopCoords2.size()*(x/100); l++){
                                        troopCoords2.erase(troopCoords2.begin());
                                    }
                                else if(shared_from_this()->num == 2)
                                    for(int l = 0; l< troopCoords3.size()*(x/100); l++){
                                        troopCoords3.erase(troopCoords3.begin());
                                    }
                                else if(shared_from_this()->num == 3)
                                    for(int l = 0; l< troopCoords4.size()*(x/100); l++){
                                        troopCoords4.erase(troopCoords4.begin());
                                    }   
                            }  
                        }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                    }
                }
                else{
                    room_.leave(shared_from_this());
                }

                boost::asio::async_read(socket_,
                                        boost::asio::buffer(read_msg_, read_msg_.size()),
                                        strand_.wrap(boost::bind(&personInRoom::readHandler, shared_from_this(), _1)));
            }
        //}
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
                                         strand_.wrap(boost::bind(&personInRoom::writeHandler, shared_from_this(), _1)));
            }
        }
        else
        {
            room_.leave(shared_from_this());
        }
    }

    tcp::socket socket_;
    boost::asio::io_service::strand& strand_;
    chatRoom& room_;
    std::array<char, MAX_NICKNAME> nickname_;
    std::array<char, MAX_IP_PACK_SIZE> read_msg_;
    std::deque<std::array<char, MAX_IP_PACK_SIZE> > write_msgs_;
};

class server
{
public:
    server(boost::asio::io_service& io_service,
           boost::asio::io_service::strand& strand,
           const tcp::endpoint& endpoint)
           : io_service_(io_service), strand_(strand), acceptor_(io_service, endpoint)
    {
        run();
    }

private:

    void run()
    {
        std::shared_ptr<personInRoom> new_participant(new personInRoom(io_service_, strand_, room_));
        acceptor_.async_accept(new_participant->socket(), strand_.wrap(boost::bind(&server::onAccept, this, new_participant, _1)));
    }

    void onAccept(std::shared_ptr<personInRoom> new_participant, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_participant->start();
        }

        run();
    }

    boost::asio::io_service& io_service_;
    boost::asio::io_service::strand& strand_;
    tcp::acceptor acceptor_;
    chatRoom room_;
};
int counter1 = 0;
int win = 0;
void winLoop(){
        while(true){
            if(number_of_players == player_max){
                int counter = 0;
                for(int i = 0; i<number_of_players; i++){
                    if(arr[player_map[i]].lost){
                        counter++;
                    }
                    else{
                        win = arr[player_map[i]].num;
                        //std::cout<<"win "<<win<<std::endl;
                    }
                }
                if(counter == number_of_players - 1 && counter1 == 0){
                    std::cout<<"Player "<<win<<" has won!!"<<std::endl;
                    counter1 = 1;
                }
            }
        }
}

int gameLoop(){
    while(true){
        for(int p =0; p<number_of_players; p++){
            if(!arr[player_map[p]].lost){
                if(arr[player_map[p]].food <=0){
                    arr[player_map[p]].food = 0;
                    if(arr[player_map[p]].num_farmers >1){
                        arr[player_map[p]].num_farmers -= 1;
                    }
                    if(arr[player_map[p]].num_troops >1){
                        arr[player_map[p]].num_troops -= 1;
                    }
                    if(arr[player_map[p]].num_spies >1){
                        arr[player_map[p]].num_spies -= 1;
                    }
                    if(arr[player_map[p]].num_miners >1){
                        arr[player_map[p]].num_miners -= 1;
                    }
                    if(arr[player_map[p]].num_engineers >1){
                        arr[player_map[p]].num_engineers -= 1;
                    }
                }
                arr[player_map[p]].food -= 0.1*(arr[player_map[p]].num_troops + arr[player_map[p]].num_miners - 2*(arr[player_map[p]].num_farmers)) - 0.01*(arr[player_map[p]].land_area);
                arr[player_map[p]].raw_materials -= 0.1*(arr[player_map[p]].num_troops - arr[player_map[p]].num_miners - 2*(arr[player_map[p]].num_farmers)) - 0.01*(arr[player_map[p]].land_area);
            }
        }
        //std::cout<<"Player1 food:"<<arr[0].food<<std::endl;
        //std::cout<<"Player2 food:"<<arr[1].food<<std::endl;
        //std::cout<<"Player3 food:"<<arr[2].food<<std::endl;
        //std::cout<<"Player4 food:"<<arr[3].food<<std::endl;
        //std::cout<<std::endl;
        //std::cout<<"Player1 raw_m:"<<arr[0].raw_materials<<std::endl;
        //std::cout<<"Player2 raw_m:"<<arr[1].raw_materials<<std::endl;
        //std::cout<<"Player3 raw_m:"<<arr[2].raw_materials<<std::endl;
        //std::cout<<"Player4 raw_m:"<<arr[3].raw_materials<<std::endl;
        //std::cout<<std::endl;
        //std::cout<<std::endl;
        //std::cout<<"Main loop runs every one second"<<std::endl;
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
}


int main(int argc, char * argv[])
{
    if (argc < 3)
        {
            std::cerr << "Usage: main <num_players> <port> [<port> ...]\n";
            return 1;
        }
    player_max = std::stoi(argv[1]);
    //Load all the Textures
    
    //Grass Texture
    sf::Texture grassTexture;
    if (!grassTexture.loadFromFile("../Graphics/grass3.jpg")){std::cout << "Error Occurred!! Image could not be loaded!\n";}
    grassTexture.setRepeated(true);

    //Stone Texture
    sf::Texture stoneTexture;
    if (!stoneTexture.loadFromFile("../Graphics/stone.jpg")){std::cout << "Error Occurred!! Image could not be loaded!\n";}
    
    //Defence Texture
    sf::Texture defenceTexture;
    if (!defenceTexture.loadFromFile("../Graphics/defence.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //Attack Texture
    sf::Texture attackTexture;
    if (!attackTexture.loadFromFile("../Graphics/attack.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //Red Texture
    sf::Texture redTexture;
    if (!redTexture.loadFromFile("../Graphics/red.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //Yellow Texture
    sf::Texture yellowTexture;
    if (!yellowTexture.loadFromFile("../Graphics/yellow.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //AttackTroop Texture
    sf::Texture attackTroop;
    if (!attackTroop.loadFromFile("../Graphics/attackTroop.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}
    
    //DefenceTroop Texture
    sf::Texture defenceTroop;
    if (!defenceTroop.loadFromFile("../Graphics/defenceTroop.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //Spy Texture
    sf::Texture spyTexture;
    if (!spyTexture.loadFromFile("../Graphics/spy.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    sf::Texture farmerTexture;
    if (!farmerTexture.loadFromFile("../Graphics/farmer1.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    sf::Texture engineerTexture;
    if (!engineerTexture.loadFromFile("../Graphics/engineer.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    sf::Texture minerTexture;
    if (!minerTexture.loadFromFile("../Graphics/miner1.png")){std::cout << "Error Occurred!! Image could not be loaded!\n";}

    //Grass is initialized
    Grass* grass = new Grass(&grassTexture,&attackTexture,&defenceTexture,&window);

    //Walls are initialized
    Walls* walls = new Walls(&stoneTexture,&window);
    walls->createWall(0);
    walls->createWall(1);
    walls->createWall(2);
    walls->createWall(3);

    //Troops are initialized
    Troop* troop = new Troop(&window,&attackTroop,&defenceTroop);
    troop->changeWarState(1);
    Troop* troop2 = new Troop(&window,&attackTroop,&defenceTroop);
    troop2->changeWarState(-1);

    //Regions Initialized
    Regions* region = new Regions(&redTexture,&yellowTexture,&window);
    region->updateColors(5,3,1);
    region->updateColors(4,5,2);

    //Spies initialized
    Spy* spy1 = new Spy(&window,&spyTexture);

    Farmer* farmer2 = new Farmer(&window,&farmerTexture);

    Engineer* engineer2 = new Engineer(&window,&engineerTexture);

    Miner* miner2 = new Miner(&window,&minerTexture);

    // run the program as long as the window is open

        std::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
        boost::shared_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(*io_service));
        boost::shared_ptr<boost::asio::io_service::strand> strand(new boost::asio::io_service::strand(*io_service));
        boost::asio::deadline_timer timer(*io_service, boost::posix_time::seconds(2));
        std::cout << "[" << std::this_thread::get_id() << "]" << "server starts" << std::endl;

        std::list < std::shared_ptr < server >> servers;
        for (int i = 2; i < argc; ++i)
        {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            std::shared_ptr<server> a_server(new server(*io_service, *strand, endpoint));
            servers.push_back(a_server);
        }

        boost::thread_group workers;
        for (int i = 0; i < 1; ++i)
        {
            boost::thread * t = new boost::thread{ boost::bind(&workerThread::run, io_service) };

#ifdef __linux__
            // bind cpu affinity for worker thread in linux
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            pthread_setaffinity_np(t->native_handle(), sizeof(cpu_set_t), &cpuset);
#endif
            workers.add_thread(t);
        }
        
        boost::thread t1{gameLoop};
        boost::thread t2{winLoop};
        
    //troopCoords.push_back(coord(12, 6));
    //spyCoords.push_back(coord(4, 4));
    try{
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        grass->drawGrass();
        walls->drawWalls();
        for(unsigned int o = 0; o<spyCoords1.size(); o++)
            spy1->drawSpy(spyCoords1[o].x, spyCoords1[o].y);
        for(unsigned int o = 0; o<spyCoords2.size(); o++)
            spy1->drawSpy(spyCoords2[o].x, spyCoords2[o].y);
        for(unsigned int o = 0; o<spyCoords3.size(); o++)
            spy1->drawSpy(spyCoords3[o].x, spyCoords3[o].y);
        for(unsigned int o = 0; o<spyCoords4.size(); o++)
            spy1->drawSpy(spyCoords4[o].x, spyCoords4[o].y);
        for(unsigned int o = 0; o<troopCoords1.size(); o++)
            troop2->drawTroop(troopCoords1[o].x, troopCoords1[o].y);
        for(unsigned int o = 0; o<troopCoords2.size(); o++)
            troop2->drawTroop(troopCoords2[o].x, troopCoords2[o].y);
        for(unsigned int o = 0; o<troopCoords3.size(); o++)
            troop2->drawTroop(troopCoords3[o].x, troopCoords3[o].y);
        for(unsigned int o = 0; o<troopCoords4.size(); o++)
            troop2->drawTroop(troopCoords4[o].x, troopCoords4[o].y);
        for(unsigned int o = 0; o<farmerCoords1.size(); o++)
            farmer2->drawFarmer(farmerCoords1[o].x, farmerCoords1[o].y);
        for(unsigned int o = 0; o<farmerCoords2.size(); o++)
            farmer2->drawFarmer(farmerCoords2[o].x, farmerCoords2[o].y);
        for(unsigned int o = 0; o<farmerCoords3.size(); o++)
            farmer2->drawFarmer(farmerCoords3[o].x, farmerCoords3[o].y);
        for(unsigned int o = 0; o<farmerCoords4.size(); o++)
            farmer2->drawFarmer(farmerCoords4[o].x, farmerCoords4[o].y);
        for(unsigned int o = 0; o<engineerCoords1.size(); o++)
            engineer2->drawEngineer(engineerCoords1[o].x, engineerCoords1[o].y);
        for(unsigned int o = 0; o<engineerCoords2.size(); o++)
            engineer2->drawEngineer(engineerCoords2[o].x, engineerCoords2[o].y);
        for(unsigned int o = 0; o<engineerCoords3.size(); o++)
            engineer2->drawEngineer(engineerCoords3[o].x, engineerCoords3[o].y);
        for(unsigned int o = 0; o<engineerCoords4.size(); o++)
            engineer2->drawEngineer(engineerCoords4[o].x, engineerCoords4[o].y);
        for(unsigned int o = 0; o<minerCoords1.size(); o++)
            miner2->drawMiner(minerCoords1[o].x, minerCoords1[o].y);
        for(unsigned int o = 0; o<minerCoords2.size(); o++)
            miner2->drawMiner(minerCoords2[o].x, minerCoords2[o].y);
        for(unsigned int o = 0; o<minerCoords3.size(); o++)
            miner2->drawMiner(minerCoords3[o].x, minerCoords3[o].y);
        for(unsigned int o = 0; o<minerCoords4.size(); o++)
            miner2->drawMiner(minerCoords4[o].x, minerCoords4[o].y);
        window.display();
    }
    t1.join();
    t2.join();
    workers.join_all();

    } catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
