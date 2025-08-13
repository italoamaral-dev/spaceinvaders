# Space Invaders

This is a classroom prototype project developed to practice programming skills through a game — specifically, Space Invaders.
The main objective is to create a spaceship that moves freely across the screen and shoots bullets to destroy as many aliens as possible. The game ends if any alien reaches the ground or collides with the spaceship, and the player’s score is then added to the high score record.

## Table of contents

- [Overview](#overview)
  - [The challenge](#the-challenge)
- [My process](#my-process)
  - [Built with](#built-with)
  - [What I learned](#what-i-learned)
  - [Continued development](#continued-development)
  - [Useful resources](#useful-resources)
- [Author](#author)
- [Acknowledgments](#acknowledgments)


## Overview

### The challenge

Users should be able to:

- Create a triangular-shaped spaceship.
- Create rectangular-shaped aliens.
- Shoot bullets from the spaceship to hit the aliens.
- Display the player’s score every time an alien is hit.
- End the game if an alien reaches the ground or collides with the spaceship.


## My process

### Built with

- Allegro Library.
- Pure C language.


### What I learned

Through this project, I had the opportunity to apply most of the programming concepts learned during the first semester of my Computer Science degree in a practical and extensive way.

I was able to:

- Develop more complex functions using arrays and matrices.
- Work with external data and fonts.
- Gain a deeper understanding of how pure C can be applied in game development.

A function I’m particularly proud of:

```c
void updateAliens(Alien a[NUM_LINHAS][NUM_COLUNAS]){
    int inv = 0;

    for(int i=0;i<NUM_LINHAS;i++){
        for(int j=0;j<NUM_COLUNAS;j++){
            Alien *ali = &a[i][j];
            if(ali->vivo){
                if(ali->x + ALIEN_W + ali->x_vel > SCREEN_W || ali->x + ali->x_vel < 0){
                    inv = 1;
                }
            }
        }
    }

    for(int i=0;i<NUM_LINHAS;i++){
        for(int j=0;j<NUM_COLUNAS;j++){
            Alien *ali = &a[i][j];
            if(ali->vivo){
                if(inv){
                    ali->y += ali->y_vel;
                    ali->x_vel *= -1;
                }
                ali->x += ali->x_vel;
            }
        }
    }
}
```
This function allowed me to explore logic for group movement and collision detection, strengthening my understanding of how to translate game mechanics into code.


### Continued development

Moving forward, I want to focus on improving my problem-solving skills for complex situations that arise during software development. Continuing to practice low-level programming with pure C will also be crucial to becoming a better programmer.


### Useful resources

- [Starting a Space Invaders Prototype with Allegro](https://www.youtube.com/watch?v=FhlHDtLTek0) These videos, produced in Portuguese, helped me understand the methods used to develop a Space Invaders-style game using the Allegro library.


## Author

- [LinkedIn](https://www.linkedin.com/in/italosamaral)  
- Email - italoamaral015@gmail.com  
- [Instagram](https://www.instagram.com/_italoamaral)
