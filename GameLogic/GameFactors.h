// To be written in a multithreaded manner

// Game class would have the constructor function as the one main thread, and the others would mere

// Threading!
// select() -> polling method :
#ifndef GAME_FACTORS_H
#define GAME_FACTORS_H

#include <unistd.h>


struct Kingdom
{
    int num;
    int defence_rating=0; // measure of how good the defense of a kingdom is
    int offence_rating=0; // measure of how good the attack/offense of a kingdom is
    int num_troops=10; //stores the total number of troops in the population not in the battlefield
    int num_spies;  // stores the total number of spies in the population not in the battlefield
    int num_farmers=10; //stores the total number of farmers in the population
    int num_engineers=10; //stores the total number of engineers in the population
    int num_miners=10; //stores the total number of miners in the population
    int raw_materials = 100; // amount of gold in the inventory which is initially 100
    int food = 100;          // amount of food in the inventory
    char loc[1000][1000]; //defining location as 2d array (1000*1000 canvas)
    // The territory of each kingdom is supposed to be in square shape, thus the boundaries having their length and width have been defined below

    int land_width=10;
    int land_length=10;
    int land_area = land_length * land_width;

    float percent_occ[4]={0,0,0,0};

    char attack_square[1000][1000]; //can take values 'r', 'y' and by default its 'g'
    bool lost = false;


    bool recruit_farmers(int n){
        if(n>100){
            n=100;
        }
        if(n<0){
            n=0;
            return false;
        }
        num_farmers = num_farmers + n/10;
        food=food-(n/100.00)*food;
        sleep(1);
        return true;
    } 
    bool recruit_engineers(int n){
        if(n>100){
            n=100;
        }
        if(n<0){
            n=0;
            return false;
        }
        num_engineers = num_engineers + n/10;
        food = food -(2*n/100.00)*food;
        sleep(1);
        return true;
    }
    bool recruit_troops(int n){
        if(n>100){
            n=100;
        }
        if(n<0){
            n=0;
            return false;
        }
        num_troops = num_troops + n/10;
        food = food - (n/100.00)*food;
        offence_rating = offence_rating + n/4.0;
        sleep(1);
        return true;
    }
    bool recruit_miners(int n){
        if(n>100){
            n=100;
        }
        if(n<0){
            n=0;
            return false;
        }
        num_miners = num_miners + n/10;
        food = food -(n/100.00)*food;
        sleep(1);
        return true;
    }

    bool recruit_spies(int n){
        if(n>100){
            n=100;
        }
        if(n<0){
            n=0;
            return false;
        }
        num_spies = num_spies + n/10;
        defence_rating = defence_rating + n/4.0;
        food = food -(n/100.00)*food;
        sleep(1);
        return true;
    }


    bool improve_attack(int raw_materials, float fraction)
    {
        if(fraction>=1){
            fraction=1;
        }
        if(fraction<=0){
            fraction=0;
            return false;
        }
        if(raw_materials >=0){
            offence_rating = offence_rating + num_engineers * raw_materials * fraction*0.01;
            raw_materials-=(raw_materials)*(fraction)*0.5;
            return true;
        }
        sleep(1);
        return false;
    } // increases attack capabilities per second
    bool improve_defense(int raw_materials, float fraction)
    {
        if(fraction>=1){
            fraction=1;
        }
        if(fraction<=0){
            fraction=0;
            return false;
        }
        if(raw_materials >=0){
            defence_rating = defence_rating + num_spies * raw_materials * (fraction)*0.01 + num_engineers * raw_materials * (fraction)*0.01;
            raw_materials-=(raw_materials)*(fraction)*0.5;
            return true;
        }
        sleep(1);
        return false;
        
    } // increases defense capabilities per second
    bool spy_capture(Kingdom *k, int n)
    {
        if(n>=100){
            n=100;
        }
        if(n<=0){
            n=0;
            return false;
        }
        k->num_spies = k->num_spies - (n/100)*(k->num_spies);
        k->defence_rating = (k->defence_rating)*(1-(1/200.0));
        raw_materials-=(n/100.0)*(raw_materials);
        sleep(1);
        return true;
    }


    int attack(Kingdom *k, int n) // Attack on kingdom k with n percent of your soldiers
    { 
        if(n>=100){
            n=100;
        }
        if(n<=0){
            n=0;
            return -1;
        }
        int ans=0;
        if(k->defence_rating < 0.001)
            ans = 80;
        else{
            ans = ((-k->defence_rating + offence_rating)/(k->defence_rating))*100.0; 
            num_troops -= (ans*n/400.0)*num_troops;
        }
        food -= food*(n/100.0);
        k->percent_occ[num]+=ans;
        //std::cout<<"Percent_occ "<<k->percent_occ[num]<<std::endl;
        int count= 0;
        for(int i=0; i<4; i++){
            count+= k->percent_occ[i];
        }
        if(count>=100){
            k->lost= true;
        }
        sleep(1);
        return ans;
    }

};

Kingdom arr[4];
#endif