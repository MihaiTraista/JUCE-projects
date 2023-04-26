#pragma once

#include <JuceHeader.h>
#include "gameManager.h"
#include "movable.h"
#include "ball.h"
#include "paddle.h"
#include "input.h"
using namespace juce;

class MainComponent  : public Component, private Timer
{
public:
    MainComponent(){
        setSize(800, 600);
        initializeChildComponents();
        startTimer(33);
    }
    ~MainComponent() override{}

    void paint (juce::Graphics&) override{}
    void resized() override{}

private:
    void timerCallback() override{
        if(waitCounter < waitCount){
            waitCounter++;
            ball.setPosition(getWidth()/2, getHeight()/2);
            if(waitCounter == waitCount){
                Random rand(Time::currentTimeMillis());
                float randomYDelta = rand.nextFloat() * 60 - 30;
                int xDirection = (int)rand.nextBool() * 2 - 1;
                ball.setVisible(true);
                ball.setDelta(ballSpeed * xDirection, randomYDelta);
            }
        }
                
        setDeltaValues();

        paddleA.checkLimitsY();
        paddleB.checkLimitsY();
        ball.interactY();

        ball.interactX(paddleA, paddleB);
        bool restartRound = false;
        if( ball.lostBall(false) ){  //  check left
            GameManager::lostPoint(1);  //  refer to players by index
            restartRound = true;
        }
        else if( ball.lostBall(true) ){  //  check right
            GameManager::lostPoint(2);  //  refer to players by index
            restartRound = true;
        }
        if (restartRound){
            ball.setVisible(false);
            waitCounter = 0;
        }
        scoreLabel.setText(std::to_string(GameManager::playerOne.points) + "      " + std::to_string(GameManager::playerTwo.points), dontSendNotification);

        paddleA.move();
        paddleB.move();
        ball.move();
    }
    
    void initializeChildComponents()
    {
        addAndMakeVisible(input);
        input.setBounds(getBounds());
        addAndMakeVisible(paddleA);
        addAndMakeVisible(paddleB);
        paddleA.setBounds(getBounds().withY(getHeight()/2-paddleHeight/2).withWidth(paddleWidth).withHeight(paddleHeight));
        paddleB.setBounds(getBounds().withX(getWidth()-paddleWidth).withWidth(paddleWidth).withHeight(paddleHeight));
        paddleA.setLimits(0, 0, 0, getHeight() - paddleHeight);
        paddleB.setLimits(0, getWidth(), 0, getHeight() - paddleHeight);
        addAndMakeVisible(ball);
        ball.setBounds(getWidth()/2 - ballSize/2, getHeight()/2 - ballSize/2, ballSize, ballSize);
        ball.setLimits(0-ballSize*2, getWidth()+ballSize*2, 0, getHeight() - paddleWidth - ballSize);
        ball.setDelta(15, 8);
        addAndMakeVisible(scoreLabel);
        scoreLabel.setSize(200, 50);
        scoreLabel.setCentrePosition(getWidth()/2, 40);
        scoreLabel.setJustificationType(Justification::centred);
        scoreLabel.setFont(Font(50));
        addAndMakeVisible(infoLabel);
        infoLabel.setBounds(0, 0, getWidth(), 15);
        infoLabel.setJustificationType(Justification::centred);
        infoLabel.setFont(Font(15));
        infoLabel.setText("Player one moves with 'W' and 'S'. Player two moves with up and down keys.", dontSendNotification);
    }
    
    void setDeltaValues()
    {
        char moveCommand = input.getPressId();

        if((moveCommand >> 2) == 1)
            paddleA.setDelta(0, 10);
        else if((moveCommand >> 2) == 2)
            paddleA.setDelta(0, -10);
        else
            paddleA.setDelta(0, 0);

        if((moveCommand & 0x03) == 1)
            paddleB.setDelta(0, 10);
        else if((moveCommand & 0x03) == 2)
            paddleB.setDelta(0, -10);
        else
            paddleB.setDelta(0, 0);
    }

    int paddleWidth = 14, paddleHeight = 200, ballSize = 20, waitCounter = 10, waitCount = 10, ballSpeed = 15;
    Paddle paddleA, paddleB;
    Ball ball;
    Input input;
    Label scoreLabel;
    Label infoLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
