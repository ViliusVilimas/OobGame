#include <SFML/Graphics.hpp>

#include <SFML/Graphics.hpp>
#include "Classes.h"
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({1920u, 1080u}), "Game Test");
    window.setFramerateLimit(60);

    Player player;
    Enemy enemy(70, 3, 30, 30);

    std::vector<Items> items;
    items.resize(10);

    items[0].name = "Sword";
    items[0].price = 30;
    items[0].descripction = "Increases damage dealt to enemies by 50%";

    items[1].name = "Big Heart";
    items[1].price = 50;
    items[1].descripction = "Doubles your current HP";

    items[2].name = "Shield";
    items[2].price = 45;
    items[2].descripction = "Reduces damage";

    items[3].name = "Vampire Fang";
    items[3].price = 40;
    items[3].descripction = "Heal 10% of enemy HP after kill";

    items[4].name = "Golden Ring";
    items[4].price = 60;
    items[4].descripction = "Double gold from kills";

    items[5].name = "Wisdom Orb";
    items[5].price = 60;
    items[5].descripction = "Double EXP from kills";

    StoreGraphics storeUI;
    StoreLogic storeLogic;
    StoreController store(storeUI, storeLogic);
    CombatGraphics combatgraphics;
    CombatLogic combatLogic;
    sf::Font font;
    font.openFromFile("fonts/PlayfulTime-BLBB8.ttf");

    bool inShop = true;
    bool inCombat = true;

    while (window.isOpen()) {
        while(player.getIsDead() == false) {
            int counter = 0;
            if (inCombat){
                combatLogic.runCombat(player, enemy, items, combatgraphics, window);
                inShop = true;
                inCombat = false;

            }
            if (inShop) {
                store.open(player, window, items);
                inShop = false;
                inCombat = true;

            }
            counter++;
            enemy.scaleByLevel(counter);
        }

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (const auto* e = event->getIf<sf::Event::Closed>()) {
                    window.close();
                }
            }

            window.clear(sf::Color::Black);

            sf::Text text(font);
            text.setFont(font);
            text.setFillColor(sf::Color::Green);
            text.setCharacterSize(48);
            text.setString("Shop closed.\nPlayer HP: " + std::to_string((int)player.getHp()) +
                           "\nGold: " + std::to_string(player.getGold()) +
                           "\n\nClose window to exit.");
            text.setPosition({200.f, 200.f});

            window.draw(text);
            window.display();
        }
    }

    return 0;
}
