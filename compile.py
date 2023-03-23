import os
from time import sleep

os.system("g++ -o Object/main GameEngine/main.cpp -lboost_system -lboost_thread -lboost_timer -pthread -lsfml-graphics -lsfml-window -lsfml-system ")
os.system("g++ -o Object/client GameEngine/client.cpp -lboost_system -lboost_timer -lboost_thread -pthread")
