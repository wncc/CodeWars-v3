// To be written in a multithreaded manner

// Game class would have the constructor function as the one main thread, and the others would mere

// Threading!
// select() -> polling method :
#ifndef GAME_FACTORS_H
#define GAME_FACTORS_H

#include <unistd.h>
#include <stdlib.h>


struct Kingdom
{
    int num;
    int defence_rating=0; // measure of how good the defense of a kingdom is
    int offence_rating=0; // measure of how good the attack/offense of a kingdom is
    int num_troops=10; //stores the total number of troops in the population not in the battlefield
    int num_spies=0;  // stores the total number of spies in the population not in the battlefield
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
    bool in = false;

    float percent_occ[4]={0,0,0,0};
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
        if(food == 0){
            sleep(1);
            return 0;
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
        if(food == 0){
            sleep(1);
            return 0;
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
        if(food == 0){
            sleep(1);
            return 0;
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
        if(food == 0){
            sleep(1);
            return 0;
        }
        num_spies = num_spies + n/10;
        defence_rating = defence_rating + n/4.0;
        food = food -(n/100.00)*food;
        sleep(1);
        return true;
    }


    bool improve_attack(float fraction)
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
    bool improve_defense(float fraction)
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
        if(k->num != num){
        if(n>=100){
            n=100;
        }
        if(n<=0){
            n=0;
            return -1;
        }
        int ans=0;
        if(!offence_rating < 0.001){
            ans = ((-k->defence_rating + (n/100.0)*offence_rating)/((n/100.0)*offence_rating))*100.0; 
            if(ans>0){
                num_troops -= (((100.00-ans)*n)/10000.0)*num_troops;
                land_area+=(ans/2)*land_area/100.0;
                k->percent_occ[num]+=ans;
                food -= food*(n/100.0);
            }
            else{
                num_troops -= (n/100.0)*num_troops;
                food -= food*(n/100.0);
            } 
        }
        //std::cout<<"Percent_occ "<<k->percent_occ[num]<<std::endl;
        int count= 0;
        for(int i=0; i<4; i++){
            count+= k->percent_occ[i];
            if(count >=100)
                count = 100;
        }
        std::cout<<"\033[1;31mPlayer \033[0m"<<k->num<<"\033[1;31m has lost \033[0m"<<count<<"\033[1;31m \% of their land\033[0m"<<std::endl;
        if(count>=100){
            k->lost= true;
            if(!k->in){
                std::cout<<"Player "<<k->num<<" has been defeated by player "<<num<<" !!"<<std::endl;
                k->in = true;
            }
        }
        sleep(1);
        return ans;
        }
        return 0;
    }

};

Kingdom arr[4];
#endif