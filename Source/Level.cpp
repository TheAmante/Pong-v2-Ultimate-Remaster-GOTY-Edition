#include "Level.h"
#include <iostream>
#include <Windows.h>
#include <stdlib.h>

Level::Level(float size) :
m_size(size), 
m_balls(), 
m_edges(), 
m_players(), 
m_textScores(),
m_ground(sf::Vector2f(size, size)),
m_centerPosition(sf::Vector2f(size/2, size/2))
{
	// Init scores
	m_scores = new int[4];
	for (int i = 0; i < 4; i++)
		m_scores[i] = 0;

	// Load font text
	m_font.loadFromFile("../Resource/Fonts/bromello.ttf");

	// Build level
	this->buildLevel();
}

Level::~Level()
{}

void Level::buildLevel()
{
	// Init ground
	m_ground.setFillColor(sf::Color::Black);

	// Init balls
	float random = rand() % 10 - 5;
	sf::Vector2f randomDirectionBall(0.5, 1);// rand() % 10 - 5, rand() % 10 - 5);
	randomDirectionBall /= sqrt(randomDirectionBall.x*randomDirectionBall.x + randomDirectionBall.y*randomDirectionBall.y);
	
	// std::cout << randomDirectionBall.x << " " << randomDirectionBall.y << std::endl;
	m_balls.push_back(new Ball(10, 0.1, m_centerPosition, randomDirectionBall));

	// Init players
	m_players.push_back(new Racquet(sf::Vector2f(100.f, 20.f), sf::Vector2f(m_size/2, 20.f), 0.2f, sf::Color::Red));
	m_players.back()->setRight(sf::Keyboard::Right);
	m_players.back()->setLeft(sf::Keyboard::Left);

	m_players.push_back(new Racquet(sf::Vector2f(100.f, 20.f), sf::Vector2f(m_size/2, m_size-20.f), 0.2f, sf::Color::Red));
	m_players.back()->setRight(sf::Keyboard::Right);
	m_players.back()->setLeft(sf::Keyboard::Left);

	// Init edges
	m_edges.push_back(new Edge(sf::Vector2f(m_size, 10.f), sf::Vector2f(m_size / 2, 5.f), sf::Color::White));
	m_edges.push_back(new Edge(sf::Vector2f(m_size, 10.f), sf::Vector2f(m_size / 2, m_size-5.f), sf::Color::White));
	m_edges.push_back(new Edge(sf::Vector2f(10.f, m_size), sf::Vector2f(5.f, m_size / 2), sf::Color::White));
	m_edges.push_back(new Edge(sf::Vector2f(10.f, m_size), sf::Vector2f(m_size-5.f, m_size/2), sf::Color::White));

	// Init texts scores
	m_textScores.push_back(new sf::Text());
	m_textScores.back()->setColor(sf::Color::Green);
	m_textScores.back()->setFont(m_font);
	m_textScores.back()->setCharacterSize(20);

	m_textScores.push_back(new sf::Text());
	m_textScores.back()->setColor(sf::Color::Green);
	m_textScores.back()->setFont(m_font);
	m_textScores.back()->setCharacterSize(20);
}

void Level::draw(sf::RenderWindow & window)
{
	window.clear();
	window.draw(m_ground);
	
	// Draw all balls
	for (int i = 0; i < m_balls.size(); i++)
		m_balls[i]->draw(window);

	// Draw all players
	for (int i = 0; i < m_players.size(); i++)
		m_players[i]->draw(window);
	
	// Draw all edges
	for (int i = 0; i < m_edges.size(); i++)
		m_edges[i]->draw(window);

	// Draw HUD
	hud(window);

	window.display();
}

void Level::hud(sf::RenderWindow & window)
{
	// Draw all texts scores
	for (int i = 0; i < m_textScores.size(); i++)
	{
		m_textScores[i]->setString("Player "+ std::to_string(i + 1) + " = " + std::to_string(m_scores[i]));
		m_textScores[i]->setPosition(m_players[i]->getPosition());
		window.draw(*m_textScores[i]);
	}
}

void Level::update()
{
	// Compute balls bounds
	ballBound();

	// Update balls target
	for (int i = 0; i < m_balls.size(); i++)
		m_balls[i]->update();

	// Update players target
	for (int i = 0; i < m_players.size(); i++)
		m_players[i]->update();
}

void Level::ballBound()
{
	// Compute bounds between players and balls
	for (int i = 0; i < m_players.size(); i++)
		for (int j = 0; j < m_balls.size(); j++)
			if (m_balls[j]->getBox().intersects(m_players[i]->getBox()))
				m_balls[j]->setDirection(m_players[i]->getBoundDirection(m_balls[j]));

	// Compute bounds between players and edges
	for (int i = 0; i < m_players.size(); i++)
		for (int j = 0; j < m_edges.size(); j++)
			if (m_edges[j]->getBox().intersects(m_players[i]->getBox()))
			{
				if (m_players[i]->getPosition().x < m_size/2)
					m_players[i]->setPosition(sf::Vector2f(m_players[i]->getSize().x/2 + 5, m_players[i]->getPosition().y)); // +5 : Epaisseur du mur -> pr�voir m�thode edge.getSize

				if (m_players[i]->getPosition().x > m_size/2)
					m_players[i]->setPosition(sf::Vector2f(m_size - m_players[i]->getSize().x/2 - 5 , m_players[i]->getPosition().y));
			
				if (m_players[i]->getPosition().y < m_size / 2)
					m_players[i]->setPosition(sf::Vector2f(m_players[i]->getPosition().x, m_players[i]->getSize().y / 2 + 5));
				
				if (m_players[i]->getPosition().y > m_size / 2)
					m_players[i]->setPosition(sf::Vector2f(m_players[i]->getPosition().x, m_size - m_players[i]->getSize().y / 2 - 5));
			}

	// Compute bounds between edges and balls
	for(int i=0; i<m_edges.size(); i++)
		for (int j = 0; j < m_balls.size(); j++)
		{
			if (m_balls[j]->getBox().intersects(m_edges[i]->getBox()))
			{
				m_balls[j]->setDirection(m_edges[i]->getBoundDirection(m_balls[j]));
			}
		}
}

void Level::restartLevel()
{
	// Clear the game
	m_balls.clear();
	m_players.clear();
	m_edges.clear();
	m_textScores.clear();

	buildLevel();
}