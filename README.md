# Codewars-v3
Welcome to Codewars-v3!</br>
Build the strongest village and capture enemy territory to win!</br>
---
# Installations:</br>
### For Linux users:</br>
```
sudo apt-get install libboost-all-dev #to install boost
sudo apt-get install libsfml-dev      #to install sfml
```
### For Windows and Mac users:</br>
FOr boost, use [this](https://www.geeksforgeeks.org/how-to-install-c-boost-libraries-on-windows/) for windows and [this](https://www.geeksforgeeks.org/how-to-install-boost-library-in-c-on-macos/) for mac.[This](https://www.boost.org/users/history/version_1_81_0.html) is boost's official site.</br>
Use [this](https://www.sfml-dev.org/download/sfml/2.3.1/) for SFML

# Instructions to run the code:</br>
Create an account on github and download this repository.</br>
Create a directory "Object" in the CodeWars-v3 folder. </br>
Run the compile.py file as follows (this only needs to be run once):</br>
```
python compile.py
```
Add your Python code team_name.py in any of the player files.
and run them:</br>
```
python <team_name>.py > team_number.txt
```

Finally run the following in separate terminals inside the Object directory:</br>
Note that the game will start only until <number_of_players> clients have started
```
./main <number_of_players> 6000
./client <player_number0> 127.0.0.1 6000
./client <player_number1> 127.0.0.1 6000
```
```
#for example, ./client 0 127.0.0.1 6000
```
Runs the file in player0